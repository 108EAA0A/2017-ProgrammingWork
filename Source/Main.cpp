#include <crtdbg.h>

#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "ConfigData.h"
#include "SceneMgr.h"
#include "FPSCounter.h"

#include "DataIO.h"
#include "RenderProcess.h"
#include "Screenshot.h"
#include "ShadowControl.h"
#include "UpdateKey.h"

#define GAME_VERSION_STR "1.0"

::Effekseer::Manager *EfMgr;// EffekseerのManagerクラス
//static ::EffekseerRendererDX9::Renderer *EfRend;

CFPSCounter cFPScounter(10);

static bool ProcessLoop() {
	
	if (ScreenFlip() == -1) return false;		//画面更新
	if (ProcessMessage() == -1) return false;	//メッセージ処理
	if (ClearDrawScreen() == -1) return false;	//画面消去
	if (!UpdateKey()) return false;				//キー入力取得
	
	return true;
}

static void EndProcess() {
	Effkseer_End();
	DxLib_End();
}

static void errPrintExit(const char *const errpos, const char *const errmsg) {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	clsDx();
	printfDx("例外発生\n%s処理内：%s\n", errpos, errmsg);
	ScreenFlip();
	WaitKey();
	std::exit(EXIT_FAILURE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//メモリリーク検出ツール
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::atexit(EndProcess);

	SetUseDXArchiveFlag(TRUE);
	//SetDXArchiveExtension(char *Extension);
	//SetDXArchiveKeyString(char *KeyString);
	SetDXArchiveKeyString("MyPassWord");
	SetDoubleStartValidFlag(FALSE);
	SetAlwaysRunFlag(TRUE);
#ifdef _DEBUG
	SetLogDrawOutFlag(TRUE);
#else
	SetLogDrawOutFlag(FALSE);
#endif

	LoadConfig();
	
	char titlebuf[40];
	snprintf(titlebuf, sizeof(titlebuf), "MMQ ver%s on DxLib ver%s", GAME_VERSION_STR, DXLIB_VERSION_STR_T);
	SetMainWindowText(titlebuf);

	// DirectX9(EXもNG)を使用するようにする。
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_9);

	SetZBufferBitDepth(32);

	SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
	extern ConfigData cConfigData;
	ChangeWindowMode(!cConfigData.FullScr);
	//ChangeWindowMode(TRUE);	// フルスクリーンからウィンドウモードに変える

	// 垂直同期を設定する
	SetWaitVSyncFlag(cConfigData.VSyncFlag);

	// 描画モードを設定する
	SetDrawMode(cConfigData.DrawMode);
	
	extern int WINDOWS_H, WINDOWS_V;
	SetGraphMode(WINDOWS_H, WINDOWS_V, 32);

	if (DxLib_Init() == -1) {	// ＤＸライブラリ初期化処理 エラーが起きたら直ちに終了
		printfDx("DxLib Init error\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	SetDrawScreen(DX_SCREEN_BACK); //裏画面設定
	{ int dustbox; GetScreenState(&WINDOWS_H, &WINDOWS_V, &dustbox); }

	if (GetValidShaderVersion() < 200) {
		printfDx("シェーダーモデル2.0が使用できません\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(10000) == -1) {
		printfDx("Effekseer Init error\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	// 歪み機能初期化
	Effekseer_InitDistortion();

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	
	SetMousePoint(WINDOWS_H / 2, WINDOWS_V / 2);
	SetMouseDispFlag(FALSE);
	// マウスカーソルのウィンドウ外への移動を禁止
	//SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);

	SetUseZBufferFlag(TRUE); // Ｚバッファを有効にする
	SetWriteZBufferFlag(TRUE); // Ｚバッファへの書き込みを有効にする

	SetUseZBuffer3D(TRUE); // ３ＤのＺバッファを有効にする
	SetWriteZBuffer3D(TRUE); // ３ＤのＺバッファへの書き込みを有効にする
	
	extern unsigned int KeyFrame[256];

	SceneMgr sceneMgr;
	sceneMgr.Initialize();

	GameMainRender_Initialize();

	register LONGLONG RefreshTime = 0;

	while (ProcessLoop()) {
		RefreshTime = GetNowHiPerformanceCount(); //今の時間を取得

		EfMgr = GetEffekseer3DManager();

		try {
			sceneMgr.Update();  //更新
		}
		catch (const char *const errmsg) {
			if (errmsg == "FORCEEND") break;
			errPrintExit("更新", errmsg);
		}
		catch (const exception& e) {
			errPrintExit("更新", e.what());
		}

		try {
			GameMainRender_FrontProcess();
			sceneMgr.Draw();    //描画
			GameMainRender_RearProcess();
		}
		catch (const char *const errmsg) {
			if (errmsg == "FORCEEND") break;
			errPrintExit("描画", errmsg);
		}
		catch (const exception& e) {
			errPrintExit("描画", e.what());
		}

		if (KeyFrame[KEY_INPUT_F12] == 1) Screenshot();

		while (GetNowHiPerformanceCount() - RefreshTime < 16667); //1周の処理が16.667ミリ秒になるまで待つ
	}

	sceneMgr.Finalize();

	GameMainRender_Terminate();

	return 0;
}