#pragma once

/* 度数法をラジアンに変換 */
inline float radf(const float Angle) {
	return (Angle * DX_PI_F / 180.0f);
}

/* ラジアンに度数法を変換 */
inline float anglef(const float Rad) {
	return (Rad * 180.0f / DX_PI_F);
}

/* 整数用累乗関数 */
inline int powi(const int X, const int Y) {
	return static_cast<int>(powf(static_cast<float>(X), static_cast<float>(Y)));
}

//int FColm2Colm(const float mem);

//unsigned int FCol2Col(const COLOR_F ColF);

bool ParamChangeInt(
	// 操作対象の変数のアドレス
	int *Param,

	// 目標とする値
	int TargetParam,

	// 推移させる時間
	float StepTime,

	// 値の変化速度
	float Speed
);

// 浮動小数点型の変数の値を指定の値に近づける処理を行う
//     戻り値 : 変数の値が指定の値に達したかどうか
//              ( true : 指定の値に達した  false : 指定の値に達していない )
bool ParamChangeFloat(
	// 操作対象の変数のアドレス
	float *Param,

	// 目標とする値
	float TargetParam,

	// 推移させる時間
	float StepTime,

	// 値の変化速度
	float Speed
);

// 浮動小数点型の変数に代入されている角度を指定の角度に近づける処理を行う
// ( 目標とする角度と変数に代入されている角度の差が 2.0f * π 以内である必要がある )
//     戻り値 : 変数の角度が指定の角度に達したかどうか
//              ( true : 指定の角度に達した  false : 指定の角度に達していない )
bool ParamChangeAngle(
	// 操作対象の変数のアドレス
	float *Param,

	// 目標とする角度
	float TargetParam,

	// 推移させる時間
	float StepTime,

	// 角度の変化速度
	float Speed
);

VECTOR WorldMatrix2Pos(MATRIX);