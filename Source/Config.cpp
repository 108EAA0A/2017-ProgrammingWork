#include "Config.h"

#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "ConfigData.h"
#include "DataIO.h"
#include "RenderProcess.h"
#include "Screenshot.h"
#include "Structure.h"
#include "UpdateKey.h"

enum eConfig {
	eConfig_MasterVol,	//�剹�ʐݒ�
	eConfig_BGMVol,		//BGM���ʐݒ�
	eConfig_SEVol,		//SE���ʐݒ�
	eConfig_WindowMode,	//�E�B���h�E���[�h�ݒ�
	eConfig_Resolution,	//�𑜓x�ݒ�
	eConfig_DrawMode,	//�`�惂�[�h�ݒ�
	eConfig_AntiAlias,	//�A���`�G�C���A�X�ݒ�
	eConfig_VSync,		//���������ݒ�
	eConfig_Shadow,		//�e�ݒ�
	eConfig_Reflection,	//���ːݒ�
	eConfig_SSType,		//�X�N���[���V���b�g�`���ݒ�
	eConfig_Back,		//�߂�

	eConfig_Num,		//�{���ڂ̐�
};

extern unsigned int KeyFrame[256];

extern int WINDOWS_H, WINDOWS_V;

static int NowSelect = eConfig_MasterVol;    //���݂̑I�����(�����͑�1����)

static const int MasterVol_Y = 240;
static const int BGMVol_Y = 270;
static const int SEVol_Y = 300;
static const int WindowMode_Y = 330;
static const int Resolution_Y = 360;
static const int DrawMode_Y = 390;
static const int Antialias_Y = 420;
static const int VSync_Y = 450;
static const int Shadow_Y = 480;
static const int Reflection_Y = 510;
static const int SSType_Y = 540;
static const int BACK_Y = 570;

Config::Config(ISceneChanger *const changer) : BaseScene(changer) {
}

//������
void Config::Initialize() {
	SetFontSize(16);
}

//�I������
void Config::Finalize() {
	SaveConfig();
	ClearDrawScreen();
	ScreenFlip();
}

//�X�V
void Config::Update() {
	BaseScene::Draw();//�e�N���X�̕`�惁�\�b�h���Ă�
	
	extern ConfigData cConfigData;
	static int PrevResolution = cConfigData.WindowResolution;

	if (KeyFrame[KEY_INPUT_ESCAPE] != 0) {//Esc�L�[��������Ă�����
		mSceneChanger->ChangeScene(eScene_Title);//�V�[�����^�C�g���ɕύX
	}
	if (KeyFrame[KEY_INPUT_DOWN] == 1) {//���L�[��������Ă�����
		NowSelect = (NowSelect + 1) % eConfig_Num;	//�I����Ԃ��������
	}
	if (KeyFrame[KEY_INPUT_UP] == 1) {//��L�[��������Ă�����
		NowSelect = (NowSelect + (eConfig_Num - 1)) % eConfig_Num;	//�I����Ԃ���グ��
	}

	if (KeyFrame[KEY_INPUT_LEFT] > 50) {
		switch (NowSelect) {			//���ݑI�𒆂̏�Ԃɂ���ď����𕪊�
		case eConfig_MasterVol:
			cConfigData.MasterVol -= 2;
			if (cConfigData.MasterVol < 0x00) cConfigData.MasterVol = 0x00;
			break;
		case eConfig_BGMVol:
			cConfigData.BGMVol -= 2;
			if ((cConfigData.BGMVol == 0xFE) || (cConfigData.BGMVol == 0xFF)) cConfigData.BGMVol = 0x00;
			break;
		case eConfig_SEVol:
			cConfigData.SEVol -= 2;
			if ((cConfigData.SEVol == 0xFE) || (cConfigData.SEVol == 0xFF)) cConfigData.SEVol = 0x00;
			break;
		}
	}
	if (KeyFrame[KEY_INPUT_RIGHT] > 50) {
		switch (NowSelect) {			 //���ݑI�𒆂̏�Ԃɂ���ď����𕪊�
		case eConfig_MasterVol:
			cConfigData.MasterVol += 2;
			if (cConfigData.MasterVol > 100) cConfigData.MasterVol = 100;
			break;
		case eConfig_BGMVol:
			cConfigData.BGMVol += 2;
			if ((cConfigData.BGMVol == 0x00) || (cConfigData.BGMVol == 0x01)) cConfigData.BGMVol = 0xFF;
			break;
		case eConfig_SEVol:
			cConfigData.SEVol += 2;
			if ((cConfigData.SEVol == 0x00) || (cConfigData.SEVol == 0x01)) cConfigData.SEVol = 0xFF;
			break;
		}
	}

	if (KeyFrame[KEY_INPUT_LEFT] == 1) {//���L�[��������Ă�����
		switch (NowSelect) {			//���ݑI�𒆂̏�Ԃɂ���ď����𕪊�
			case eConfig_MasterVol:
				--cConfigData.MasterVol;
				if (cConfigData.MasterVol < 0x00) cConfigData.MasterVol = 0x00;
				break;
			case eConfig_BGMVol:
				if (cConfigData.BGMVol != 0x00) --cConfigData.BGMVol;
				break;
			case eConfig_SEVol:
				if (cConfigData.SEVol != 0x00) --cConfigData.SEVol;
				break;
			case eConfig_WindowMode:
				cConfigData.FullScr = false;
				ChangeWindowMode(TRUE);
				extern int MainScreenHandle;
				SetDrawScreen(MainScreenHandle);
				SetMouseDispFlag(FALSE);
				Effekseer_InitDistortion();	// �c�݋@�\������
				break;
			case eConfig_Resolution:
				if (cConfigData.WindowResolution != BlackBerry) --cConfigData.WindowResolution;
				break;
			case eConfig_DrawMode:
				cConfigData.DrawMode = (cConfigData.DrawMode + (DX_DRAWMODE_NUM - 2)) % (DX_DRAWMODE_NUM - 1);
				break;
			case eConfig_AntiAlias:
				cConfigData.AntialiasFlag = false;
				GameMainRender_Terminate();
				GameMainRender_Initialize();
				break;
			case eConfig_VSync:
				cConfigData.VSyncFlag = false;
				break;
			case eConfig_Shadow:
				cConfigData.ShadowLevel = (cConfigData.ShadowLevel + (LevelNum - 1)) % LevelNum;
				break;
			case eConfig_Reflection:
				cConfigData.ReflectionLevel = (cConfigData.ReflectionLevel + (LevelNum - 1)) % LevelNum;
				break;
			case eConfig_SSType:
				cConfigData.SSType = (cConfigData.SSType + (eSSType_Num - 1)) % eSSType_Num;
				break;
		}
	}
	
	if (KeyFrame[KEY_INPUT_RIGHT] == 1) {//�E�L�[��������Ă�����
		switch (NowSelect) {			 //���ݑI�𒆂̏�Ԃɂ���ď����𕪊�
			case eConfig_MasterVol:
				++cConfigData.MasterVol;
				if (cConfigData.MasterVol > 100) cConfigData.MasterVol = 100;
				break;
			case eConfig_BGMVol:
				if (cConfigData.BGMVol != 0xFF) ++cConfigData.BGMVol;
				break;
			case eConfig_SEVol:
				if (cConfigData.SEVol != 0xFF) ++cConfigData.SEVol;
				break;
			case eConfig_WindowMode:
				cConfigData.FullScr = true;
				ChangeWindowMode(FALSE);
				extern int MainScreenHandle;
				SetDrawScreen(MainScreenHandle);
				SetMouseDispFlag(FALSE);
				Effekseer_InitDistortion();	// �c�݋@�\������
				break;
			case eConfig_Resolution:
				if (cConfigData.WindowResolution != FullHD) ++cConfigData.WindowResolution;
				break;
			case eConfig_DrawMode:
				cConfigData.DrawMode = (cConfigData.DrawMode + 1) % (DX_DRAWMODE_NUM - 1);
				break;
			case eConfig_AntiAlias:
				cConfigData.AntialiasFlag = true;
				GameMainRender_Terminate();
				GameMainRender_Initialize();
				break;
			case eConfig_VSync:
				cConfigData.VSyncFlag = true;
				break;
			case eConfig_Shadow:
				cConfigData.ShadowLevel = (cConfigData.ShadowLevel + 1) % LevelNum;
				break;
			case eConfig_Reflection:
				cConfigData.ReflectionLevel = (cConfigData.ReflectionLevel + 1) % LevelNum;
				break;
			case eConfig_SSType:
				cConfigData.SSType = (cConfigData.SSType + 1) % eSSType_Num;
				break;
		}
	}

	if (PrevResolution != cConfigData.WindowResolution) {
		switch (cConfigData.WindowResolution) {
			case BlackBerry: WINDOWS_H = 640;  WINDOWS_V = 480;  break;
			case SVGA:		 WINDOWS_H = 800;  WINDOWS_V = 600;  break;
			case XGA:		 WINDOWS_H = 1024; WINDOWS_V = 768;  break;
			case SXGA:		 WINDOWS_H = 1280; WINDOWS_V = 1024; break;
			case HDTV:		 WINDOWS_H = 1080; WINDOWS_V = 720;  break;
			case FullHD:	 WINDOWS_H = 1920; WINDOWS_V = 1080; break;
		}
		SetGraphMode(WINDOWS_H, WINDOWS_V, 32);
		SetMouseDispFlag(FALSE);
		// �c�݋@�\������
		Effekseer_InitDistortion();
	}

	if (KeyFrame[KEY_INPUT_RETURN] == 1) {//�G���^�[�L�[�������ꂽ��
		if (NowSelect == eConfig_Back) {
			mSceneChanger->ChangeScene(eScene_Title);//�V�[�����^�C�g���ɕύX
		}
	}

}

//�`��
void Config::Draw() {
	extern ConfigData cConfigData;

	DrawFillBox(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, DxColor::Black);
	DrawString(200, 150, "�ݒ��ʂł��B", DxColor::White);
	DrawString(200, 170, "Esc�L�[�������ƃ��j���[��ʂɖ߂�܂��B", DxColor::White);

	DrawFormatString(280, MasterVol_Y, DxColor::White, "�}�X�^�[���� : %d", cConfigData.MasterVol);
	DrawFormatString(280, BGMVol_Y, DxColor::White, "BGM���� : %d", cConfigData.BGMVol);
	DrawFormatString(280, SEVol_Y, DxColor::White, "SE���� : %d", cConfigData.SEVol);

	DrawFormatString(280, WindowMode_Y, DxColor::White, "�t���X�N���[�� : %s", (GetWindowModeFlag() == TRUE ? "OFF" : "ON"));
	
	switch (cConfigData.WindowResolution) {
		case BlackBerry:	DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 640�~480");   break;
		case SVGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 800�~600");   break;
		case XGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 1024�~768");  break;
		case SXGA:			DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 1280�~1024"); break;
		case HDTV:			DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 1280�~720");  break;
		case FullHD:		DrawFormatString(280, Resolution_Y, DxColor::White, "�𑜓x : 1920�~1080"); break;
	}

	switch (cConfigData.DrawMode) {
		case DX_DRAWMODE_NEAREST:		DrawFormatString(280, DrawMode_Y, DxColor::White, "�`�惂�[�h : �j�A���X�g�l�C�o�[�@");	break;
		case DX_DRAWMODE_BILINEAR:		DrawFormatString(280, DrawMode_Y, DxColor::White, "�`�惂�[�h : �o�C���j�A�@");			break;
		case DX_DRAWMODE_ANISOTROPIC:	DrawFormatString(280, DrawMode_Y, DxColor::White, "�`�惂�[�h : �A���`�G�C���A�X�@");	break;
	}

	DrawFormatString(280, Antialias_Y, DxColor::White, "�A���`�G�C���A�X : %s", (cConfigData.AntialiasFlag ? "ON" : "OFF"));
	
	DrawFormatString(280, VSync_Y, DxColor::White, "VSync : %s", (cConfigData.VSyncFlag ? "ON" : "OFF"));
	
	switch (cConfigData.ShadowLevel) {
		case Lowest:	DrawFormatString(280, Shadow_Y, DxColor::White, "�e�i�� : �Œ�i�Ȃ��j");	break;
		case Low:		DrawFormatString(280, Shadow_Y, DxColor::White, "�e�i�� : ��");				break;
		case Medium:	DrawFormatString(280, Shadow_Y, DxColor::White, "�e�i�� : ��");				break;
		case High:		DrawFormatString(280, Shadow_Y, DxColor::White, "�e�i�� : ��");				break;
		case Highest:	DrawFormatString(280, Shadow_Y, DxColor::White, "�e�i�� : �ō�");			break;
	}

	switch (cConfigData.ReflectionLevel) {
		case Lowest:	DrawFormatString(280, Reflection_Y, DxColor::White, "���˕i�� : �Œ�i�Ȃ��j"); break;
		case Low:		DrawFormatString(280, Reflection_Y, DxColor::White, "���˕i�� : ��");			break;
		case Medium:	DrawFormatString(280, Reflection_Y, DxColor::White, "���˕i�� : ��");			break;
		case High:		DrawFormatString(280, Reflection_Y, DxColor::White, "���˕i�� : ��");			break;
		case Highest:	DrawFormatString(280, Reflection_Y, DxColor::White, "���˕i�� : �ō�");			break;
	}

	switch (cConfigData.SSType) {
		case eSSType_BMP: DrawFormatString(280, SSType_Y, DxColor::White, "�X�N���[���V���b�g�`�� : BMP"); break;
		case eSSType_PNG: DrawFormatString(280, SSType_Y, DxColor::White, "�X�N���[���V���b�g�`�� : PNG"); break;
		case eSSType_JPG: DrawFormatString(280, SSType_Y, DxColor::White, "�X�N���[���V���b�g�`�� : JPG"); break;
	}

	DrawString(280, BACK_Y, "�߂�", DxColor::White);

	int y = 0;
	switch (NowSelect) {	//���݂̑I����Ԃɏ]���ď����𕪊�
		case eConfig_MasterVol:		y = MasterVol_Y;	break;
		case eConfig_BGMVol:		y = BGMVol_Y;		break;
		case eConfig_SEVol:			y = SEVol_Y;		break;
		case eConfig_WindowMode:	y = WindowMode_Y;	break;
		case eConfig_Resolution:	y = Resolution_Y;	break;
		case eConfig_DrawMode:		y = DrawMode_Y;		break;
		case eConfig_AntiAlias:		y = Antialias_Y;	break;
		case eConfig_VSync:			y = VSync_Y;		break;
		case eConfig_Shadow:		y = Shadow_Y;		break;
		case eConfig_Reflection:	y = Reflection_Y;	break;
		case eConfig_SSType:		y = SSType_Y;		break;
		case eConfig_Back:			y = BACK_Y;			break;
	}
	DrawString(250, y, "��", DxColor::White);
}