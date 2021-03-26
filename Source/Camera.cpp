#include "Camera.h"
#include "Chara_Player.h"
#include "DxMath.h"
#include "StageModel.h"

// カメラの初期水平角度
#define DEFAULT_H_ANGLE				(DX_PI_F)

// カメラの初期垂直角度
#define DEFAULT_V_ANGLE				(0.4f)

#undef DEFAULT_FOV
// カメラの画角
#define DEFAULT_FOV					(40.0f)

#define CAMERA_NEAR_LIM				10.0f
#define CAMERA_FAR_LIM				300.0f

// カメラの座標を目標座標に近づける計算で除算に使用する値
#define MOVE_DIV_NUM				(0.16666667f)

SCameraInfo g_CamInfo;

bool FPSFlag = false;

// カメラの処理を初期化する
void Camera_Initialize() {
	extern Chara_Player ActionPlayer;

	// 各パラメータに初期値をセットする
	g_CamInfo.FOV			 = DEFAULT_FOV;
	g_CamInfo.Position		 = VGet(-9.5f * ActionPlayer.scale.y*11.5f, 4.0f * ActionPlayer.scale.y*11.5f, 0.0f * ActionPlayer.scale.y*11.5f);
	g_CamInfo.FocusFlag		 = false;
	g_CamInfo.ControllableFlag = true;
	g_CamInfo.LookAtPosition = VGet(0.0f, 15.0f*ActionPlayer.scale.y, 0.0f);
	g_CamInfo.HAngle         = DEFAULT_H_ANGLE;
	g_CamInfo.VAngle         = DEFAULT_V_ANGLE;
	g_CamInfo.Distance	 	 = 75.0f;
	g_CamInfo.CollisionSize  = 0.5f;
	g_CamInfo.Shake          = false;
	g_CamInfo.ShakePosition  = VGet( 0.0f, 0.0f, 0.0f );
}

// カメラの状態推移処理を実行する
void Camera_Step(
	// 推移させる時間( 単位：秒 )
	const float StepTime
)
{
	float       MoveScale;
	VECTOR      TempV;
	float       NotHitDistance;
	float       HitDistance;
	float       TestDistance;
	VECTOR      TestPosition;
	VECTOR      Direction;

	extern StageModel Stage;
	extern Chara_Player ActionPlayer;

	/*// プレイヤーが注視点になるので、プレイヤーが存在しない場合は何もせず終了
	if( PCInfo == NULL ) {
		return;
	}*/

	//if (!g_CamInfo.FocusFlag) {

	extern unsigned int KeyFrame[256];
	if (KeyFrame[KEY_INPUT_F5] == 1) FPSFlag = !FPSFlag;

	if (g_CamInfo.ControllableFlag) {
		static float MouseSpeed_X = 0, MouseSpeed_Y = 0;

		if (!FPSFlag) {
			g_CamInfo.Distance -= GetMouseWheelRotVolF()*5.0f;

			if (g_CamInfo.Distance < CAMERA_NEAR_LIM) g_CamInfo.Distance = CAMERA_NEAR_LIM;
			if (g_CamInfo.Distance > CAMERA_FAR_LIM)  g_CamInfo.Distance = CAMERA_FAR_LIM;
		}

		{
			extern int WINDOWS_H, WINDOWS_V;
			static int MouseX	  = WINDOWS_H / 2, MouseY	  = WINDOWS_V / 2;
			static int PrevMouseX = WINDOWS_H / 2, PrevMouseY = WINDOWS_V / 2;

			if (PrevMouseX != WINDOWS_H / 2) PrevMouseX = MouseX; //1ループ前のマウスのX座標保存
			if (PrevMouseY != WINDOWS_V / 2) PrevMouseY = MouseY; //1ループ前のマウスのY座標保存
			GetMousePoint(&MouseX, &MouseY); //マウスの座標取得
			MouseSpeed_X = static_cast<float>(MouseX - PrevMouseX) / 1000.0f; //マウスのX移動距離を速度に変換 
			MouseSpeed_Y = static_cast<float>(MouseY - PrevMouseY) / 1000.0f; //マウスのY移動距離を速度に変換
			SetMousePoint(WINDOWS_H / 2, WINDOWS_V / 2); //マウスを画面中央に移動
		}

		// 水平方向の角度変更入力の処理
		if (!FPSFlag) g_CamInfo.HAngle -= MouseSpeed_X;
		else g_CamInfo.HAngle += MouseSpeed_X;

		if (g_CamInfo.HAngle < -DX_PI_F) {
			g_CamInfo.HAngle += DX_TWO_PI_F;
		}
		if (g_CamInfo.HAngle > DX_PI_F) {
			g_CamInfo.HAngle -= DX_TWO_PI_F;
		}

		// 垂直方向の角度変更入力の処理
		g_CamInfo.VAngle += MouseSpeed_Y;
		if (g_CamInfo.VAngle < -DX_PI_F / 2.0f + 0.6f) {
			g_CamInfo.VAngle = -DX_PI_F / 2.0f + 0.6f;
		}
		if (g_CamInfo.VAngle > DX_PI_F / 2.0f - 0.6f) {
			g_CamInfo.VAngle = DX_PI_F / 2.0f - 0.6f;
		}
	}
	/*else {
		g_CamInfo.HAngle = atan2f(ActionPlayer.pos.x - g_CamInfo.TargetLookAtPosition.x, ActionPlayer.pos.z - g_CamInfo.TargetLookAtPosition.z);
		g_CamInfo.VAngle = DEFAULT_V_ANGLE;
	}*/

	// カメラを揺らす処理を行うかどうかのフラグが立っていたらカメラを揺らす処理を行う
	if (g_CamInfo.Shake) {
		// sin を使用した揺らし座標の算出
		g_CamInfo.ShakePosition.y =
			sin( g_CamInfo.ShakeAngle ) * ( 1.0f - ( g_CamInfo.ShakeTimeCounter /
										g_CamInfo.ShakeTime ) ) * g_CamInfo.ShakeWidth;
		g_CamInfo.ShakePosition.x = 0.0f;
		g_CamInfo.ShakePosition.z = 0.0f;

		// 揺らし処理に使用する sin に渡す角度の変更処理
		g_CamInfo.ShakeAngle += g_CamInfo.ShakeAngleSpeed * StepTime;
		
		// 揺らす時間が経過したら揺らし処理を終了する
		g_CamInfo.ShakeTimeCounter += StepTime;
		if (g_CamInfo.ShakeTimeCounter >= g_CamInfo.ShakeTime) {
			g_CamInfo.Shake = false;
		}
	}
	else {
		// 揺らされていない場合は揺らし処理による加算座標を０にする
		g_CamInfo.ShakePosition = VGet(0.0f, 0.0f, 0.0f);
	}

	// 目標注視点座標の算出
	//if (!g_CamInfo.FocusFlag) {
	if (FPSFlag) {
		g_CamInfo.TargetPosition = ActionPlayer.pos;

		// カメラの揺らし処理を無視したカメラの座標、注視点座標をそれぞれの目標座標に近づける
		MoveScale = (1.0f / 60.0f) / MOVE_DIV_NUM;

		TempV = g_CamInfo.TargetPosition - g_CamInfo.OrigPosition;
		TempV = VScale(TempV, MoveScale);
		g_CamInfo.OrigPosition = g_CamInfo.OrigPosition + TempV;

		g_CamInfo.OrigPosition = ActionPlayer.pos.cYadd(ActionPlayer.collCap.height*2.0f);
		g_CamInfo.Position = g_CamInfo.OrigPosition + g_CamInfo.ShakePosition;
		
		return;
	}

	g_CamInfo.TargetLookAtPosition = ActionPlayer.pos.cYadd(15.0f*ActionPlayer.scale.y);

	// 目標座標の算出
	g_CamInfo.TargetPosition.x = 0.0f;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.VAngle) * g_CamInfo.Distance;
	g_CamInfo.TargetPosition.y = sin(g_CamInfo.VAngle) * g_CamInfo.Distance;

	g_CamInfo.TargetPosition.x = -sin(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;

	g_CamInfo.TargetPosition = VAdd(g_CamInfo.TargetPosition, g_CamInfo.TargetLookAtPosition);

	// カメラの揺らし処理を無視したカメラの座標、注視点座標をそれぞれの目標座標に近づける
	MoveScale = StepTime / MOVE_DIV_NUM;

	TempV = g_CamInfo.TargetPosition - g_CamInfo.OrigPosition;
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigPosition = g_CamInfo.OrigPosition + TempV;

	TempV = g_CamInfo.TargetLookAtPosition - g_CamInfo.OrigLookAtPosition;
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigLookAtPosition = g_CamInfo.OrigLookAtPosition + TempV;

	// カメラの揺らし処理を無視したカメラの座標、注視点座標にカメラの揺らし処理による
	// 加算座標を足した座標を現在のカメラの座標、注視点座標とする
	g_CamInfo.Position = g_CamInfo.OrigPosition + g_CamInfo.ShakePosition;

	g_CamInfo.LookAtPosition = g_CamInfo.OrigLookAtPosition + g_CamInfo.ShakePosition;

	// カメラから見て右方向のベクトルを算出
	g_CamInfo.RightDirection = VNorm(VCross(g_CamInfo.Direction, VGet(0.0f, -1.0f, 0.0f)));

	// カメラの正面方向のＹ成分を抜いたベクトルを算出
	g_CamInfo.FrontDirection = VNorm(VCross(VGet(0.0f, -1.0f, 0.0f), g_CamInfo.RightDirection));

	if (g_CamInfo.ControllableFlag) {
		// カメラ座標から注視点座標の間にコリジョン用ポリゴンが存在するかチェック
		if (Stage.HitCheck_Capsule(Capsule::CapGet(g_CamInfo.Position, g_CamInfo.LookAtPosition, g_CamInfo.CollisionSize))) {

			// コリジョン用ポリゴンが存在しない座標までカメラ座標を注視点に近づける

			// 注視点座標からカメラ座標へのベクトルを算出
			Direction = VNorm(g_CamInfo.Position - g_CamInfo.LookAtPosition);

			// ポリゴンに当たらない距離をセット
			NotHitDistance = 0.0f;

			// ポリゴンに当たる距離をセット
			HitDistance = g_CamInfo.Distance;
			do {
				// 当たるかどうかテストする距離を算出( 当たらない距離と当たる距離の中間 )
				TestDistance = NotHitDistance + (HitDistance - NotHitDistance) / 2.0f;

				// テスト用のカメラ座標を算出
				TestPosition = g_CamInfo.LookAtPosition + VScale(Direction, TestDistance);

				// 新しい座標でコリジョン用ポリゴンに当たるかテスト
				if (Stage.HitCheck_Capsule(Capsule::CapGet(TestPosition, g_CamInfo.LookAtPosition, g_CamInfo.CollisionSize))) {
					// 当たったら当たる距離を TestDistance に変更する
					HitDistance = TestDistance;
				}
				else {
					// 当たらなかったら当たらない距離を TestDistance に変更する
					NotHitDistance = TestDistance;
				}

				// HitDistance と NoHitDistance が十分に近づいていなかったらループ
			} while (HitDistance - NotHitDistance > 1.0f);
			g_CamInfo.NowDistance = HitDistance;

			// カメラの座標をセット
			g_CamInfo.Position = TestPosition;
			g_CamInfo.OrigPosition = TestPosition;
		}
		else {
			g_CamInfo.NowDistance = g_CamInfo.Distance;
		}
	}

	// リスナーの位置を変更
	//Set3DSoundListenerPosAndFrontPos_UpVecY( g_CamInfo.Position, g_CamInfo.TargetLookAtPosition );

	Camera_Setup();
}

// カメラの設定を行う
void Camera_Setup() {
	// カメラの写す範囲を設定
	SetupCamera_Perspective(radf(g_CamInfo.FOV));
	SetCameraNearFar(CAMERA_NEAR_Z, CAMERA_FAR_Z);
	if (!FPSFlag) SetCameraPositionAndTarget_UpVecY(g_CamInfo.Position, g_CamInfo.LookAtPosition);
	else SetCameraPositionAndAngle(g_CamInfo.Position, g_CamInfo.VAngle, g_CamInfo.HAngle + radf(180.0f), 0.0f);
}

// カメラを揺らす処理を開始する
void Camera_Shake(
	// 揺れる幅
	const float ShakeWidth,

	// 揺れる時間
	const float ShakeTime,

	// 揺れの速度
	const float ShakeAngleSpeed
)
{
	g_CamInfo.Shake            = true;
	g_CamInfo.ShakeWidth       = ShakeWidth;
	g_CamInfo.ShakeTimeCounter = 0.0f;
	g_CamInfo.ShakeTime        = ShakeTime;
	g_CamInfo.ShakeAngle       = 0.0f;
	g_CamInfo.ShakeAngleSpeed  = ShakeAngleSpeed;
}

// カメラの向いている方向を取得する
//     戻り値 : カメラの向いている方向のベクトル
VECTOR Camera_FrontDirection() {
	return g_CamInfo.FrontDirection;
}

// カメラの向いている方向から見て右方向を取得する
//     戻り値 : カメラの向いている方向から見て右方向のベクトルを取得する
VECTOR Camera_RightDirection() {
	return g_CamInfo.RightDirection;
}

