#pragma once

#include "_DxLib.h"

#define MIRROR_NUM			1		// ���̐�


// ���̏�����
void Mirror_Initialize();

void Mirror_Finalize();

// ���ɉf��f����`�悷�邽�߂̃J�����̐ݒ���s��
void Mirror_SetupCamera(const int MirrorNo, const VECTOR CameraEyePos, const VECTOR CameraTargetPos);

// ���̕`��
void Mirror_Render(const int MirrorNo);

// ���̃f�o�b�O�`��
void Mirror_DebugRender(const int MirrorNo, const int DrawX, const int DrawY);