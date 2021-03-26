#include "Camera.h"
#include "Chara_Player.h"
#include "DxMath.h"
#include "StageModel.h"

// �J�����̏��������p�x
#define DEFAULT_H_ANGLE				(DX_PI_F)

// �J�����̏��������p�x
#define DEFAULT_V_ANGLE				(0.4f)

#undef DEFAULT_FOV
// �J�����̉�p
#define DEFAULT_FOV					(40.0f)

#define CAMERA_NEAR_LIM				10.0f
#define CAMERA_FAR_LIM				300.0f

// �J�����̍��W��ڕW���W�ɋ߂Â���v�Z�ŏ��Z�Ɏg�p����l
#define MOVE_DIV_NUM				(0.16666667f)

SCameraInfo g_CamInfo;

bool FPSFlag = false;

// �J�����̏���������������
void Camera_Initialize() {
	extern Chara_Player ActionPlayer;

	// �e�p�����[�^�ɏ����l���Z�b�g����
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

// �J�����̏�Ԑ��ڏ��������s����
void Camera_Step(
	// ���ڂ����鎞��( �P�ʁF�b )
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

	/*// �v���C���[�������_�ɂȂ�̂ŁA�v���C���[�����݂��Ȃ��ꍇ�͉��������I��
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

			if (PrevMouseX != WINDOWS_H / 2) PrevMouseX = MouseX; //1���[�v�O�̃}�E�X��X���W�ۑ�
			if (PrevMouseY != WINDOWS_V / 2) PrevMouseY = MouseY; //1���[�v�O�̃}�E�X��Y���W�ۑ�
			GetMousePoint(&MouseX, &MouseY); //�}�E�X�̍��W�擾
			MouseSpeed_X = static_cast<float>(MouseX - PrevMouseX) / 1000.0f; //�}�E�X��X�ړ������𑬓x�ɕϊ� 
			MouseSpeed_Y = static_cast<float>(MouseY - PrevMouseY) / 1000.0f; //�}�E�X��Y�ړ������𑬓x�ɕϊ�
			SetMousePoint(WINDOWS_H / 2, WINDOWS_V / 2); //�}�E�X����ʒ����Ɉړ�
		}

		// ���������̊p�x�ύX���͂̏���
		if (!FPSFlag) g_CamInfo.HAngle -= MouseSpeed_X;
		else g_CamInfo.HAngle += MouseSpeed_X;

		if (g_CamInfo.HAngle < -DX_PI_F) {
			g_CamInfo.HAngle += DX_TWO_PI_F;
		}
		if (g_CamInfo.HAngle > DX_PI_F) {
			g_CamInfo.HAngle -= DX_TWO_PI_F;
		}

		// ���������̊p�x�ύX���͂̏���
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

	// �J������h�炷�������s�����ǂ����̃t���O�������Ă�����J������h�炷�������s��
	if (g_CamInfo.Shake) {
		// sin ���g�p�����h�炵���W�̎Z�o
		g_CamInfo.ShakePosition.y =
			sin( g_CamInfo.ShakeAngle ) * ( 1.0f - ( g_CamInfo.ShakeTimeCounter /
										g_CamInfo.ShakeTime ) ) * g_CamInfo.ShakeWidth;
		g_CamInfo.ShakePosition.x = 0.0f;
		g_CamInfo.ShakePosition.z = 0.0f;

		// �h�炵�����Ɏg�p���� sin �ɓn���p�x�̕ύX����
		g_CamInfo.ShakeAngle += g_CamInfo.ShakeAngleSpeed * StepTime;
		
		// �h�炷���Ԃ��o�߂�����h�炵�������I������
		g_CamInfo.ShakeTimeCounter += StepTime;
		if (g_CamInfo.ShakeTimeCounter >= g_CamInfo.ShakeTime) {
			g_CamInfo.Shake = false;
		}
	}
	else {
		// �h�炳��Ă��Ȃ��ꍇ�͗h�炵�����ɂ����Z���W���O�ɂ���
		g_CamInfo.ShakePosition = VGet(0.0f, 0.0f, 0.0f);
	}

	// �ڕW�����_���W�̎Z�o
	//if (!g_CamInfo.FocusFlag) {
	if (FPSFlag) {
		g_CamInfo.TargetPosition = ActionPlayer.pos;

		// �J�����̗h�炵�����𖳎������J�����̍��W�A�����_���W�����ꂼ��̖ڕW���W�ɋ߂Â���
		MoveScale = (1.0f / 60.0f) / MOVE_DIV_NUM;

		TempV = g_CamInfo.TargetPosition - g_CamInfo.OrigPosition;
		TempV = VScale(TempV, MoveScale);
		g_CamInfo.OrigPosition = g_CamInfo.OrigPosition + TempV;

		g_CamInfo.OrigPosition = ActionPlayer.pos.cYadd(ActionPlayer.collCap.height*2.0f);
		g_CamInfo.Position = g_CamInfo.OrigPosition + g_CamInfo.ShakePosition;
		
		return;
	}

	g_CamInfo.TargetLookAtPosition = ActionPlayer.pos.cYadd(15.0f*ActionPlayer.scale.y);

	// �ڕW���W�̎Z�o
	g_CamInfo.TargetPosition.x = 0.0f;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.VAngle) * g_CamInfo.Distance;
	g_CamInfo.TargetPosition.y = sin(g_CamInfo.VAngle) * g_CamInfo.Distance;

	g_CamInfo.TargetPosition.x = -sin(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;
	g_CamInfo.TargetPosition.z = cos(g_CamInfo.HAngle) * g_CamInfo.TargetPosition.z;

	g_CamInfo.TargetPosition = VAdd(g_CamInfo.TargetPosition, g_CamInfo.TargetLookAtPosition);

	// �J�����̗h�炵�����𖳎������J�����̍��W�A�����_���W�����ꂼ��̖ڕW���W�ɋ߂Â���
	MoveScale = StepTime / MOVE_DIV_NUM;

	TempV = g_CamInfo.TargetPosition - g_CamInfo.OrigPosition;
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigPosition = g_CamInfo.OrigPosition + TempV;

	TempV = g_CamInfo.TargetLookAtPosition - g_CamInfo.OrigLookAtPosition;
	TempV = VScale(TempV, MoveScale);
	g_CamInfo.OrigLookAtPosition = g_CamInfo.OrigLookAtPosition + TempV;

	// �J�����̗h�炵�����𖳎������J�����̍��W�A�����_���W�ɃJ�����̗h�炵�����ɂ��
	// ���Z���W�𑫂������W�����݂̃J�����̍��W�A�����_���W�Ƃ���
	g_CamInfo.Position = g_CamInfo.OrigPosition + g_CamInfo.ShakePosition;

	g_CamInfo.LookAtPosition = g_CamInfo.OrigLookAtPosition + g_CamInfo.ShakePosition;

	// �J�������猩�ĉE�����̃x�N�g�����Z�o
	g_CamInfo.RightDirection = VNorm(VCross(g_CamInfo.Direction, VGet(0.0f, -1.0f, 0.0f)));

	// �J�����̐��ʕ����̂x�����𔲂����x�N�g�����Z�o
	g_CamInfo.FrontDirection = VNorm(VCross(VGet(0.0f, -1.0f, 0.0f), g_CamInfo.RightDirection));

	if (g_CamInfo.ControllableFlag) {
		// �J�������W���璍���_���W�̊ԂɃR���W�����p�|���S�������݂��邩�`�F�b�N
		if (Stage.HitCheck_Capsule(Capsule::CapGet(g_CamInfo.Position, g_CamInfo.LookAtPosition, g_CamInfo.CollisionSize))) {

			// �R���W�����p�|���S�������݂��Ȃ����W�܂ŃJ�������W�𒍎��_�ɋ߂Â���

			// �����_���W����J�������W�ւ̃x�N�g�����Z�o
			Direction = VNorm(g_CamInfo.Position - g_CamInfo.LookAtPosition);

			// �|���S���ɓ�����Ȃ��������Z�b�g
			NotHitDistance = 0.0f;

			// �|���S���ɓ����鋗�����Z�b�g
			HitDistance = g_CamInfo.Distance;
			do {
				// �����邩�ǂ����e�X�g���鋗�����Z�o( ������Ȃ������Ɠ����鋗���̒��� )
				TestDistance = NotHitDistance + (HitDistance - NotHitDistance) / 2.0f;

				// �e�X�g�p�̃J�������W���Z�o
				TestPosition = g_CamInfo.LookAtPosition + VScale(Direction, TestDistance);

				// �V�������W�ŃR���W�����p�|���S���ɓ����邩�e�X�g
				if (Stage.HitCheck_Capsule(Capsule::CapGet(TestPosition, g_CamInfo.LookAtPosition, g_CamInfo.CollisionSize))) {
					// ���������瓖���鋗���� TestDistance �ɕύX����
					HitDistance = TestDistance;
				}
				else {
					// ������Ȃ������瓖����Ȃ������� TestDistance �ɕύX����
					NotHitDistance = TestDistance;
				}

				// HitDistance �� NoHitDistance ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
			} while (HitDistance - NotHitDistance > 1.0f);
			g_CamInfo.NowDistance = HitDistance;

			// �J�����̍��W���Z�b�g
			g_CamInfo.Position = TestPosition;
			g_CamInfo.OrigPosition = TestPosition;
		}
		else {
			g_CamInfo.NowDistance = g_CamInfo.Distance;
		}
	}

	// ���X�i�[�̈ʒu��ύX
	//Set3DSoundListenerPosAndFrontPos_UpVecY( g_CamInfo.Position, g_CamInfo.TargetLookAtPosition );

	Camera_Setup();
}

// �J�����̐ݒ���s��
void Camera_Setup() {
	// �J�����̎ʂ��͈͂�ݒ�
	SetupCamera_Perspective(radf(g_CamInfo.FOV));
	SetCameraNearFar(CAMERA_NEAR_Z, CAMERA_FAR_Z);
	if (!FPSFlag) SetCameraPositionAndTarget_UpVecY(g_CamInfo.Position, g_CamInfo.LookAtPosition);
	else SetCameraPositionAndAngle(g_CamInfo.Position, g_CamInfo.VAngle, g_CamInfo.HAngle + radf(180.0f), 0.0f);
}

// �J������h�炷�������J�n����
void Camera_Shake(
	// �h��镝
	const float ShakeWidth,

	// �h��鎞��
	const float ShakeTime,

	// �h��̑��x
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

// �J�����̌����Ă���������擾����
//     �߂�l : �J�����̌����Ă�������̃x�N�g��
VECTOR Camera_FrontDirection() {
	return g_CamInfo.FrontDirection;
}

// �J�����̌����Ă���������猩�ĉE�������擾����
//     �߂�l : �J�����̌����Ă���������猩�ĉE�����̃x�N�g�����擾����
VECTOR Camera_RightDirection() {
	return g_CamInfo.RightDirection;
}

