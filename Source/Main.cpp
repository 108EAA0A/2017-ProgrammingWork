#include <crtdbg.h>

#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "ConfigData.h"
#include "SceneMgr.h"
#include "FPSCounter.h"

#include "DataIO.h"
#include "RenderProcess.h"
#include "Screenshot.h"
#include "ShadowControl.h"
#include "UpdateKey.h"

#define GAME_VERSION_STR "1.0"

::Effekseer::Manager *EfMgr;// Effekseer��Manager�N���X
//static ::EffekseerRendererDX9::Renderer *EfRend;

CFPSCounter cFPScounter(10);

static bool ProcessLoop() {
	
	if (ScreenFlip() == -1) return false;		//��ʍX�V
	if (ProcessMessage() == -1) return false;	//���b�Z�[�W����
	if (ClearDrawScreen() == -1) return false;	//��ʏ���
	if (!UpdateKey()) return false;				//�L�[���͎擾
	
	return true;
}

static void EndProcess() {
	Effkseer_End();
	DxLib_End();
}

static void errPrintExit(const char *const errpos, const char *const errmsg) {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	clsDx();
	printfDx("��O����\n%s�������F%s\n", errpos, errmsg);
	ScreenFlip();
	WaitKey();
	std::exit(EXIT_FAILURE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//���������[�N���o�c�[��
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::atexit(EndProcess);

	SetUseDXArchiveFlag(TRUE);
	//SetDXArchiveExtension(char *Extension);
	//SetDXArchiveKeyString(char *KeyString);
	SetDXArchiveKeyString("MyPassWord");
	SetDoubleStartValidFlag(FALSE);
	SetAlwaysRunFlag(TRUE);
#ifdef _DEBUG
	SetLogDrawOutFlag(TRUE);
#else
	SetLogDrawOutFlag(FALSE);
#endif

	LoadConfig();
	
	char titlebuf[40];
	snprintf(titlebuf, sizeof(titlebuf), "MMQ ver%s on DxLib ver%s", GAME_VERSION_STR, DXLIB_VERSION_STR_T);
	SetMainWindowText(titlebuf);

	// DirectX9(EX��NG)���g�p����悤�ɂ���B
	// Effekseer���g�p����ɂ͕K���ݒ肷��B
	SetUseDirect3DVersion(DX_DIRECT3D_9);

	SetZBufferBitDepth(32);

	SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
	extern ConfigData cConfigData;
	ChangeWindowMode(!cConfigData.FullScr);
	//ChangeWindowMode(TRUE);	// �t���X�N���[������E�B���h�E���[�h�ɕς���

	// ����������ݒ肷��
	SetWaitVSyncFlag(cConfigData.VSyncFlag);

	// �`�惂�[�h��ݒ肷��
	SetDrawMode(cConfigData.DrawMode);
	
	extern int WINDOWS_H, WINDOWS_V;
	SetGraphMode(WINDOWS_H, WINDOWS_V, 32);

	if (DxLib_Init() == -1) {	// �c�w���C�u�������������� �G���[���N�����璼���ɏI��
		printfDx("DxLib Init error\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	SetDrawScreen(DX_SCREEN_BACK); //����ʐݒ�
	{ int dustbox; GetScreenState(&WINDOWS_H, &WINDOWS_V, &dustbox); }

	if (GetValidShaderVersion() < 200) {
		printfDx("�V�F�[�_�[���f��2.0���g�p�ł��܂���\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	// Effekseer������������B
	// �����ɂ͉�ʂɕ\������ő�p�[�e�B�N������ݒ肷��B
	if (Effkseer_Init(10000) == -1) {
		printfDx("Effekseer Init error\n");
		ScreenFlip();
		WaitKey();
		std::exit(EXIT_FAILURE);
	}

	// �c�݋@�\������
	Effekseer_InitDistortion();

	// �t���X�N���[���E�C���h�E�̐؂�ւ��Ń��\�[�X��������̂�h���B
	// Effekseer���g�p����ꍇ�͕K���ݒ肷��B
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DX���C�u�����̃f�o�C�X���X�g�������̃R�[���o�b�N��ݒ肷��B
	// �E�C���h�E�ƃt���X�N���[���̐؂�ւ�����������ꍇ�͕K�����s����B
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	
	SetMousePoint(WINDOWS_H / 2, WINDOWS_V / 2);
	SetMouseDispFlag(FALSE);
	// �}�E�X�J�[�\���̃E�B���h�E�O�ւ̈ړ����֎~
	//SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);

	SetUseZBufferFlag(TRUE); // �y�o�b�t�@��L���ɂ���
	SetWriteZBufferFlag(TRUE); // �y�o�b�t�@�ւ̏������݂�L���ɂ���

	SetUseZBuffer3D(TRUE); // �R�c�̂y�o�b�t�@��L���ɂ���
	SetWriteZBuffer3D(TRUE); // �R�c�̂y�o�b�t�@�ւ̏������݂�L���ɂ���
	
	extern unsigned int KeyFrame[256];

	SceneMgr sceneMgr;
	sceneMgr.Initialize();

	GameMainRender_Initialize();

	register LONGLONG RefreshTime = 0;

	while (ProcessLoop()) {
		RefreshTime = GetNowHiPerformanceCount(); //���̎��Ԃ��擾

		EfMgr = GetEffekseer3DManager();

		try {
			sceneMgr.Update();  //�X�V
		}
		catch (const char *const errmsg) {
			if (errmsg == "FORCEEND") break;
			errPrintExit("�X�V", errmsg);
		}
		catch (const exception& e) {
			errPrintExit("�X�V", e.what());
		}

		try {
			GameMainRender_FrontProcess();
			sceneMgr.Draw();    //�`��
			GameMainRender_RearProcess();
		}
		catch (const char *const errmsg) {
			if (errmsg == "FORCEEND") break;
			errPrintExit("�`��", errmsg);
		}
		catch (const exception& e) {
			errPrintExit("�`��", e.what());
		}

		if (KeyFrame[KEY_INPUT_F12] == 1) Screenshot();

		while (GetNowHiPerformanceCount() - RefreshTime < 16667); //1���̏�����16.667�~���b�ɂȂ�܂ő҂�
	}

	sceneMgr.Finalize();

	GameMainRender_Terminate();

	return 0;
}