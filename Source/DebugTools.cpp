#include "_DxLib.h"
#include "Camera.h"
#include "Chara_Player.h"

#include "FPSCounter.h"

void DebugMode() {
	extern unsigned int KeyFrame[256]; // �O���[�o���ϐ��g�p�錾
	
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

	printfDx("�J�������[�h :%s\n", (FPSFlag ? "FPS" : "TPS"));
	printfDx("�J�������W x:%.4f y:%.4f z:%.4f\n", g_CamInfo.Position.x, g_CamInfo.Position.y, g_CamInfo.Position.z);
	printfDx("�J���������_���W x:%.4f y:%.4f z:%.4f\n", g_CamInfo.LookAtPosition.x, g_CamInfo.LookAtPosition.y, g_CamInfo.LookAtPosition.z);
	printfDx("�J������] x:%.4f y:%.4f\n", g_CamInfo.HAngle, g_CamInfo.VAngle);
	printfDx("�J�����ݒ苗�� :%.0f\n", g_CamInfo.Distance);
	printfDx("�J���������� :%.4f\n", g_CamInfo.NowDistance);

	printfDx("�v���C���[���W x:%.4f y:%.4f z:%.4f\n", ActionPlayer.pos.x, ActionPlayer.pos.y, ActionPlayer.pos.z);
	printfDx("�v���C���[���x x:%.4f y:%.4f z:%.4f\n", ActionPlayer.speed.x, ActionPlayer.speed.y, ActionPlayer.speed.z);
	printfDx("�v���C���[��� :%s\n", ActionPlayer.Statestr.at(ActionPlayer.state));
	printfDx("�Đ��A�j���[�V�����ԍ� :%d\n", ActionPlayer.NowAnim.playnum);
	printfDx("�A�j���[�V�����Đ��t���[���� :%.1f\n", ActionPlayer.NowAnim.time);
	printfDx("���[�V�����J�b�g�t���[�� :%d\n", ActionPlayer.NowAnim.totaltime);
	if (ActionPlayer.AnimBlendRate != 1.0f) {
		printfDx("���O�Đ��A�j���[�V�����ԍ� :%d\n", ActionPlayer.PrevAnim.playnum);
		printfDx("���O�A�j���[�V�����Đ��t���[���� :%.1f\n", ActionPlayer.PrevAnim.time);
		printfDx("���O���[�V�����J�b�g�t���[�� :%d\n", ActionPlayer.PrevAnim.totaltime);
		printfDx("�A�j���[�V�����u�����h�� :%.1f\n", ActionPlayer.AnimBlendRate);
	}
}