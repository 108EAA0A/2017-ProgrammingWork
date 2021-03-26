#pragma once

#include "_DxLib.h"

// カメラの手前クリップ距離
#define CAMERA_NEAR_Z		(1.0f)

// カメラの奥クリップ距離
#define CAMERA_FAR_Z		(10000.0f)

// カメラの情報
typedef struct _SCameraInfo {
	// カメラの画角
	float FOV;

	// カメラの座標
	VECTOR Position;

	// フォーカスモードかどうか
	bool FocusFlag;

	bool ControllableFlag;
	
	// カメラの注視点座標
	VECTOR LookAtPosition;

	// カメラ揺らし座標を無視したカメラの座標
	VECTOR OrigPosition;

	// カメラ揺らし座標を無視したカメラの注視点座標
	VECTOR OrigLookAtPosition;

	// カメラの目標座標
	VECTOR TargetPosition;

	// カメラの目標注視点座標
	VECTOR TargetLookAtPosition;

	// カメラの注視点に対する水平角度
	float  HAngle;

	// カメラの注視点に対する垂直角度
	float  VAngle;

	// カメラの座標からカメラの注視点座標までの設定した距離
	float  Distance;

	// カメラの座標からカメラの注視点座標までの現在の距離
	float  NowDistance;

	// カメラの座標からカメラの注視点座標へのベクトル
	VECTOR Direction;

	// カメラから見て右方向のベクトル
	VECTOR RightDirection;

	// カメラの正面方向のＹ成分を抜いたベクトル
	VECTOR FrontDirection;

	// カメラの当たり判定サイズ
	float  CollisionSize;

	// カメラを揺らす処理を行っているかどうか
	bool   Shake;

	// カメラを揺らす幅
	float  ShakeWidth;

	// カメラを揺らす時間
	float  ShakeTime;

	// カメラを揺らす処理の経過時間計測用変数
	float  ShakeTimeCounter;

	// カメラを揺らす処理で使用する sin に渡す角度
	float  ShakeAngle;

	// カメラを揺らす処理で使用する sin に渡す角度の変化速度
	float  ShakeAngleSpeed;

	// カメラを揺らす処理で算出したカメラ座標に足す座標
	VECTOR ShakePosition;
} SCameraInfo;

// カメラの処理を初期化する
extern void Camera_Initialize();

// カメラの状態推移処理を実行する
extern void Camera_Step(
	// 推移させる時間( 単位：秒 )
	const float StepTime
);

// カメラの設定を行う
extern void Camera_Setup();

// カメラを揺らす処理を開始する
extern void Camera_Shake(
	// 揺れる幅
	const float ShakeWidth,

	// 揺れる時間
	const float ShakeTime,

	// 揺れの速度
	const float ShakeAngleSpeed
);

// カメラの向いている方向を取得する
//     戻り値 : カメラの向いている方向のベクトル
extern VECTOR Camera_FrontDirection( void );

// カメラの向いている方向から見て右方向を取得する
//     戻り値 : カメラの向いている方向から見て右方向のベクトルを取得する
extern VECTOR Camera_RightDirection( void );