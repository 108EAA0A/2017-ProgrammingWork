#include "Title.h"

#include "_DxLib.h"
#include "NowLoading.h"
#include "RenderProcess.h"
#include "SoundAsset.h"
#include "UpdateKey.h"

enum eTitle_Select {
	eTitle_Action,	// ゲーム選択
	eTitle_Config,		// 設定
	eTitle_Exit,		// 終了

	eTitle_Num,			// 選択項目の数

	eTitle_Title,		// タイトル画面
};

extern unsigned int KeyFrame[256];

static int NowScene = eTitle_Title;
static int NowSelect = eTitle_Action;    //現在の選択状態(初期はゲーム選択中)

static int TitleBG;
static SoundAsset *Enter;

Title::Title(ISceneChanger *const changer) : BaseScene(changer) {
}

//初期化
void Title::Initialize() {
	NowScene = eTitle_Title;
	NowSelect = eTitle_Action;

	SetUseASyncLoadFlag(TRUE);

	TitleBG = LoadGraph("resource/仮画像.png");
	//TitleBGM = new SoundAsset("resource/Sound/BGM/Title.ogg", SoundType::BGM);
	Enter = new SoundAsset("resource/Sound/SE/se_maoudamashii_system37.ogg", SoundType::SE);

	NowLoading();

	//TitleBGM->play(true);

	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

}

//終了処理
void Title::Finalize() {
	DeleteGraph(TitleBG);
	//delete TitleBGM;
	delete Enter;

	ClearDrawScreen();
	ScreenFlip();
	SetFontSize(20);
}

//更新
void Title::Update() {
	if (KeyFrame[KEY_INPUT_DOWN] == 1) {//下キーが押されていたら
		NowSelect = (NowSelect + 1) % eTitle_Num;//選択状態を一つ下げる
		Enter->playMulti();
	}
	if (KeyFrame[KEY_INPUT_UP] == 1) {//上キーが押されていたら
		NowSelect = (NowSelect + (eTitle_Num - 1)) % eTitle_Num;//選択状態を一つ上げる
		Enter->playMulti();
	}
	if (KeyFrame[KEY_INPUT_RETURN] == 1) {//エンターキーが押されたら
		switch (NowSelect) {
		case eTitle_Action:
			mSceneChanger->ChangeScene(eScene_Action);
			break;
		case eTitle_Config:
			mSceneChanger->ChangeScene(eScene_Config);
			break;
		case eTitle_Exit:
			FORCEEND;
		default:
			mythrow("選択例外です");
		}
	}
}

static int DrawCenterX(const char *const str) {
	return (BASE_RENDER_SIZE_W - GetDrawStringWidth(str, strlen(str))) / 2;
}

//描画
void Title::Draw() {
	int y = 0;
	
	DrawGraph(0, 0, TitleBG, FALSE);

	switch (NowScene) {
	case eTitle_Title:
	{
		const int START_Y = 530;   //「はじめから」文字のy位置
		const int CONFIG_Y = 600;  //「設定」文字のy位置
		const int EXIT_Y = 760;    //「終了」文字のy位置

		const int RoundRectCenterY = 150;
		const int RoundRectW = 600, RoundRectH = 400;

		//DrawFillBox(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, DxColor::Black);
		SetDrawBlendMode(DX_BLENDMODE_MUL, 200);
		//DrawRoundRect((BASE_RENDER_SIZE_W - RoundRectW) / 2, (BASE_RENDER_SIZE_H - RoundRectH) / 2, (BASE_RENDER_SIZE_W - RoundRectW) / 2, BASE_RENDER_SIZE_H - (BASE_RENDER_SIZE_H - RoundRectH) / 2, 50, 50, DxColor::Blue, FALSE);
		DrawRoundRect((BASE_RENDER_SIZE_W - RoundRectW) / 2, RoundRectCenterY + (BASE_RENDER_SIZE_H - RoundRectH) / 2, (BASE_RENDER_SIZE_W - RoundRectW) / 2 + RoundRectW, RoundRectCenterY + (BASE_RENDER_SIZE_H - RoundRectH) / 2 + RoundRectH, 50, 50, DxColor::Blue, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		
		SetFontSize(120);
		DrawString(DrawCenterX("ゲームタイトル"), 200, "ゲームタイトル", DxColor::White);
		SetFontSize(60);
		DrawString(DrawCenterX("はじめから"), START_Y, "はじめから", DxColor::White);
		DrawString(DrawCenterX("設定"), CONFIG_Y, "設定", DxColor::White);
		DrawString(DrawCenterX("終了"), EXIT_Y, "終了", DxColor::White);

		switch (NowSelect) {
		case eTitle_Action:
			y = START_Y;
			break;
		case eTitle_Config:
			y = CONFIG_Y;
			break;
		case eTitle_Exit:
			y = EXIT_Y;
			break;
		}
		DrawString(700, y, "■", DxColor::White);
	}
		break;

	case eTitle_Config:
		break;
	}
}