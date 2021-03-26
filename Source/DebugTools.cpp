#include "_DxLib.h"
#include "Camera.h"
#include "Chara_Player.h"

#include "FPSCounter.h"

void DebugMode() {
	extern unsigned int KeyFrame[256]; // グローバル変数使用宣言
	
	extern Chara_Player ActionPlayer;
	extern SCameraInfo g_CamInfo;
	extern bool FPSFlag;

	//FpsTimeFanction();
	extern CFPSCounter cFPScounter;
	printfDx("FPS %.2f\n", cFPScounter.GetFPS());

	const Vector LineCenter = g_CamInfo.OrigLookAtPosition;
	SetMaterialParam(DefaultMaterial);
	DrawLine3D(LineCenter, LineCenter.cXadd(10.0f), DxColor::Red);
	DrawLine3D(LineCenter, LineCenter.cYadd(10.0f), DxColor::Green);
	DrawLine3D(LineCenter, LineCenter.cZadd(10.0f), DxColor::Blue);
	DrawLine3D(LineCenter, LineCenter + ActionPlayer.speed * 15.0f, DxColor::White);

	printfDx("カメラモード :%s\n", (FPSFlag ? "FPS" : "TPS"));
	printfDx("カメラ座標 x:%.4f y:%.4f z:%.4f\n", g_CamInfo.Position.x, g_CamInfo.Position.y, g_CamInfo.Position.z);
	printfDx("カメラ注視点座標 x:%.4f y:%.4f z:%.4f\n", g_CamInfo.LookAtPosition.x, g_CamInfo.LookAtPosition.y, g_CamInfo.LookAtPosition.z);
	printfDx("カメラ回転 x:%.4f y:%.4f\n", g_CamInfo.HAngle, g_CamInfo.VAngle);
	printfDx("カメラ設定距離 :%.0f\n", g_CamInfo.Distance);
	printfDx("カメラ実距離 :%.4f\n", g_CamInfo.NowDistance);

	printfDx("プレイヤー座標 x:%.4f y:%.4f z:%.4f\n", ActionPlayer.pos.x, ActionPlayer.pos.y, ActionPlayer.pos.z);
	printfDx("プレイヤー速度 x:%.4f y:%.4f z:%.4f\n", ActionPlayer.speed.x, ActionPlayer.speed.y, ActionPlayer.speed.z);
	printfDx("プレイヤー状態 :%s\n", ActionPlayer.Statestr.at(ActionPlayer.state));
	printfDx("再生アニメーション番号 :%d\n", ActionPlayer.NowAnim.playnum);
	printfDx("アニメーション再生フレーム数 :%.1f\n", ActionPlayer.NowAnim.time);
	printfDx("モーションカットフレーム :%d\n", ActionPlayer.NowAnim.totaltime);
	if (ActionPlayer.AnimBlendRate != 1.0f) {
		printfDx("直前再生アニメーション番号 :%d\n", ActionPlayer.PrevAnim.playnum);
		printfDx("直前アニメーション再生フレーム数 :%.1f\n", ActionPlayer.PrevAnim.time);
		printfDx("直前モーションカットフレーム :%d\n", ActionPlayer.PrevAnim.totaltime);
		printfDx("アニメーションブレンド率 :%.1f\n", ActionPlayer.AnimBlendRate);
	}
}