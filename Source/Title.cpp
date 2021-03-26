#include "Title.h"

#include "_DxLib.h"
#include "NowLoading.h"
#include "RenderProcess.h"
#include "SoundAsset.h"
#include "UpdateKey.h"

enum eTitle_Select {
	eTitle_Action,	// �Q�[���I��
	eTitle_Config,		// �ݒ�
	eTitle_Exit,		// �I��

	eTitle_Num,			// �I�����ڂ̐�

	eTitle_Title,		// �^�C�g�����
};

extern unsigned int KeyFrame[256];

static int NowScene = eTitle_Title;
static int NowSelect = eTitle_Action;    //���݂̑I�����(�����̓Q�[���I��)

static int TitleBG;
static SoundAsset *Enter;

Title::Title(ISceneChanger *const changer) : BaseScene(changer) {
}

//������
void Title::Initialize() {
	NowScene = eTitle_Title;
	NowSelect = eTitle_Action;

	SetUseASyncLoadFlag(TRUE);

	TitleBG = LoadGraph("resource/���摜.png");
	//TitleBGM = new SoundAsset("resource/Sound/BGM/Title.ogg", SoundType::BGM);
	Enter = new SoundAsset("resource/Sound/SE/se_maoudamashii_system37.ogg", SoundType::SE);

	NowLoading();

	//TitleBGM->play(true);

	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

}

//�I������
void Title::Finalize() {
	DeleteGraph(TitleBG);
	//delete TitleBGM;
	delete Enter;

	ClearDrawScreen();
	ScreenFlip();
	SetFontSize(20);
}

//�X�V
void Title::Update() {
	if (KeyFrame[KEY_INPUT_DOWN] == 1) {//���L�[��������Ă�����
		NowSelect = (NowSelect + 1) % eTitle_Num;//�I����Ԃ��������
		Enter->playMulti();
	}
	if (KeyFrame[KEY_INPUT_UP] == 1) {//��L�[��������Ă�����
		NowSelect = (NowSelect + (eTitle_Num - 1)) % eTitle_Num;//�I����Ԃ���グ��
		Enter->playMulti();
	}
	if (KeyFrame[KEY_INPUT_RETURN] == 1) {//�G���^�[�L�[�������ꂽ��
		switch (NowSelect) {
		case eTitle_Action:
			mSceneChanger->ChangeScene(eScene_Action);
			break;
		case eTitle_Config:
			mSceneChanger->ChangeScene(eScene_Config);
			break;
		case eTitle_Exit:
			FORCEEND;
		default:
			mythrow("�I���O�ł�");
		}
	}
}

static int DrawCenterX(const char *const str) {
	return (BASE_RENDER_SIZE_W - GetDrawStringWidth(str, strlen(str))) / 2;
}

//�`��
void Title::Draw() {
	int y = 0;
	
	DrawGraph(0, 0, TitleBG, FALSE);

	switch (NowScene) {
	case eTitle_Title:
	{
		const int START_Y = 530;   //�u�͂��߂���v������y�ʒu
		const int CONFIG_Y = 600;  //�u�ݒ�v������y�ʒu
		const int EXIT_Y = 760;    //�u�I���v������y�ʒu

		const int RoundRectCenterY = 150;
		const int RoundRectW = 600, RoundRectH = 400;

		//DrawFillBox(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, DxColor::Black);
		SetDrawBlendMode(DX_BLENDMODE_MUL, 200);
		//DrawRoundRect((BASE_RENDER_SIZE_W - RoundRectW) / 2, (BASE_RENDER_SIZE_H - RoundRectH) / 2, (BASE_RENDER_SIZE_W - RoundRectW) / 2, BASE_RENDER_SIZE_H - (BASE_RENDER_SIZE_H - RoundRectH) / 2, 50, 50, DxColor::Blue, FALSE);
		DrawRoundRect((BASE_RENDER_SIZE_W - RoundRectW) / 2, RoundRectCenterY + (BASE_RENDER_SIZE_H - RoundRectH) / 2, (BASE_RENDER_SIZE_W - RoundRectW) / 2 + RoundRectW, RoundRectCenterY + (BASE_RENDER_SIZE_H - RoundRectH) / 2 + RoundRectH, 50, 50, DxColor::Blue, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		
		SetFontSize(120);
		DrawString(DrawCenterX("�Q�[���^�C�g��"), 200, "�Q�[���^�C�g��", DxColor::White);
		SetFontSize(60);
		DrawString(DrawCenterX("�͂��߂���"), START_Y, "�͂��߂���", DxColor::White);
		DrawString(DrawCenterX("�ݒ�"), CONFIG_Y, "�ݒ�", DxColor::White);
		DrawString(DrawCenterX("�I��"), EXIT_Y, "�I��", DxColor::White);

		switch (NowSelect) {
		case eTitle_Action:
			y = START_Y;
			break;
		case eTitle_Config:
			y = CONFIG_Y;
			break;
		case eTitle_Exit:
			y = EXIT_Y;
			break;
		}
		DrawString(700, y, "��", DxColor::White);
	}
		break;

	case eTitle_Config:
		break;
	}
}