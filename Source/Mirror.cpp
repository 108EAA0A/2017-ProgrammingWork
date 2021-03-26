// 鏡面効果基本

#include "Mirror.h"

#include "_DxLib.h"
#include "ConfigData.h"
#include "DxMath.h"
#include "Structure.h"
//#include "StageModel.h"

#define MIRROR_SCREEN_W		1920	// 鏡に映る映像の取得に使用するスクリーンの横解像度
#define MIRROR_SCREEN_H		1080	// 鏡に映る映像の取得に使用するスクリーンの縦解像度
//#define MIRROR_POINTNUM		256		// 鏡の描画に使用する面の頂点分割数
#define MIRROR_DEBUG_SCALE	4		// 鏡のデバッグ表示時に元の何分の１にするかのサイズ

int MirrorHandle[MIRROR_NUM];		// 鏡に映る映像の取得に使用するスクリーン
static int MirrorVertexNum;
static FLOAT4 MirrorScreenPosW[MIRROR_NUM][4];	// 鏡に映る映像の取得に使用するクリーンの中の鏡の四隅の座標( 同次座標 )

extern ConfigData cConfigData;

// 鏡のワールド座標
static const VECTOR MirrorWorldPos[MIRROR_NUM][4] = {
	{
		{ -2000.0f, -5.75f,  2000.0f },
		{  2000.0f, -5.75f,  2000.0f },
		{ -2000.0f, -5.75f, -2000.0f },
		{  2000.0f, -5.75f, -2000.0f },
	},
};

// 鏡の Ambient Color
static const COLOR_F MirrorAmbientColor[MIRROR_NUM] = {
	//{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
} ;

// 鏡の Diffuse Color
static const int MirrorDiffuseColor[MIRROR_NUM][4] = {
	{ 0x7F, 0x7F, 0xFF, 0xFF },
} ;

// 鏡のブレンドモードとパラメータ
static const int MirrorBlendParam[MIRROR_NUM][2] = {
	//{ DX_BLENDMODE_NOBLEND, 255 },
	{ DX_BLENDMODE_ALPHA,   128 },
} ;



// 鏡の初期化
void Mirror_Initialize() {
	if (cConfigData.ReflectionLevel == Lowest) return;

	SetCreateDrawValidGraphMultiSample(4, 4);

	// 鏡に映る映像の取得に使用するスクリーンの作成
	for (int i = 0; i < MIRROR_NUM; ++i) {
		MirrorHandle[i] = MakeScreen(MIRROR_SCREEN_W, MIRROR_SCREEN_H, TRUE);
	}

	SetCreateDrawValidGraphMultiSample(0, 0);

	switch (cConfigData.ReflectionLevel) {
		case Lowest: return;
		case Low:		MirrorVertexNum = 64;  break;
		case Medium:	MirrorVertexNum = 100; break;
		case High:		MirrorVertexNum = 200; break;
		case Highest:	MirrorVertexNum = 256; break;
		default: mythrow("反射レベル設定値が異常です");
	}
	//MirrorVertexNum = powi(2, ReflectionLevel + 4);
}

// 鏡の後始末
void Mirror_Finalize() {
	if (cConfigData.ReflectionLevel == Lowest) return;

	for (int i = 0; i < MIRROR_NUM; ++i) {
		DeleteGraph(MirrorHandle[i]);
	}
}

// 鏡に映る映像を描画するためのカメラの設定を行う
void Mirror_SetupCamera(const int MirrorNo, const VECTOR CameraEyePos, const VECTOR CameraTargetPos) {
	if (cConfigData.ReflectionLevel == Lowest) return;
	
	float EyeLength, TargetLength;
	VECTOR MirrorNormal;
	VECTOR MirrorCameraEyePos, MirrorCameraTargetPos;

	/*extern StageModel Stage;
	VECTOR MirrorStagePos[4] = {
		MirrorWorldPos[MirrorNo][0] * Stage.scale,
		MirrorWorldPos[MirrorNo][1] * Stage.scale,
		MirrorWorldPos[MirrorNo][2] * Stage.scale,
		MirrorWorldPos[MirrorNo][3] * Stage.scale
	};*/

	// 鏡の面の法線を算出
	//MirrorNormal = VNorm(VCross(MirrorWorldPos[MirrorNo][1] - MirrorWorldPos[MirrorNo][0], MirrorWorldPos[MirrorNo][2] - MirrorWorldPos[MirrorNo][0]));
	MirrorNormal = VGet(0.0f, 1.0f, 0.0f);

	// 鏡の面からカメラの座標までの最短距離、鏡の面からカメラの注視点までの最短距離を算出
	EyeLength = Plane_Point_MinLength(MirrorWorldPos[MirrorNo][0], MirrorNormal, CameraEyePos);
	TargetLength = Plane_Point_MinLength(MirrorWorldPos[MirrorNo][0], MirrorNormal, CameraTargetPos);

	// 鏡に映る映像を描画する際に使用するカメラの座標とカメラの注視点を算出
	MirrorCameraEyePos = CameraEyePos + VScale(MirrorNormal, -EyeLength * 2.0f);
	MirrorCameraTargetPos = CameraTargetPos + VScale(MirrorNormal, -TargetLength * 2.0f);

	// 計算で得られたカメラの座標とカメラの注視点の座標をカメラの設定する
	SetCameraPositionAndTarget_UpVecY(MirrorCameraEyePos, MirrorCameraTargetPos);

	// 鏡に映る映像の中での鏡の四隅の座標を算出( 同次座標 )
	for (int i = 0; i < 4; ++i) {
		MirrorScreenPosW[MirrorNo][i] = ConvWorldPosToScreenPosPlusW(MirrorWorldPos[MirrorNo][i]);
	}
}

// 鏡の描画
void Mirror_Render(const int MirrorNo) {
	if (cConfigData.ReflectionLevel == Lowest) return;

	VERTEX3D *const Vert = new VERTEX3D[MirrorVertexNum * MirrorVertexNum];
	unsigned short *const Index = new unsigned short[(MirrorVertexNum - 1) * (MirrorVertexNum - 1) * 6];
	MATERIALPARAM Material;
	VECTOR HUnitPos;
	VECTOR VUnitPos[2];
	VECTOR HPos;
	VECTOR VPos[2];
	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	int TextureW, TextureH;
	
	// 鏡の描画に使用するマテリアルのセットアップ
	Material.Ambient  = MirrorAmbientColor[MirrorNo];
	Material.Diffuse  = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Power    = 1.0f;
	SetMaterialParam(Material);

	// 鏡の面の法線を算出
	//const VECTOR MirrorNormal = VNorm(VCross(MirrorWorldPosP[1] - MirrorWorldPosP[0], MirrorWorldPosP[2] - MirrorWorldPosP[0]));
	const VECTOR MirrorNormal = VGet(0.0f, 1.0f, 0.0f);

	// 鏡に映る映像を書き込んだ画像のテクスチャのサイズを取得
	GetGraphTextureSize(MirrorHandle[MirrorNo], &TextureW, &TextureH);

	// 鏡の描画に使用する頂点のセットアップ
	VUnitPos[0] = VScale(MirrorWorldPos[MirrorNo][2] - MirrorWorldPos[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitPos[1] = VScale(MirrorWorldPos[MirrorNo][3] - MirrorWorldPos[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[0] = F4Scale(MirrorScreenPosW[MirrorNo][2] - MirrorScreenPosW[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[1] = F4Scale(MirrorScreenPosW[MirrorNo][3] - MirrorScreenPosW[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	const COLOR_U8 DiffuseColor = GetColorU8(MirrorDiffuseColor[MirrorNo][0], MirrorDiffuseColor[MirrorNo][1], MirrorDiffuseColor[MirrorNo][2], MirrorDiffuseColor[MirrorNo][3]);
	const COLOR_U8 SpecularColor = GetColorU8(0, 0, 0, 0);
	VPos[0] = MirrorWorldPos[MirrorNo][0];
	VPos[1] = MirrorWorldPos[MirrorNo][1];
	VUV[0] = MirrorScreenPosW[MirrorNo][0];
	VUV[1] = MirrorScreenPosW[MirrorNo][1];

	for (int i = 0, k = 0; i < MirrorVertexNum; ++i) {
		HUnitPos = VScale(VPos[1] - VPos[0], 1.0f / (MirrorVertexNum - 1));
		HPos	 = VPos[0];
		HUnitUV	 = F4Scale(VUV[1] - VUV[0], 1.0f / (MirrorVertexNum - 1));
		HUV		 = VUV[0];
		for (int j = 0; j < MirrorVertexNum; ++j, ++k) {
			Vert[k].pos	 = HPos;
			Vert[k].norm = MirrorNormal;
			Vert[k].dif	 = DiffuseColor;
			Vert[k].spc	 = SpecularColor;
			Vert[k].u	 = HUV.x / (HUV.w * TextureW);
			Vert[k].v	 = HUV.y / (HUV.w * TextureH);
			Vert[k].su	 = 0.0f;
			Vert[k].sv	 = 0.0f;
			HUV	 = HUV	+ HUnitUV;
			HPos = HPos + HUnitPos;
		}
		VUV[0]  = VUV[0]  + VUnitUV[0];
		VUV[1]  = VUV[1]  + VUnitUV[1];
		VPos[0] = VPos[0] + VUnitPos[0];
		VPos[1] = VPos[1] + VUnitPos[1];
	}

	// 鏡の描画に使用する頂点インデックスをセットアップ
	for (int i = 0, k = 0; i < MirrorVertexNum - 1; ++i) {
		for (int j = 0; j < MirrorVertexNum - 1; ++j, k += 6) {
			Index[k + 0] = (i + 0) * MirrorVertexNum + j + 0;
			Index[k + 1] = (i + 0) * MirrorVertexNum + j + 1;
			Index[k + 2] = (i + 1) * MirrorVertexNum + j + 0;
			Index[k + 3] = (i + 1) * MirrorVertexNum + j + 1;
			Index[k + 4] = Index[k + 2];
			Index[k + 5] = Index[k + 1];
		}
	}

	const int DMbuf = GetDrawMode();
	// 描画モードをバイリニアフィルタリングに設定
	//SetDrawMode(DX_DRAWMODE_BILINEAR);
	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

	// 描画ブレンドモードを変更
	SetDrawBlendMode(MirrorBlendParam[MirrorNo][0], MirrorBlendParam[MirrorNo][1]);

	// 描画にＺバッファを使用する
	SetUseZBuffer3D(TRUE);

	// Ｚバッファに書き込みを行う
	SetWriteZBuffer3D(TRUE);

	//SetTextureAddressMode(DX_TEXADDRESS_MIRROR);

	// 鏡を描画
	DrawPolygonIndexed3D(Vert, MirrorVertexNum * MirrorVertexNum, Index, (MirrorVertexNum - 1) * (MirrorVertexNum - 1) * 2, MirrorHandle[MirrorNo], TRUE);

	delete[] Vert;
	delete[] Index;

	// Ｚバッファに書き込みを行う設定を元に戻す
	//SetWriteZBuffer3D( FALSE ) ;

	// Ｚバッファを使用する設定を元に戻す
	//SetUseZBuffer3D( FALSE ) ;

	// 描画ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	// 描画モードを元に戻す
	//SetDrawMode(DX_DRAWMODE_NEAREST);
	SetDrawMode(DMbuf);
}

// 鏡のデバッグ描画
void Mirror_DebugRender(const int MirrorNo, const int DrawX, const int DrawY) {
	if (cConfigData.ReflectionLevel == Lowest) return;

	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	int x, y;

	// 指定の座標に鏡の映像の取得に使用したスクリーンを描画
	DrawExtendGraph(DrawX, DrawY, DrawX + MIRROR_SCREEN_W / MIRROR_DEBUG_SCALE, DrawY + MIRROR_SCREEN_H / MIRROR_DEBUG_SCALE, MirrorHandle[MirrorNo], FALSE);

	// 鏡の映像の取得に使用したスクリーンの中の鏡の部分に点を描画
	VUnitUV[0] = F4Scale(MirrorScreenPosW[MirrorNo][2] - MirrorScreenPosW[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[1] = F4Scale(MirrorScreenPosW[MirrorNo][3] - MirrorScreenPosW[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	VUV[0] = MirrorScreenPosW[MirrorNo][0];
	VUV[1] = MirrorScreenPosW[MirrorNo][1];
	for (int i = 0; i < MirrorVertexNum; ++i) {
		HUnitUV = F4Scale(VUV[1] - VUV[0], 1.0f / (MirrorVertexNum - 1));
		HUV = VUV[0];
		for (int j = 0; j < MirrorVertexNum; ++j) {
			x = static_cast<int>(HUV.x / HUV.w / MIRROR_DEBUG_SCALE);
			y = static_cast<int>(HUV.y / HUV.w / MIRROR_DEBUG_SCALE);

			if( x > 0 && x < MIRROR_SCREEN_W / MIRROR_DEBUG_SCALE &&
			    y > 0 && y < MIRROR_SCREEN_H / MIRROR_DEBUG_SCALE ) {
				DrawPixel(DrawX + x, DrawY + y, DxColor::Green);
			}

			HUV = HUV + HUnitUV;
		}
		VUV[0] = VUV[0] + VUnitUV[0];
		VUV[1] = VUV[1] + VUnitUV[1];
	}
}