#pragma once

#include "_DxLib.h"

#define MIRROR_NUM			1		// 鏡の数


// 鏡の初期化
void Mirror_Initialize();

void Mirror_Finalize();

// 鏡に映る映像を描画するためのカメラの設定を行う
void Mirror_SetupCamera(const int MirrorNo, const VECTOR CameraEyePos, const VECTOR CameraTargetPos);

// 鏡の描画
void Mirror_Render(const int MirrorNo);

// 鏡のデバッグ描画
void Mirror_DebugRender(const int MirrorNo, const int DrawX, const int DrawY);