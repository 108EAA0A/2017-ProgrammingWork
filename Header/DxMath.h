#pragma once

/* �x���@�����W�A���ɕϊ� */
inline float radf(const float Angle) {
	return (Angle * DX_PI_F / 180.0f);
}

/* ���W�A���ɓx���@��ϊ� */
inline float anglef(const float Rad) {
	return (Rad * 180.0f / DX_PI_F);
}

/* �����p�ݏ�֐� */
inline int powi(const int X, const int Y) {
	return static_cast<int>(powf(static_cast<float>(X), static_cast<float>(Y)));
}

//int FColm2Colm(const float mem);

//unsigned int FCol2Col(const COLOR_F ColF);

bool ParamChangeInt(
	// ����Ώۂ̕ϐ��̃A�h���X
	int *Param,

	// �ڕW�Ƃ���l
	int TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �l�̕ω����x
	float Speed
);

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
);

// ���������_�^�̕ϐ��ɑ������Ă���p�x���w��̊p�x�ɋ߂Â��鏈�����s��
// ( �ڕW�Ƃ���p�x�ƕϐ��ɑ������Ă���p�x�̍��� 2.0f * �� �ȓ��ł���K�v������ )
//     �߂�l : �ϐ��̊p�x���w��̊p�x�ɒB�������ǂ���
//              ( true : �w��̊p�x�ɒB����  false : �w��̊p�x�ɒB���Ă��Ȃ� )
bool ParamChangeAngle(
	// ����Ώۂ̕ϐ��̃A�h���X
	float *Param,

	// �ڕW�Ƃ���p�x
	float TargetParam,

	// ���ڂ����鎞��
	float StepTime,

	// �p�x�̕ω����x
	float Speed
);

VECTOR WorldMatrix2Pos(MATRIX);