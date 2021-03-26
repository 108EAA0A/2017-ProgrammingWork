#include "_DxLib.h"
#include "RenderProcess.h"

#define DOWN_SCALE			8	// ガウスフィルタを掛ける画像が画面のサイズの何分の１か
#define BRIGHT_CLIP_BORDER	235	// 高輝度部分を抜き出す境界値
#define GAUSS_RATIO			800	// ガウスフィルタのぼかし強度

static int HighBrightScreen;	// 普通の描画結果から高輝度部分を抜き出した結果を書き込むスクリーン
static int DownScaleScreen;		// 高輝度部分を縮小した結果を書き込むスクリーン
static int GaussScreen;			// 縮小画像をガウスフィルタでぼかした結果を書き込むスクリーン

void Bloom_Init() {
	SetCreateGraphColorBitDepth(16);
	HighBrightScreen = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
	DownScaleScreen	 = MakeScreen(BASE_RENDER_SIZE_W / DOWN_SCALE, BASE_RENDER_SIZE_H / DOWN_SCALE, FALSE);
	GaussScreen		 = MakeScreen(BASE_RENDER_SIZE_W / DOWN_SCALE, BASE_RENDER_SIZE_H / DOWN_SCALE, FALSE);
	SetCreateGraphColorBitDepth(32);
}

void Bloom_Finalize() {
	DeleteGraph(HighBrightScreen);
	DeleteGraph(DownScaleScreen);
	DeleteGraph(GaussScreen);
}

void Bloom_Setup() {
	extern int MainScreenHandle;

	// 描画結果から高輝度部分のみを抜き出した画像を得る
	// BRIGHT_CLIP_BORDERより明度が小さい部分は黒で塗りつぶす
	GraphFilterBlt(MainScreenHandle, HighBrightScreen, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, BRIGHT_CLIP_BORDER, TRUE, DxColor::Black, 0xFF);

	// 高輝度部分を８分の１に縮小した画像を得る
	GraphFilterBlt(HighBrightScreen, DownScaleScreen, DX_GRAPH_FILTER_DOWN_SCALE, DOWN_SCALE);

	// 縮小した画像をガウスフィルタでぼかす
	GraphFilterBlt(DownScaleScreen, GaussScreen, DX_GRAPH_FILTER_GAUSS, 16, GAUSS_RATIO);
}

void Bloom_Draw() {
	const int DrawModebuf = GetDrawMode();
	
	// 描画モードをバイリニアフィルタリングにする( 拡大したときにドットがぼやけるようにする )
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	// 描画ブレンドモードを加算にする
	SetDrawBlendMode(DX_BLENDMODE_ADD, 0x3F);

	// 高輝度部分を縮小してぼかした画像を画面いっぱいに２回描画する( ２回描画するのはより明るくみえるようにするため )
	DrawExtendGraph(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, GaussScreen, FALSE);
	//DrawExtendGraph(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, GaussScreen, FALSE);

	// 描画ブレンドモードをブレンド無しに戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0xFF);

	// 描画モードを戻す
	SetDrawMode(DrawModebuf);
}