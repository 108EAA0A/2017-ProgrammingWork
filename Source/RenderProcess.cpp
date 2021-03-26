#include "RenderProcess.h"

#include "_DxLib.h"
#include "Camera.h"
#include "ConfigData.h"


// ��`��p���
// ��U�����ɑS�Ă̕`�揈����̌��ʂ�`�悵�Ă��痠��ʂɁi�K�v�Ȃ�g��k�����āj�`�悷��
int MainScreenHandle;

//int MaskHandle;


// �Q�[�����C���̕`�揈��������������
void GameMainRender_Initialize() {
	extern ConfigData cConfigData;
	if (cConfigData.AntialiasFlag) {
		// �A���`�G�C���A�X�����_�����O�p�̕`��Ώۂɂł���O���t�B�b�N�n���h�����쐬����
		SetCreateDrawValidGraphMultiSample(4, 4);
		MainScreenHandle = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
		SetCreateDrawValidGraphMultiSample(0, 0);
	}
	else {
		SetCreateDrawValidGraphMultiSample(0, 0);
		MainScreenHandle = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
	}

	// �}�X�N��ʂ��쐬���܂�
	//CreateMaskScreen();

	// ��̃}�X�N���쐬���܂�
	//MaskHandle = LoadMask(_T("BlendGraph.bmp"));

}

// �Q�[�����C���̕`�揈���̌�n��������
void GameMainRender_Terminate() {
	// ��`��p��ʂ��폜
	DeleteGraph(MainScreenHandle);

	//DeleteMask(MaskHandle);

	// �}�X�N��ʂ��폜���܂�
	//DeleteMaskScreen();
}

void GameMainRender_FrontProcess() {
	// �`������`��p��ʂɐݒ�
	SetDrawScreen(MainScreenHandle);

	ClearDrawScreen();

	// SetDrawScreen �����s����ƃJ�����̐ݒ肪���Z�b�g����Ă��܂��̂ŁA
	// �J�����̐ݒ���ēx���s
	Camera_Setup();
}

void GameMainRender_RearProcess() {
	extern int WINDOWS_H, WINDOWS_V;
	
	// �`���𗠉�ʂɐݒ�
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	//DrawMask(0, 0, MaskHandle, DX_MASKTRANS_NONE);

	// ��`��p��ʂ𗠉�ʂɕ`��
	DrawExtendGraph(0, 0, WINDOWS_H, WINDOWS_V, MainScreenHandle, FALSE);

	// SetDrawScreen �����s����ƃJ�����̐ݒ肪���Z�b�g����Ă��܂��̂ŁA
	// �J�����̐ݒ���ēx���s
	Camera_Setup();
}