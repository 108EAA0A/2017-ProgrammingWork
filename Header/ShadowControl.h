#pragma once

#include "_DxLib.h"

/* �V���h�E�}�b�v�\���� */
struct ShadowMapStr {
	int Handle;
	//int Definition;
	VECTOR MinPos;
	VECTOR MaxPos;
};

typedef void(*drawFuncPtr)(bool);

// �e�`��̏���������
void ShadowRender_Initialize();

// �e�`��̌�n��������
void ShadowRender_Terminate();

// �X�e�[�W�J�n���̕`�揀�����s��
void ShadowRender_StageSetup(const VECTOR LightDirection);

// �V���h�E�}�b�v�ɉe�𗎂Ƃ�
void ShadowRender_Setup(const VECTOR CenterPos);

// �e�̕`��J�n
void ShadowRender_DrawStart();

// �e�̕`��I��
void ShadowRender_DrawEnd();

// �e�`��̃f�o�b�O�`�揈��
void ShadowRender_DrawDebug();

//void PointShadowRender(const VECTOR Pos, const Capsule& Cap);