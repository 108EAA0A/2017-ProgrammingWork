#include "ShadowControl.h"

#include "_DxLib.h"
#include "DxMath.h"
#include "ConfigData.h"
#include "Structure.h"
#include "StageModel.h"

extern ConfigData cConfigData;

ShadowMapStr StageShadowMap;	// �͈́F�X�e�[�W�S��A�ŏ��ɂP��`�悵�Ďg���܂킵
ShadowMapStr FarShadowMap;		// �͈́F�X�e�[�W�S��A�����G�Ȃǂ̉e�p
ShadowMapStr NearShadowMap;		// �͈́F�v���C���[���ӁA�Y��ȉe�p

drawFuncPtr StaticRenderProcess;
drawFuncPtr MovableRenderProcess;

//int PointShadow;

static void StaticRenderProcessDraw(const bool Flag) {
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	StaticRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	StaticRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}
static void MovableRenderProcessDraw(const bool Flag) {
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	MovableRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	MovableRenderProcess(Flag);
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}

void ShadowRender_Initialize() {
	// �Ꮘ�����׃��[�h�ł͖����ꍇ�̓V���h�E�}�b�v��A���`�G�C���A�X��ʂ��쐬����
	if (cConfigData.ShadowLevel != Lowest) {
		const int SHADOWMAP_RESOLUTION = powi(2, cConfigData.ShadowLevel + 9);

		// �V���h�E�}�b�v�n���h�����쐬����
		StageShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (StageShadowMap.Handle == -1) mythrow("StageShadowMap�̐����Ɏ��s���܂���");
		
		FarShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (FarShadowMap.Handle == -1) mythrow("FarShadowMap�̐����Ɏ��s���܂���");

		NearShadowMap.Handle = MakeShadowMap(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
		if (NearShadowMap.Handle == -1) mythrow("NearShadowMap�̐����Ɏ��s���܂���");
	}
	/*else {
		PointShadow = LoadGraph("resource/Shadow.png");
	}*/
}

void ShadowRender_Terminate() {
	if (cConfigData.ShadowLevel != Lowest) {
		// �V���h�E�}�b�v�n���h�����폜����
		DeleteShadowMap(StageShadowMap.Handle);
		StageShadowMap.Handle = -1;

		DeleteShadowMap(FarShadowMap.Handle);
		FarShadowMap.Handle = -1;

		DeleteShadowMap(NearShadowMap.Handle);
		NearShadowMap.Handle = -1;
	}
}

void ShadowRender_StageSetup(const VECTOR LightDirection) {
	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�͉������p�V���h�E�}�b�v�ɃX�e�[�W��`�悷��
	if (cConfigData.ShadowLevel != Lowest) {
		if (StageShadowMap.Handle == -1) mythrow("StageShadowMap����������Ă��܂���");
		else if (FarShadowMap.Handle == -1) mythrow("FarShadowMap����������Ă��܂���");
		else if (NearShadowMap.Handle == -1) mythrow("NearShadowMap����������Ă��܂���");

		// �V���h�E�}�b�v�ւ̃����_�����O�Ŏg�p���郉�C�g�̕������Z�b�g
		SetShadowMapLightDirection(StageShadowMap.Handle, LightDirection);
		SetShadowMapLightDirection(FarShadowMap.Handle, LightDirection);
		SetShadowMapLightDirection(NearShadowMap.Handle, LightDirection);

		// �X�e�[�W���f���p�Ɖ������p�̃V���h�E�}�b�v�ɂ̓X�e�[�W�S�̂����߂�
		{
			MV1_REF_POLYGONLIST PolyList;
			extern StageModel Stage;
			
			PolyList = MV1GetReferenceMesh(Stage.handle, -1, FALSE);
			SetShadowMapDrawArea(StageShadowMap.Handle, PolyList.MinPosition * Stage.scale.x, PolyList.MaxPosition * Stage.scale.x);
			SetShadowMapDrawArea(FarShadowMap.Handle, PolyList.MinPosition * Stage.scale.x, PolyList.MaxPosition * Stage.scale.x);
		}

		// �X�e�[�W���f���p�̃V���h�E�}�b�v�ɃX�e�[�W�������_�����O
		ShadowMap_DrawSetup(StageShadowMap.Handle);
		StaticRenderProcessDraw(false);
		ShadowMap_DrawEnd();
	}
}

void ShadowRender_Setup(const VECTOR CenterPos) {
	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v�ւ̕`����s��
	if (cConfigData.ShadowLevel != Lowest) {
		// �������p�̃V���h�E�}�b�v�ɃL�����N�^�[�������_�����O
		ShadowMap_DrawSetup(FarShadowMap.Handle);
		MovableRenderProcessDraw(false);
		ShadowMap_DrawEnd();

		// �ߋ����p�̃V���h�E�}�b�v�̕`��͈͂��Z�o
		static const float NearDistance = 150.0f;
		NearShadowMap.MinPos = CenterPos - VGet(NearDistance, 0.2f, NearDistance);
		NearShadowMap.MaxPos = CenterPos + VGet(NearDistance, 5.0f, NearDistance);
		// �ߋ����p�̃V���h�E�}�b�v�̕`��͈͂��Z�b�g
		SetShadowMapDrawArea(NearShadowMap.Handle, NearShadowMap.MinPos, NearShadowMap.MaxPos);

		// �ߋ����p�̃V���h�E�}�b�v�ɃL�����N�^�[�������_�����O
		ShadowMap_DrawSetup(NearShadowMap.Handle);
		MovableRenderProcessDraw(true);
		ShadowMap_DrawEnd();
	}
}

void ShadowRender_DrawStart() {
	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v���g�p����
	if (cConfigData.ShadowLevel != Lowest) {
		SetUseShadowMap(0, StageShadowMap.Handle);
		SetUseShadowMap(1, FarShadowMap.Handle);
		SetUseShadowMap(2, NearShadowMap.Handle);
	}
	// �Ꮘ�����׃��[�h�̏ꍇ�͊ۉe��`�悷��
	/*if (ShadowLevel == Lowest) {
	PointShadowRender(ActionPlayer.pos, ActionPlayer.collCap);
	for (int i = 0; i < GoblinNum; ++i) {
	if (!(Goblin[i].deadflag && Goblin[i].state == CharaState::DownLoop)) PointShadowRender(Goblin[i].pos, Goblin[i].collCap);
	}
	}*/
}

void ShadowRender_DrawEnd() {
	// �Ꮘ�����׃��[�h�ł͂Ȃ��ꍇ�̓V���h�E�}�b�v�̎g�p������
	if (cConfigData.ShadowLevel != Lowest) {
		SetUseShadowMap(0, -1);
		SetUseShadowMap(1, -1);
		SetUseShadowMap(2, -1);
	}
}

void ShadowRender_DrawDebug() {
	if (cConfigData.ShadowLevel != Lowest) {
		TestDrawShadowMap(NearShadowMap.Handle, 1440, 810, 1920, 1080);
	}
}

/*void PointShadowRender(const VECTOR Pos, const Capsule& Cap) {
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	extern StageModel Stage;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	//SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitResDim = Stage.CollCheck_Capsule(Cap);
	
	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	HitRes = HitResDim.Dim;
	for (int i = 0; i < HitResDim.HitNum; ++i, ++HitRes) {
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		SlideVec = HitRes->Normal * 0.005f;
		Vertex[0].pos = Vertex[0].pos + SlideVec;
		Vertex[1].pos = Vertex[1].pos + SlideVec;
		Vertex[2].pos = Vertex[2].pos + SlideVec;

		// �|���S���̕s�����x��ݒ肷��
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[0].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[0].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		if (HitRes->Position[1].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[1].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[1].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		if (HitRes->Position[2].y > Pos.y - (Cap.posU.y - Cap.posD.y))
			Vertex[2].dif.a = (BYTE)(128.0f * (1.0f - fabs(HitRes->Position[2].y - Pos.y) / (Cap.posU.y - Cap.posD.y)));

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		Vertex[0].u = (HitRes->Position[0].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - Pos.x) / (Cap.r * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - Pos.z) / (Cap.r * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(Vertex, 1, PointShadow, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(HitResDim);

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	// �y�o�b�t�@�𖳌��ɂ���
	//SetUseZBuffer3D(FALSE);

	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
}*/