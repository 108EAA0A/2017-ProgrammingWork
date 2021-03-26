// ���ʌ��ʊ�{

#include "Mirror.h"

#include "_DxLib.h"
#include "ConfigData.h"
#include "DxMath.h"
#include "Structure.h"
//#include "StageModel.h"

#define MIRROR_SCREEN_W		1920	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̉��𑜓x
#define MIRROR_SCREEN_H		1080	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̏c�𑜓x
//#define MIRROR_POINTNUM		256		// ���̕`��Ɏg�p����ʂ̒��_������
#define MIRROR_DEBUG_SCALE	4		// ���̃f�o�b�O�\�����Ɍ��̉����̂P�ɂ��邩�̃T�C�Y

int MirrorHandle[MIRROR_NUM];		// ���ɉf��f���̎擾�Ɏg�p����X�N���[��
static int MirrorVertexNum;
static FLOAT4 MirrorScreenPosW[MIRROR_NUM][4];	// ���ɉf��f���̎擾�Ɏg�p����N���[���̒��̋��̎l���̍��W( �������W )

extern ConfigData cConfigData;

// ���̃��[���h���W
static const VECTOR MirrorWorldPos[MIRROR_NUM][4] = {
	{
		{ -2000.0f, -5.75f,  2000.0f },
		{  2000.0f, -5.75f,  2000.0f },
		{ -2000.0f, -5.75f, -2000.0f },
		{  2000.0f, -5.75f, -2000.0f },
	},
};

// ���� Ambient Color
static const COLOR_F MirrorAmbientColor[MIRROR_NUM] = {
	//{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
} ;

// ���� Diffuse Color
static const int MirrorDiffuseColor[MIRROR_NUM][4] = {
	{ 0x7F, 0x7F, 0xFF, 0xFF },
} ;

// ���̃u�����h���[�h�ƃp�����[�^
static const int MirrorBlendParam[MIRROR_NUM][2] = {
	//{ DX_BLENDMODE_NOBLEND, 255 },
	{ DX_BLENDMODE_ALPHA,   128 },
} ;



// ���̏�����
void Mirror_Initialize() {
	if (cConfigData.ReflectionLevel == Lowest) return;

	SetCreateDrawValidGraphMultiSample(4, 4);

	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̍쐬
	for (int i = 0; i < MIRROR_NUM; ++i) {
		MirrorHandle[i] = MakeScreen(MIRROR_SCREEN_W, MIRROR_SCREEN_H, TRUE);
	}

	SetCreateDrawValidGraphMultiSample(0, 0);

	switch (cConfigData.ReflectionLevel) {
		case Lowest: return;
		case Low:		MirrorVertexNum = 64;  break;
		case Medium:	MirrorVertexNum = 100; break;
		case High:		MirrorVertexNum = 200; break;
		case Highest:	MirrorVertexNum = 256; break;
		default: mythrow("���˃��x���ݒ�l���ُ�ł�");
	}
	//MirrorVertexNum = powi(2, ReflectionLevel + 4);
}

// ���̌�n��
void Mirror_Finalize() {
	if (cConfigData.ReflectionLevel == Lowest) return;

	for (int i = 0; i < MIRROR_NUM; ++i) {
		DeleteGraph(MirrorHandle[i]);
	}
}

// ���ɉf��f����`�悷�邽�߂̃J�����̐ݒ���s��
void Mirror_SetupCamera(const int MirrorNo, const VECTOR CameraEyePos, const VECTOR CameraTargetPos) {
	if (cConfigData.ReflectionLevel == Lowest) return;
	
	float EyeLength, TargetLength;
	VECTOR MirrorNormal;
	VECTOR MirrorCameraEyePos, MirrorCameraTargetPos;

	/*extern StageModel Stage;
	VECTOR MirrorStagePos[4] = {
		MirrorWorldPos[MirrorNo][0] * Stage.scale,
		MirrorWorldPos[MirrorNo][1] * Stage.scale,
		MirrorWorldPos[MirrorNo][2] * Stage.scale,
		MirrorWorldPos[MirrorNo][3] * Stage.scale
	};*/

	// ���̖ʂ̖@�����Z�o
	//MirrorNormal = VNorm(VCross(MirrorWorldPos[MirrorNo][1] - MirrorWorldPos[MirrorNo][0], MirrorWorldPos[MirrorNo][2] - MirrorWorldPos[MirrorNo][0]));
	MirrorNormal = VGet(0.0f, 1.0f, 0.0f);

	// ���̖ʂ���J�����̍��W�܂ł̍ŒZ�����A���̖ʂ���J�����̒����_�܂ł̍ŒZ�������Z�o
	EyeLength = Plane_Point_MinLength(MirrorWorldPos[MirrorNo][0], MirrorNormal, CameraEyePos);
	TargetLength = Plane_Point_MinLength(MirrorWorldPos[MirrorNo][0], MirrorNormal, CameraTargetPos);

	// ���ɉf��f����`�悷��ۂɎg�p����J�����̍��W�ƃJ�����̒����_���Z�o
	MirrorCameraEyePos = CameraEyePos + VScale(MirrorNormal, -EyeLength * 2.0f);
	MirrorCameraTargetPos = CameraTargetPos + VScale(MirrorNormal, -TargetLength * 2.0f);

	// �v�Z�œ���ꂽ�J�����̍��W�ƃJ�����̒����_�̍��W���J�����̐ݒ肷��
	SetCameraPositionAndTarget_UpVecY(MirrorCameraEyePos, MirrorCameraTargetPos);

	// ���ɉf��f���̒��ł̋��̎l���̍��W���Z�o( �������W )
	for (int i = 0; i < 4; ++i) {
		MirrorScreenPosW[MirrorNo][i] = ConvWorldPosToScreenPosPlusW(MirrorWorldPos[MirrorNo][i]);
	}
}

// ���̕`��
void Mirror_Render(const int MirrorNo) {
	if (cConfigData.ReflectionLevel == Lowest) return;

	VERTEX3D *const Vert = new VERTEX3D[MirrorVertexNum * MirrorVertexNum];
	unsigned short *const Index = new unsigned short[(MirrorVertexNum - 1) * (MirrorVertexNum - 1) * 6];
	MATERIALPARAM Material;
	VECTOR HUnitPos;
	VECTOR VUnitPos[2];
	VECTOR HPos;
	VECTOR VPos[2];
	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	int TextureW, TextureH;
	
	// ���̕`��Ɏg�p����}�e���A���̃Z�b�g�A�b�v
	Material.Ambient  = MirrorAmbientColor[MirrorNo];
	Material.Diffuse  = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Material.Power    = 1.0f;
	SetMaterialParam(Material);

	// ���̖ʂ̖@�����Z�o
	//const VECTOR MirrorNormal = VNorm(VCross(MirrorWorldPosP[1] - MirrorWorldPosP[0], MirrorWorldPosP[2] - MirrorWorldPosP[0]));
	const VECTOR MirrorNormal = VGet(0.0f, 1.0f, 0.0f);

	// ���ɉf��f�����������񂾉摜�̃e�N�X�`���̃T�C�Y���擾
	GetGraphTextureSize(MirrorHandle[MirrorNo], &TextureW, &TextureH);

	// ���̕`��Ɏg�p���钸�_�̃Z�b�g�A�b�v
	VUnitPos[0] = VScale(MirrorWorldPos[MirrorNo][2] - MirrorWorldPos[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitPos[1] = VScale(MirrorWorldPos[MirrorNo][3] - MirrorWorldPos[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[0] = F4Scale(MirrorScreenPosW[MirrorNo][2] - MirrorScreenPosW[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[1] = F4Scale(MirrorScreenPosW[MirrorNo][3] - MirrorScreenPosW[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	const COLOR_U8 DiffuseColor = GetColorU8(MirrorDiffuseColor[MirrorNo][0], MirrorDiffuseColor[MirrorNo][1], MirrorDiffuseColor[MirrorNo][2], MirrorDiffuseColor[MirrorNo][3]);
	const COLOR_U8 SpecularColor = GetColorU8(0, 0, 0, 0);
	VPos[0] = MirrorWorldPos[MirrorNo][0];
	VPos[1] = MirrorWorldPos[MirrorNo][1];
	VUV[0] = MirrorScreenPosW[MirrorNo][0];
	VUV[1] = MirrorScreenPosW[MirrorNo][1];

	for (int i = 0, k = 0; i < MirrorVertexNum; ++i) {
		HUnitPos = VScale(VPos[1] - VPos[0], 1.0f / (MirrorVertexNum - 1));
		HPos	 = VPos[0];
		HUnitUV	 = F4Scale(VUV[1] - VUV[0], 1.0f / (MirrorVertexNum - 1));
		HUV		 = VUV[0];
		for (int j = 0; j < MirrorVertexNum; ++j, ++k) {
			Vert[k].pos	 = HPos;
			Vert[k].norm = MirrorNormal;
			Vert[k].dif	 = DiffuseColor;
			Vert[k].spc	 = SpecularColor;
			Vert[k].u	 = HUV.x / (HUV.w * TextureW);
			Vert[k].v	 = HUV.y / (HUV.w * TextureH);
			Vert[k].su	 = 0.0f;
			Vert[k].sv	 = 0.0f;
			HUV	 = HUV	+ HUnitUV;
			HPos = HPos + HUnitPos;
		}
		VUV[0]  = VUV[0]  + VUnitUV[0];
		VUV[1]  = VUV[1]  + VUnitUV[1];
		VPos[0] = VPos[0] + VUnitPos[0];
		VPos[1] = VPos[1] + VUnitPos[1];
	}

	// ���̕`��Ɏg�p���钸�_�C���f�b�N�X���Z�b�g�A�b�v
	for (int i = 0, k = 0; i < MirrorVertexNum - 1; ++i) {
		for (int j = 0; j < MirrorVertexNum - 1; ++j, k += 6) {
			Index[k + 0] = (i + 0) * MirrorVertexNum + j + 0;
			Index[k + 1] = (i + 0) * MirrorVertexNum + j + 1;
			Index[k + 2] = (i + 1) * MirrorVertexNum + j + 0;
			Index[k + 3] = (i + 1) * MirrorVertexNum + j + 1;
			Index[k + 4] = Index[k + 2];
			Index[k + 5] = Index[k + 1];
		}
	}

	const int DMbuf = GetDrawMode();
	// �`�惂�[�h���o�C���j�A�t�B���^�����O�ɐݒ�
	//SetDrawMode(DX_DRAWMODE_BILINEAR);
	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

	// �`��u�����h���[�h��ύX
	SetDrawBlendMode(MirrorBlendParam[MirrorNo][0], MirrorBlendParam[MirrorNo][1]);

	// �`��ɂy�o�b�t�@���g�p����
	SetUseZBuffer3D(TRUE);

	// �y�o�b�t�@�ɏ������݂��s��
	SetWriteZBuffer3D(TRUE);

	//SetTextureAddressMode(DX_TEXADDRESS_MIRROR);

	// ����`��
	DrawPolygonIndexed3D(Vert, MirrorVertexNum * MirrorVertexNum, Index, (MirrorVertexNum - 1) * (MirrorVertexNum - 1) * 2, MirrorHandle[MirrorNo], TRUE);

	delete[] Vert;
	delete[] Index;

	// �y�o�b�t�@�ɏ������݂��s���ݒ�����ɖ߂�
	//SetWriteZBuffer3D( FALSE ) ;

	// �y�o�b�t�@���g�p����ݒ�����ɖ߂�
	//SetUseZBuffer3D( FALSE ) ;

	// �`��u�����h���[�h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	// �`�惂�[�h�����ɖ߂�
	//SetDrawMode(DX_DRAWMODE_NEAREST);
	SetDrawMode(DMbuf);
}

// ���̃f�o�b�O�`��
void Mirror_DebugRender(const int MirrorNo, const int DrawX, const int DrawY) {
	if (cConfigData.ReflectionLevel == Lowest) return;

	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	int x, y;

	// �w��̍��W�ɋ��̉f���̎擾�Ɏg�p�����X�N���[����`��
	DrawExtendGraph(DrawX, DrawY, DrawX + MIRROR_SCREEN_W / MIRROR_DEBUG_SCALE, DrawY + MIRROR_SCREEN_H / MIRROR_DEBUG_SCALE, MirrorHandle[MirrorNo], FALSE);

	// ���̉f���̎擾�Ɏg�p�����X�N���[���̒��̋��̕����ɓ_��`��
	VUnitUV[0] = F4Scale(MirrorScreenPosW[MirrorNo][2] - MirrorScreenPosW[MirrorNo][0], 1.0f / (MirrorVertexNum - 1));
	VUnitUV[1] = F4Scale(MirrorScreenPosW[MirrorNo][3] - MirrorScreenPosW[MirrorNo][1], 1.0f / (MirrorVertexNum - 1));
	VUV[0] = MirrorScreenPosW[MirrorNo][0];
	VUV[1] = MirrorScreenPosW[MirrorNo][1];
	for (int i = 0; i < MirrorVertexNum; ++i) {
		HUnitUV = F4Scale(VUV[1] - VUV[0], 1.0f / (MirrorVertexNum - 1));
		HUV = VUV[0];
		for (int j = 0; j < MirrorVertexNum; ++j) {
			x = static_cast<int>(HUV.x / HUV.w / MIRROR_DEBUG_SCALE);
			y = static_cast<int>(HUV.y / HUV.w / MIRROR_DEBUG_SCALE);

			if( x > 0 && x < MIRROR_SCREEN_W / MIRROR_DEBUG_SCALE &&
			    y > 0 && y < MIRROR_SCREEN_H / MIRROR_DEBUG_SCALE ) {
				DrawPixel(DrawX + x, DrawY + y, DxColor::Green);
			}

			HUV = HUV + HUnitUV;
		}
		VUV[0] = VUV[0] + VUnitUV[0];
		VUV[1] = VUV[1] + VUnitUV[1];
	}
}