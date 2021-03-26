#include <math.h>
#include "_DxLib.h"
#include "Screenshot.h"
#include "UpdateKey.h"

void NowLoading() {
	extern int WINDOWS_H, WINDOWS_V;
	extern unsigned int KeyFrame[256];
	
	const float WIN_H_2 = (float)WINDOWS_H / 2.0f;
	const float WIN_V_2 = (float)WINDOWS_V / 2.0f;
	const unsigned int FontColor = GetColor(0x90, 0xD0, 0xD0);
	const unsigned int EdgeColor = GetColor(0x60, 0xA0, 0xA0);

	const int FirstLoadNum = GetASyncLoadNum();
	int LoadingNum;

	//int Movie2 = LoadGraph("resource\\nc65608_cut.ogv");
	//while (CheckHandleASyncLoad(Movie));
	//SeekMovieToGraph(Movie2, 1000);
	//PlayMovieToGraph(Movie2);

	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	float wavetime = 0.0f;
	float sinwave[13];
	for (int i = 0; i < 13; ++i) sinwave[i] = 37.5f * sinf((wavetime + i * 25) * DX_PI_F / 180.0f);

	const int StrWidth = GetDrawStringWidth("NowLoading・・・", (int)strlen("NowLoading..."));
	const int CharWidth = GetDrawStringWidth("A", 1);

	const float LoadingBar_x1 = (static_cast<float>(WINDOWS_H) * 1.0f / 5.0f);
	const float LoadingBar_x2 = (static_cast<float>(WINDOWS_H) * 4.0f / 5.0f);
	const float LoadingBar_y1 = (static_cast<float>(WINDOWS_V) * 1.0f / 3.0f);
	const float LoadingBar_y2 = (static_cast<float>(WINDOWS_V) * 2.0f / 3.0f);

	float LoadingBar_x2_2;

	LONGLONG RefreshTime = 0;
	while (GetASyncLoadNum() != 0 && ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey()) {
		RefreshTime = GetNowHiPerformanceCount(); //今の時間を取得

		if (KeyFrame[KEY_INPUT_ESCAPE] > 0) FORCEEND;
		LoadingNum = GetASyncLoadNum();
		//DrawExtendGraph(0, 0, WINDOWS_H, WINDOWS_V, Movie2, FALSE);
		//WaitTimer(17);
		LoadingBar_x2_2 = LoadingBar_x2 - (static_cast<float>(LoadingBar_x2 - LoadingBar_x1) * (static_cast<float>(LoadingNum) / static_cast<float>(FirstLoadNum)));
		DrawBoxAA(LoadingBar_x1, LoadingBar_y1, LoadingBar_x2  , LoadingBar_y2, DxColor::White, FALSE);
		DrawBoxAA(LoadingBar_x1, LoadingBar_y1, LoadingBar_x2_2, LoadingBar_y2, DxColor::White, TRUE);
		DrawStringF(WIN_H_2 - (StrWidth / 2),				   WIN_V_2 + sinwave[0],  "N", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth,	   WIN_V_2 + sinwave[1],  "o", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 2,  WIN_V_2 + sinwave[2],  "w", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 4,  WIN_V_2 + sinwave[3],  "L", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 5,  WIN_V_2 + sinwave[4],  "o", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 6,  WIN_V_2 + sinwave[5],  "a", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 7,  WIN_V_2 + sinwave[6],  "d", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 8,  WIN_V_2 + sinwave[7],  "i", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 9,  WIN_V_2 + sinwave[8],  "n", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 10, WIN_V_2 + sinwave[9],  "g", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 11, WIN_V_2 + sinwave[10], ".", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 12, WIN_V_2 + sinwave[11], ".", FontColor, EdgeColor);
		DrawStringF(WIN_H_2 - (StrWidth / 2) + CharWidth * 13, WIN_V_2 + sinwave[12], ".", FontColor, EdgeColor);
		wavetime += 3.0f;
		for (int i = 0; i < 13; ++i) sinwave[i] = 37.5f * sinf((wavetime + i * 25) * DX_PI_F / 180.0f);

		if (KeyFrame[KEY_INPUT_F12] == 1) Screenshot();

		while (GetNowHiPerformanceCount() - RefreshTime < 16667); //1周の処理が16.667ミリ秒になるまで待つ
	}

	ChangeFontType(DX_FONTTYPE_NORMAL);
	ClearDrawScreen();
	SetUseASyncLoadFlag(FALSE); // 非同期読み込みをやめる
}