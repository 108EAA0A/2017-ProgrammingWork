#include "_DxLib.h"
#include "DxMath.h"

bool ParamChangeInt(
	// ����Ώۂ̕ϐ��̃A�h���X
	int *const Param,

	// �ڕW�Ƃ���l
	int TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �l�̕ω����x
	float Speed
)
{
	// �ϐ��̒l���ڕW�Ƃ���l��菬�������ǂ����ŏ����𕪊�
	if (*Param < TargetParam) {
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l�ɕω��l�����Z����
		*Param += static_cast<int>(Speed * StepTime);

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if (*Param >= TargetParam) {
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}
	else {
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l����ω��l�����Z����
		*Param -= static_cast<int>(Speed * StepTime);

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if (*Param <= TargetParam) {
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}

	// �����ɗ�����܂��ڕW�̒l�ɒB���Ă��Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
	return false;
}

// ���������_�^�̕ϐ��̒l���w��̒l�ɋ߂Â��鏈�����s��
//     �߂�l : �ϐ��̒l���w��̒l�ɒB�������ǂ���
//              ( true : �w��̒l�ɒB����  false : �w��̒l�ɒB���Ă��Ȃ� )
bool ParamChangeFloat(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,

	// �ڕW�Ƃ���l
	float TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �l�̕ω����x
	float Speed
)
{
	// �ϐ��̒l���ڕW�Ƃ���l��菬�������ǂ����ŏ����𕪊�
	if (*Param < TargetParam) {
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l�ɕω��l�����Z����
		*Param += Speed * StepTime;

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if (*Param >= TargetParam) {
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}
	else {
		// �ڕW�̒l��菬�����ꍇ�͕ϐ��̒l����ω��l�����Z����
		*Param -= Speed * StepTime;

		// �ڕW�̒l�ɒB���Ă��邩�ǂ����̔���
		if (*Param <= TargetParam) {
			// �B���Ă�����ϐ��ɖڕW�̒l�������� true ��Ԃ�
			*Param = TargetParam;
			return true;
		}
	}

	// �����ɗ�����܂��ڕW�̒l�ɒB���Ă��Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
	return false;
}


// ���������_�^�̕ϐ��ɑ������Ă���p�x���w��̊p�x�ɋ߂Â��鏈�����s��
// ( �ڕW�Ƃ���p�x�ƕϐ��ɑ������Ă���p�x�̍��� 2.0f * �� �ȓ��ł���K�v������ )
//     �߂�l : �ϐ��̊p�x���w��̊p�x�ɒB�������ǂ���
//              ( true : �w��̊p�x�ɒB����  false : �w��̊p�x�ɒB���Ă��Ȃ� )
bool ParamChangeAngle(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *const Param,

	// �ڕW�Ƃ���p�x
	float TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �p�x�̕ω����x
	float Speed
)
{
	float RelateAngle;

	// �ڕW�Ƃ���p�x�ƕϐ��̊p�x�Ƃ̍����Z�o
	RelateAngle = TargetParam - *Param;

	// �p�x�̍����P�W�O�x�ȏ�̏ꍇ�͊p�x�̍������߂�������t�]������
	if (RelateAngle < -DX_PI_F) {
		RelateAngle += DX_TWO_PI_F;
	}
	else if (RelateAngle > DX_PI_F) {
		RelateAngle -= DX_TWO_PI_F;
	}

	// �p�x�̍����O�ɋ߂Â���
	if (ParamChangeFloat(&RelateAngle, 0.0f, StepTime, Speed)) {
		// �p�x�̍����O�ɂȂ�����ڕW�Ƃ���p�x�ɂȂ�Ƃ������ƂȂ̂�
		// �ϐ��ɖڕW�̊p�x�������� true ��Ԃ�
		*Param = TargetParam;
		return true;
	}

	// �ڕW�̊p�x����p�x�̍������������̂�V�����p�x�Ƃ���
	*Param = TargetParam - RelateAngle;

	// �܂��ڕW�̊p�x�ɒB���Ă��Ȃ��̂� false ��Ԃ�
	return false;
}

VECTOR WorldMatrix2Pos(MATRIX WorldMatrix) {
	return VGet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2]);
}