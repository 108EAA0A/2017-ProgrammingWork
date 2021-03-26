#pragma once

#include "_DxLib.h"

/* シャドウマップ構造体 */
struct ShadowMapStr {
	int Handle;
	//int Definition;
	VECTOR MinPos;
	VECTOR MaxPos;
};

typedef void(*drawFuncPtr)(bool);

// 影描画の準備をする
void ShadowRender_Initialize();

// 影描画の後始末をする
void ShadowRender_Terminate();

// ステージ開始時の描画準備を行う
void ShadowRender_StageSetup(const VECTOR LightDirection);

// シャドウマップに影を落とす
void ShadowRender_Setup(const VECTOR CenterPos);

// 影の描画開始
void ShadowRender_DrawStart();

// 影の描画終了
void ShadowRender_DrawEnd();

// 影描画のデバッグ描画処理
void ShadowRender_DrawDebug();

//void PointShadowRender(const VECTOR Pos, const Capsule& Cap);