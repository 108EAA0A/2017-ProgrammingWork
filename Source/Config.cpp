#include "Config.h"

#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "ConfigData.h"
#include "DataIO.h"
#include "RenderProcess.h"
#include "Screenshot.h"
#include "Structure.h"
#include "UpdateKey.h"

enum eConfig {
	eConfig_MasterVol,	//主音量設定
	eConfig_BGMVol,		//BGM音量設定
	eConfig_SEVol,		//SE音量設定
	eConfig_WindowMode,	//ウィンドウモード設定
	eConfig_Resolution,	//解像度設定
	eConfig_DrawMode,	//描画モード設定
	eConfig_AntiAlias,	//アンチエイリアス設定
	eConfig_VSync,		//垂直同期設定
	eConfig_Shadow,		//影設定
	eConfig_Reflection,	//反射設定
	eConfig_SSType,		//スクリーンショット形式設定
	eConfig_Back,		//戻る

	eConfig_Num,		//本項目の数
};

extern unsigned int KeyFrame[256];

extern int WINDOWS_H, WINDOWS_V;

static int NowSelect = eConfig_MasterVol;    //現在の選択状態(初期は第1項目)

static const int MasterVol_Y = 240;
static const int BGMVol_Y = 270;
static const int SEVol_Y = 300;
static const int WindowMode_Y = 330;
static const int Resolution_Y = 360;
static const int DrawMode_Y = 390;
static const int Antialias_Y = 420;
static const int VSync_Y = 450;
static const int Shadow_Y = 480;
static const int Reflection_Y = 510;
static const int SSType_Y = 540;
static const int BACK_Y = 570;

Config::Config(ISceneChanger *const changer) : BaseScene(changer) {
}

//初期化
void Config::Initialize() {
	SetFontSize(16);
}

//終了処理
void Config::Finalize() {
	SaveConfig();
	ClearDrawScreen();
	ScreenFlip();
}

//更新
void Config::Update() {
	BaseScene::Draw();//親クラスの描画メソッドを呼ぶ
	
	extern ConfigData cConfigData;
	static int PrevResolution = cConfigData.WindowResolution;

	if (KeyFrame[KEY_INPUT_ESCAPE] != 0) {//Escキーが押されていたら
		mSceneChanger->ChangeScene(eScene_Title);//シーンをタイトルに変更
	}
	if (KeyFrame[KEY_INPUT_DOWN] == 1) {//下キーが押されていたら
		NowSelect = (NowSelect + 1) % eConfig_Num;	//選択状態を一つ下げる
	}
	if (KeyFrame[KEY_INPUT_UP] == 1) {//上キーが押されていたら
		NowSelect = (NowSelect + (eConfig_Num - 1)) % eConfig_Num;	//選択状態を一つ上げる
	}

	if (KeyFrame[KEY_INPUT_LEFT] > 50) {
		switch (NowSelect) {			//現在選択中の状態によって処理を分岐
		case eConfig_MasterVol:
			cConfigData.MasterVol -= 2;
			if (cConfigData.MasterVol < 0x00) cConfigData.MasterVol = 0x00;
			break;
		case eConfig_BGMVol:
			cConfigData.BGMVol -= 2;
			if ((cConfigData.BGMVol == 0xFE) || (cConfigData.BGMVol == 0xFF)) cConfigData.BGMVol = 0x00;
			break;
		case eConfig_SEVol:
			cConfigData.SEVol -= 2;
			if ((cConfigData.SEVol == 0xFE) || (cConfigData.SEVol == 0xFF)) cConfigData.SEVol = 0x00;
			break;
		}
	}
	if (KeyFrame[KEY_INPUT_RIGHT] > 50) {
		switch (NowSelect) {			 //現在選択中の状態によって処理を分岐
		case eConfig_MasterVol:
			cConfigData.MasterVol += 2;
			if (cConfigData.MasterVol > 100) cConfigData.MasterVol = 100;
			break;
		case eConfig_BGMVol:
			cConfigData.BGMVol += 2;
			if ((cConfigData.BGMVol == 0x00) || (cConfigData.BGMVol == 0x01)) cConfigData.BGMVol = 0xFF;
			break;
		case eConfig_SEVol:
			cConfigData.SEVol += 2;
			if ((cConfigData.SEVol == 0x00) || (cConfigData.SEVol == 0x01)) cConfigData.SEVol = 0xFF;
			break;
		}
	}

	if (KeyFrame[KEY_INPUT_LEFT] == 1) {//左キーが押されていたら
		switch (NowSelect) {			//現在選択中の状態によって処理を分岐
			case eConfig_MasterVol:
				--cConfigData.MasterVol;
				if (cConfigData.MasterVol < 0x00) cConfigData.MasterVol = 0x00;
				break;
			case eConfig_BGMVol:
				if (cConfigData.BGMVol != 0x00) --cConfigData.BGMVol;
				break;
			case eConfig_SEVol:
				if (cConfigData.SEVol != 0x00) --cConfigData.SEVol;
				break;
			case eConfig_WindowMode:
				cConfigData.FullScr = false;
				ChangeWindowMode(TRUE);
				extern int MainScreenHandle;
				SetDrawScreen(MainScreenHandle);
				SetMouseDispFlag(FALSE);
				Effekseer_InitDistortion();	// 歪み機能初期化
				break;
			case eConfig_Resolution:
				if (cConfigData.WindowResolution != BlackBerry) --cConfigData.WindowResolution;
				break;
			case eConfig_DrawMode:
				cConfigData.DrawMode = (cConfigData.DrawMode + (DX_DRAWMODE_NUM - 2)) % (DX_DRAWMODE_NUM - 1);
				break;
			case eConfig_AntiAlias:
				cConfigData.AntialiasFlag = false;
				GameMainRender_Terminate();
				GameMainRender_Initialize();
				break;
			case eConfig_VSync:
				cConfigData.VSyncFlag = false;
				break;
			case eConfig_Shadow:
				cConfigData.ShadowLevel = (cConfigData.ShadowLevel + (LevelNum - 1)) % LevelNum;
				break;
			case eConfig_Reflection:
				cConfigData.ReflectionLevel = (cConfigData.ReflectionLevel + (LevelNum - 1)) % LevelNum;
				break;
			case eConfig_SSType:
				cConfigData.SSType = (cConfigData.SSType + (eSSType_Num - 1)) % eSSType_Num;
				break;
		}
	}
	
	if (KeyFrame[KEY_INPUT_RIGHT] == 1) {//右キーが押されていたら
		switch (NowSelect) {			 //現在選択中の状態によって処理を分岐
			case eConfig_MasterVol:
				++cConfigData.MasterVol;
				if (cConfigData.MasterVol > 100) cConfigData.MasterVol = 100;
				break;
			case eConfig_BGMVol:
				if (cConfigData.BGMVol != 0xFF) ++cConfigData.BGMVol;
				break;
			case eConfig_SEVol:
				if (cConfigData.SEVol != 0xFF) ++cConfigData.SEVol;
				break;
			case eConfig_WindowMode:
				cConfigData.FullScr = true;
				ChangeWindowMode(FALSE);
				extern int MainScreenHandle;
				SetDrawScreen(MainScreenHandle);
				SetMouseDispFlag(FALSE);
				Effekseer_InitDistortion();	// 歪み機能初期化
				break;
			case eConfig_Resolution:
				if (cConfigData.WindowResolution != FullHD) ++cConfigData.WindowResolution;
				break;
			case eConfig_DrawMode:
				cConfigData.DrawMode = (cConfigData.DrawMode + 1) % (DX_DRAWMODE_NUM - 1);
				break;
			case eConfig_AntiAlias:
				cConfigData.AntialiasFlag = true;
				GameMainRender_Terminate();
				GameMainRender_Initialize();
				break;
			case eConfig_VSync:
				cConfigData.VSyncFlag = true;
				break;
			case eConfig_Shadow:
				cConfigData.ShadowLevel = (cConfigData.ShadowLevel + 1) % LevelNum;
				break;
			case eConfig_Reflection:
				cConfigData.ReflectionLevel = (cConfigData.ReflectionLevel + 1) % LevelNum;
				break;
			case eConfig_SSType:
				cConfigData.SSType = (cConfigData.SSType + 1) % eSSType_Num;
				break;
		}
	}

	if (PrevResolution != cConfigData.WindowResolution) {
		switch (cConfigData.WindowResolution) {
			case BlackBerry: WINDOWS_H = 640;  WINDOWS_V = 480;  break;
			case SVGA:		 WINDOWS_H = 800;  WINDOWS_V = 600;  break;
			case XGA:		 WINDOWS_H = 1024; WINDOWS_V = 768;  break;
			case SXGA:		 WINDOWS_H = 1280; WINDOWS_V = 1024; break;
			case HDTV:		 WINDOWS_H = 1080; WINDOWS_V = 720;  break;
			case FullHD:	 WINDOWS_H = 1920; WINDOWS_V = 1080; break;
		}
		SetGraphMode(WINDOWS_H, WINDOWS_V, 32);
		SetMouseDispFlag(FALSE);
		// 歪み機能初期化
		Effekseer_InitDistortion();
	}

	if (KeyFrame[KEY_INPUT_RETURN] == 1) {//エンターキーが押されたら
		if (NowSelect == eConfig_Back) {
			mSceneChanger->ChangeScene(eScene_Title);//シーンをタイトルに変更
		}
	}

}

//描画
void Config::Draw() {
	extern ConfigData cConfigData;

	DrawFillBox(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, DxColor::Black);
	DrawString(200, 150, "設定画面です。", DxColor::White);
	DrawString(200, 170, "Escキーを押すとメニュー画面に戻ります。", DxColor::White);

	DrawFormatString(280, MasterVol_Y, DxColor::White, "マスター音量 : %d", cConfigData.MasterVol);
	DrawFormatString(280, BGMVol_Y, DxColor::White, "BGM音量 : %d", cConfigData.BGMVol);
	DrawFormatString(280, SEVol_Y, DxColor::White, "SE音量 : %d", cConfigData.SEVol);

	DrawFormatString(280, WindowMode_Y, DxColor::White, "フルスクリーン : %s", (GetWindowModeFlag() == TRUE ? "OFF" : "ON"));
	
	switch (cConfigData.WindowResolution) {
		case BlackBerry:	DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 640×480");   break;
		case SVGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 800×600");   break;
		case XGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 1024×768");  break;
		case SXGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 1280×1024"); break;
		case HDTV:			DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 1280×720");  break;
		case FullHD:		DrawFormatString(280, Resolution_Y, DxColor::White, "解像度 : 1920×1080"); break;
	}

	switch (cConfigData.DrawMode) {
		case DX_DRAWMODE_NEAREST:		DrawFormatString(280, DrawMode_Y, DxColor::White, "描画モード : ニアレストネイバー法");	break;
		case DX_DRAWMODE_BILINEAR:		DrawFormatString(280, DrawMode_Y, DxColor::White, "描画モード : バイリニア法");			break;
		case DX_DRAWMODE_ANISOTROPIC:	DrawFormatString(280, DrawMode_Y, DxColor::White, "描画モード : アンチエイリアス法");	break;
	}

	DrawFormatString(280, Antialias_Y, DxColor::White, "アンチエイリアス : %s", (cConfigData.AntialiasFlag ? "ON" : "OFF"));
	
	DrawFormatString(280, VSync_Y, DxColor::White, "VSync : %s", (cConfigData.VSyncFlag ? "ON" : "OFF"));
	
	switch (cConfigData.ShadowLevel) {
		case Lowest:	DrawFormatString(280, Shadow_Y, DxColor::White, "影品質 : 最低（なし）");	break;
		case Low:		DrawFormatString(280, Shadow_Y, DxColor::White, "影品質 : 低");				break;
		case Medium:	DrawFormatString(280, Shadow_Y, DxColor::White, "影品質 : 中");				break;
		case High:		DrawFormatString(280, Shadow_Y, DxColor::White, "影品質 : 高");				break;
		case Highest:	DrawFormatString(280, Shadow_Y, DxColor::White, "影品質 : 最高");			break;
	}

	switch (cConfigData.ReflectionLevel) {
		case Lowest:	DrawFormatString(280, Reflection_Y, DxColor::White, "反射品質 : 最低（なし）"); break;
		case Low:		DrawFormatString(280, Reflection_Y, DxColor::White, "反射品質 : 低");			break;
		case Medium:	DrawFormatString(280, Reflection_Y, DxColor::White, "反射品質 : 中");			break;
		case High:		DrawFormatString(280, Reflection_Y, DxColor::White, "反射品質 : 高");			break;
		case Highest:	DrawFormatString(280, Reflection_Y, DxColor::White, "反射品質 : 最高");			break;
	}

	switch (cConfigData.SSType) {
		case eSSType_BMP: DrawFormatString(280, SSType_Y, DxColor::White, "スクリーンショット形式 : BMP"); break;
		case eSSType_PNG: DrawFormatString(280, SSType_Y, DxColor::White, "スクリーンショット形式 : PNG"); break;
		case eSSType_JPG: DrawFormatString(280, SSType_Y, DxColor::White, "スクリーンショット形式 : JPG"); break;
	}

	DrawString(280, BACK_Y, "戻る", DxColor::White);

	int y = 0;
	switch (NowSelect) {	//現在の選択状態に従って処理を分岐
		case eConfig_MasterVol:		y = MasterVol_Y;	break;
		case eConfig_BGMVol:		y = BGMVol_Y;		break;
		case eConfig_SEVol:			y = SEVol_Y;		break;
		case eConfig_WindowMode:	y = WindowMode_Y;	break;
		case eConfig_Resolution:	y = Resolution_Y;	break;
		case eConfig_DrawMode:		y = DrawMode_Y;		break;
		case eConfig_AntiAlias:		y = Antialias_Y;	break;
		case eConfig_VSync:			y = VSync_Y;		break;
		case eConfig_Shadow:		y = Shadow_Y;		break;
		case eConfig_Reflection:	y = Reflection_Y;	break;
		case eConfig_SSType:		y = SSType_Y;		break;
		case eConfig_Back:			y = BACK_Y;			break;
	}
	DrawString(250, y, "■", DxColor::White);
}