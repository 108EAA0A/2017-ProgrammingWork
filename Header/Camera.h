#pragma once

#include "_DxLib.h"

// �J�����̎�O�N���b�v����
#define CAMERA_NEAR_Z		(1.0f)

// �J�����̉��N���b�v����
#define CAMERA_FAR_Z		(10000.0f)

// �J�����̏��
typedef struct _SCameraInfo {
	// �J�����̉�p
	float FOV;

	// �J�����̍��W
	VECTOR Position;

	// �t�H�[�J�X���[�h���ǂ���
	bool FocusFlag;

	bool ControllableFlag;
	
	// �J�����̒����_���W
	VECTOR LookAtPosition;

	// �J�����h�炵���W�𖳎������J�����̍��W
	VECTOR OrigPosition;

	// �J�����h�炵���W�𖳎������J�����̒����_���W
	VECTOR OrigLookAtPosition;

	// �J�����̖ڕW���W
	VECTOR TargetPosition;

	// �J�����̖ڕW�����_���W
	VECTOR TargetLookAtPosition;

	// �J�����̒����_�ɑ΂��鐅���p�x
	float  HAngle;

	// �J�����̒����_�ɑ΂��鐂���p�x
	float  VAngle;

	// �J�����̍��W����J�����̒����_���W�܂ł̐ݒ肵������
	float  Distance;

	// �J�����̍��W����J�����̒����_���W�܂ł̌��݂̋���
	float  NowDistance;

	// �J�����̍��W����J�����̒����_���W�ւ̃x�N�g��
	VECTOR Direction;

	// �J�������猩�ĉE�����̃x�N�g��
	VECTOR RightDirection;

	// �J�����̐��ʕ����̂x�����𔲂����x�N�g��
	VECTOR FrontDirection;

	// �J�����̓����蔻��T�C�Y
	float  CollisionSize;

	// �J������h�炷�������s���Ă��邩�ǂ���
	bool   Shake;

	// �J������h�炷��
	float  ShakeWidth;

	// �J������h�炷����
	float  ShakeTime;

	// �J������h�炷�����̌o�ߎ��Ԍv���p�ϐ�
	float  ShakeTimeCounter;

	// �J������h�炷�����Ŏg�p���� sin �ɓn���p�x
	float  ShakeAngle;

	// �J������h�炷�����Ŏg�p���� sin �ɓn���p�x�̕ω����x
	float  ShakeAngleSpeed;

	// �J������h�炷�����ŎZ�o�����J�������W�ɑ������W
	VECTOR ShakePosition;
} SCameraInfo;

// �J�����̏���������������
extern void Camera_Initialize();

// �J�����̏�Ԑ��ڏ��������s����
extern void Camera_Step(
	// ���ڂ����鎞��( �P�ʁF�b )
	const float StepTime
);

// �J�����̐ݒ���s��
extern void Camera_Setup();

// �J������h�炷�������J�n����
extern void Camera_Shake(
	// �h��镝
	const float ShakeWidth,

	// �h��鎞��
	const float ShakeTime,

	// �h��̑��x
	const float ShakeAngleSpeed
);

// �J�����̌����Ă���������擾����
//     �߂�l : �J�����̌����Ă�������̃x�N�g��
extern VECTOR Camera_FrontDirection( void );

// �J�����̌����Ă���������猩�ĉE�������擾����
//     �߂�l : �J�����̌����Ă���������猩�ĉE�����̃x�N�g�����擾����
extern VECTOR Camera_RightDirection( void );