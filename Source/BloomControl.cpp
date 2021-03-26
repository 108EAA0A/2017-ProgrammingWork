#include "_DxLib.h"
#include "RenderProcess.h"

#define DOWN_SCALE			8	// �K�E�X�t�B���^���|����摜����ʂ̃T�C�Y�̉����̂P��
#define BRIGHT_CLIP_BORDER	235	// ���P�x�����𔲂��o�����E�l
#define GAUSS_RATIO			800	// �K�E�X�t�B���^�̂ڂ������x

static int HighBrightScreen;	// ���ʂ̕`�挋�ʂ��獂�P�x�����𔲂��o�������ʂ��������ރX�N���[��
static int DownScaleScreen;		// ���P�x�������k���������ʂ��������ރX�N���[��
static int GaussScreen;			// �k���摜���K�E�X�t�B���^�łڂ��������ʂ��������ރX�N���[��

void Bloom_Init() {
	SetCreateGraphColorBitDepth(16);
	HighBrightScreen = MakeScreen(BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, FALSE);
	DownScaleScreen	 = MakeScreen(BASE_RENDER_SIZE_W / DOWN_SCALE, BASE_RENDER_SIZE_H / DOWN_SCALE, FALSE);
	GaussScreen		 = MakeScreen(BASE_RENDER_SIZE_W / DOWN_SCALE, BASE_RENDER_SIZE_H / DOWN_SCALE, FALSE);
	SetCreateGraphColorBitDepth(32);
}

void Bloom_Finalize() {
	DeleteGraph(HighBrightScreen);
	DeleteGraph(DownScaleScreen);
	DeleteGraph(GaussScreen);
}

void Bloom_Setup() {
	extern int MainScreenHandle;

	// �`�挋�ʂ��獂�P�x�����݂̂𔲂��o�����摜�𓾂�
	// BRIGHT_CLIP_BORDER��薾�x�������������͍��œh��Ԃ�
	GraphFilterBlt(MainScreenHandle, HighBrightScreen, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, BRIGHT_CLIP_BORDER, TRUE, DxColor::Black, 0xFF);

	// ���P�x�������W���̂P�ɏk�������摜�𓾂�
	GraphFilterBlt(HighBrightScreen, DownScaleScreen, DX_GRAPH_FILTER_DOWN_SCALE, DOWN_SCALE);

	// �k�������摜���K�E�X�t�B���^�łڂ���
	GraphFilterBlt(DownScaleScreen, GaussScreen, DX_GRAPH_FILTER_GAUSS, 16, GAUSS_RATIO);
}

void Bloom_Draw() {
	const int DrawModebuf = GetDrawMode();
	
	// �`�惂�[�h���o�C���j�A�t�B���^�����O�ɂ���( �g�債���Ƃ��Ƀh�b�g���ڂ₯��悤�ɂ��� )
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	// �`��u�����h���[�h�����Z�ɂ���
	SetDrawBlendMode(DX_BLENDMODE_ADD, 0x3F);

	// ���P�x�������k�����Ăڂ������摜����ʂ����ς��ɂQ��`�悷��( �Q��`�悷��̂͂�薾�邭�݂���悤�ɂ��邽�� )
	DrawExtendGraph(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, GaussScreen, FALSE);
	//DrawExtendGraph(0, 0, BASE_RENDER_SIZE_W, BASE_RENDER_SIZE_H, GaussScreen, FALSE);

	// �`��u�����h���[�h���u�����h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0xFF);

	// �`�惂�[�h��߂�
	SetDrawMode(DrawModebuf);
}