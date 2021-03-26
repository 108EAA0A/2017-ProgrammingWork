#include "_DxLib.h"
#include "DebugTools.h"
#include <math.h>

char DirectionX = '\0';
char DirectionZ = '\0';

bool UpdateKey() {
	extern unsigned int KeyFrame[256];		// �O���[�o���ϐ��g�p�錾
	int tmpMouse;
	char tmpKey[256];						// ���݂̃L�[�̓��͏�Ԃ��i�[����

	tmpMouse = GetMouseInput();
	if (GetHitKeyStateAll(tmpKey) == -1) return false;	// �S�ẴL�[�̓��͏�Ԃ𓾂�

	for (int i = 0; i < 256; ++i) {
		if (tmpKey[i] != 0) ++KeyFrame[i];	// i�Ԃ̃L�[�R�[�h�ɑΉ�����L�[��������Ă�������Z
		else KeyFrame[i] = 0;				// ������Ă��Ȃ����0�ɂ���
	}

	return true;
}

void InputDirection() {
	extern unsigned int KeyFrame[256];
	
	bool DirectionU = false;
	bool DirectionD = false;
	bool DirectionL = false;
	bool DirectionR = false;

	if ((KeyFrame[KEY_INPUT_UP] > 0) || (KeyFrame[KEY_INPUT_W] > 0)) DirectionU = true;
	if ((KeyFrame[KEY_INPUT_DOWN] > 0) || (KeyFrame[KEY_INPUT_S] > 0)) DirectionD = true;
	if ((KeyFrame[KEY_INPUT_LEFT] > 0) || (KeyFrame[KEY_INPUT_A] > 0)) DirectionL = true;
	if ((KeyFrame[KEY_INPUT_RIGHT] > 0) || (KeyFrame[KEY_INPUT_D] > 0)) DirectionR = true;

	if (!DirectionU && !DirectionD && !DirectionL && !DirectionR) DirectionX = '\0', DirectionZ = '\0';

	if (DirectionU && !DirectionD && !DirectionL && !DirectionR) DirectionX = '\0', DirectionZ = 'U';
	if (!DirectionU && DirectionD && !DirectionL && !DirectionR) DirectionX = '\0', DirectionZ = 'D';
	if (!DirectionU && !DirectionD && DirectionL && !DirectionR) DirectionX = 'L', DirectionZ = '\0';
	if (!DirectionU && !DirectionD && !DirectionL && DirectionR) DirectionX = 'R', DirectionZ = '\0';

	if (DirectionU && !DirectionD && DirectionL && !DirectionR) DirectionX = 'L', DirectionZ = 'U';
	if (DirectionU && !DirectionD && !DirectionL && DirectionR) DirectionX = 'R', DirectionZ = 'U';
	if (!DirectionU && DirectionD && DirectionL && !DirectionR) DirectionX = 'L', DirectionZ = 'D';
	if (!DirectionU && DirectionD && !DirectionL && DirectionR) DirectionX = 'R', DirectionZ = 'D';

	if (DirectionU && DirectionD && !DirectionL && !DirectionR) DirectionX = '\0', DirectionZ = '\0';
	if (!DirectionU && !DirectionD && DirectionL && DirectionR) DirectionX = '\0', DirectionZ = '\0';

	if (!DirectionU && DirectionD && DirectionL && DirectionR) DirectionX = '\0', DirectionZ = 'D';
	if (DirectionU && !DirectionD && DirectionL && DirectionR) DirectionX = '\0', DirectionZ = 'U';
	if (DirectionU && DirectionD && !DirectionL && DirectionR) DirectionX = 'R', DirectionZ = '\0';
	if (DirectionU && DirectionD && DirectionL && !DirectionR) DirectionX = 'L', DirectionZ = '\0';
	
	if (DirectionU && DirectionD && DirectionL && DirectionR) DirectionX = '\0', DirectionZ = '\0';
}