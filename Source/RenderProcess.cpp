#include "RenderProcess.h"

#include "_DxLib.h"
#include "Camera.h"
#include "ConfigData.h"


// 主描画用画面
// 一旦ここに全ての描画処理後の結果を描画してから裏画面に（必要なら拡大縮小して）描画する
int MainScreenHandle;

//int MaskHandle;


// ゲームメインの描画処理を初期化する
void GameMainRender_Initialize() {
	extern ConfigData cConfigData;
	if (cConfigData.AntialiasFlag) {
		// アンチエイリアスレンダリング用の描画対象にできるグラフィックハンドルを作成する
		SetCreateDrawValidGraphMultiSample(4, 4);
		MainScreenHandle = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
		SetCreateDrawValidGraphMultiSample(0, 0);
	}
	else {
		SetCreateDrawValidGraphMultiSample(0, 0);
		MainScreenHandle = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
	}

	// マスク画面を作成します
	//CreateMaskScreen();

	// 空のマスクを作成します
	//MaskHandle = LoadMask(_T("BlendGraph.bmp"));

}

// ゲームメインの描画処理の後始末をする
void GameMainRender_Terminate() {
	// 主描画用画面を削除
	DeleteGraph(MainScreenHandle);

	//DeleteMask(MaskHandle);

	// マスク画面を削除します
	//DeleteMaskScreen();
}

void GameMainRender_FrontProcess() {
	// 描画先を主描画用画面に設定
	SetDrawScreen(MainScreenHandle);

	ClearDrawScreen();

	// SetDrawScreen を実行するとカメラの設定がリセットされてしまうので、
	// カメラの設定を再度実行
	Camera_Setup();
}

void GameMainRender_RearProcess() {
	extern int WINDOWS_H, WINDOWS_V;
	
	// 描画先を裏画面に設定
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	//DrawMask(0, 0, MaskHandle, DX_MASKTRANS_NONE);

	// 主描画用画面を裏画面に描画
	DrawExtendGraph(0, 0, WINDOWS_H, WINDOWS_V, MainScreenHandle, FALSE);

	// SetDrawScreen を実行するとカメラの設定がリセットされてしまうので、
	// カメラの設定を再度実行
	Camera_Setup();
}