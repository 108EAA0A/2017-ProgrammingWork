#include "ShadowControl.h"

#include "_DxLib.h"
#include "DxMath.h"
#include "ConfigData.h"
#include "Structure.h"
#include "StageModel.h"

extern ConfigData cConfigData;

ShadowMapStr StageShadowMap;	// 範囲：ステージ全域、最初に１回描画して使いまわし
ShadowMapStr FarShadowMap;		// 範囲：ステージ全域、遠い敵などの影用
ShadowMapStr NearShadowMap;		// 範囲：プレイヤー周辺、綺麗な影用

drawFuncPtr StaticRenderProcess;
drawFuncPtr MovableRenderProcess;

//int PointShadow;

static void StaticRenderProcessDraw(const bool Flag) {
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	StaticRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	StaticRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}
static void MovableRenderProcessDraw(const bool Flag) {
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	MovableRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	MovableRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}

void ShadowRender_Initialize() {
	// 低処理負荷モードでは無い場合はシャドウマップやアンチエイリアス画面を作成する
	if (cConfigData.ShadowLevel != Lowest) {
		const int SHADOWMAP_RESOLUTION = powi(2, cConfigData.ShadowLevel + 9);

		// シャドウマップハンドルを作成する
		StageShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (StageShadowMap.Handle == -1) mythrow("StageShadowMapの生成に失敗しました");
		
		FarShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (FarShadowMap.Handle == -1) mythrow("FarShadowMapの生成に失敗しました");

		NearShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (NearShadowMap.Handle == -1) mythrow("NearShadowMapの生成に失敗しました");
	}
	/*else {
		PointShadow = LoadGraph("resource/Shadow.png");
	}*/
}

void ShadowRender_Terminate() {
	if (cConfigData.ShadowLevel != Lowest) {
		// シャドウマップハンドルを削除する
		DeleteShadowMap(StageShadowMap.Handle);
		StageShadowMap.Handle = -1;

		DeleteShadowMap(FarShadowMap.Handle);
		FarShadowMap.Handle = -1;

		DeleteShadowMap(NearShadowMap.Handle);
		NearShadowMap.Handle = -1;
	}
}

void ShadowRender_StageSetup(const VECTOR LightDirection) {
	// 低処理負荷モードではない場合は遠距離用シャドウマップにステージを描画する
	if (cConfigData.ShadowLevel != Lowest) {
		if (StageShadowMap.Handle == -1) mythrow("StageShadowMapが生成されていません");
		else if (FarShadowMap.Handle == -1) mythrow("FarShadowMapが生成されていません");
		else if (NearShadowMap.Handle == -1) mythrow("NearShadowMapが生成されていません");

		// シャドウマップへのレンダリングで使用するライトの方向をセット
		SetShadowMapLightDirection(StageShadowMap.Handle, LightDirection);
		SetShadowMapLightDirection(FarShadowMap.Handle, LightDirection);
		SetShadowMapLightDirection(NearShadowMap.Handle, LightDirection);

		// ステージモデル用と遠距離用のシャドウマップにはステージ全体を収める
		{
			MV1_REF_POLYGONLIST PolyList;
			extern StageModel Stage;
			
			PolyList = MV1GetReferenceMesh(Stage.handle, -1, FALSE);
			SetShadowMapDrawArea(StageShadowMap.Handle, PolyList.MinPosition * Stage.scale.x, PolyList.MaxPosition * Stage.scale.x);
			SetShadowMapDrawArea(FarShadowMap.Handle, PolyList.MinPosition * Stage.scale.x, PolyList.MaxPosition * Stage.scale.x);
		}

		// ステージモデル用のシャドウマップにステージをレンダリング
		ShadowMap_DrawSetup(StageShadowMap.Handle);
		StaticRenderProcessDraw(false);
		ShadowMap_DrawEnd();
	}
}

void ShadowRender_Setup(const VECTOR CenterPos) {
	// 低処理負荷モードではない場合はシャドウマップへの描画を行う
	if (cConfigData.ShadowLevel != Lowest) {
		// 遠距離用のシャドウマップにキャラクターをレンダリング
		ShadowMap_DrawSetup(FarShadowMap.Handle);
		MovableRenderProcessDraw(false);
		ShadowMap_DrawEnd();

		// 近距離用のシャドウマップの描画範囲を算出
		static const float NearDistance = 150.0f;
		NearShadowMap.MinPos = CenterPos - VGet(NearDistance, 0.2f, NearDistance);
		NearShadowMap.MaxPos = CenterPos + VGet(NearDistance, 5.0f, NearDistance);
		// 近距離用のシャドウマップの描画範囲をセット
		SetShadowMapDrawArea(NearShadowMap.Handle, NearShadowMap.MinPos, NearShadowMap.MaxPos);

		// 近距離用のシャドウマップにキャラクターをレンダリング
		ShadowMap_DrawSetup(NearShadowMap.Handle);
		MovableRenderProcessDraw(true);
		ShadowMap_DrawEnd();
	}
}

void ShadowRender_DrawStart() {
	// 低処理負荷モードではない場合はシャドウマップを使用する
	if (cConfigData.ShadowLevel != Lowest) {
		SetUseShadowMap(0, StageShadowMap.Handle);
		SetUseShadowMap(1, FarShadowMap.Handle);
		SetUseShadowMap(2, NearShadowMap.Handle);
	}
	// 低処理負荷モードの場合は丸影を描画する
	/*if (ShadowLevel == Lowest) {
	PointShadowRender(ActionPlayer.pos, ActionPlayer.collCap);
	for (int i = 0; i < GoblinNum; ++i) {
	if (!(Goblin[i].deadflag && Goblin[i].state == CharaState::DownLoop)) PointShadowRender(Goblin[i].pos, Goblin[i].collCap);
	}
	}*/
}

void ShadowRender_DrawEnd() {
	// 低処理負荷モードではない場合はシャドウマップの使用を解除
	if (cConfigData.ShadowLevel != Lowest) {
		SetUseShadowMap(0, -1);
		SetUseShadowMap(1, -1);
		SetUseShadowMap(2, -1);
	}
}

void ShadowRender_DrawDebug() {
	if (cConfigData.ShadowLevel != Lowest) {
		TestDrawShadowMap(NearShadowMap.Handle, 1440, 810, 1920, 1080);
	}
}

/*void PointShadowRender(const VECTOR Pos, const Capsule& Cap) {
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	extern StageModel Stage;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	//SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitResDim = Stage.CollCheck_Capsule(Cap);
	
	// 頂点データで変化が無い部分をセット
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// 球の直下に存在するポリゴンの数だけ繰り返し
	HitRes = HitResDim.Dim;
	for (int i = 0; i < HitResDim.HitNum; ++i, ++HitRes) {
		// ポリゴンの座標は地面ポリゴンの座標
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ちょっと持ち上げて重ならないようにする
		SlideVec = HitRes->Normal * 0.005f;
		Vertex[0].pos = Vertex[0].pos + SlideVec;
		Vertex[1].pos = Vertex[1].pos + SlideVec;
		Vertex[2].pos = Vertex[2].pos + SlideVec;

		// ポリゴンの不透明度を設定する
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[0].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[0].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		if (HitRes->Position[1].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[1].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[1].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		if (HitRes->Position[2].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[2].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[2].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
		Vertex[0].u = (HitRes->Position[0].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;

		// 影ポリゴンを描画
		DrawPolygon3D(Vertex, 1, PointShadow, TRUE);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(HitResDim);

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	//SetUseZBuffer3D(FALSE);

	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
}*/