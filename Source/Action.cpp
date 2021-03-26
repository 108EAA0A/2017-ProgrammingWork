#include "Action.h"

#include <math.h>
#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "BloomControl.h"
#include "Camera.h"
#include "Chara_Player.h"
#include "Chara_Goblin.h"
#include "ConfigData.h"
#include "DebugTools.h"
#include "DxMath.h"
#include "Effect.h"
#include "Mirror.h"
#include "NowLoading.h"
#include "ShadowControl.h"
#include "SoundAsset.h"
#include "StageModel.h"
#include "Structure.h"

Action::Action(ISceneChanger *const changer) : BaseScene(changer) {
}

/* ��������ϐ��錾 */
extern unsigned int KeyFrame[256];

static bool Debugflag;

Chara_Player ActionPlayer;

StageModel Stage;

static SoundAsset *ActionBGM;
static SoundAsset *SE_Walk[4];
static SoundAsset *SE_Jump, *SE_Land;

static VECTOR DirLightDirection;

//static int ToonRenderingVertex;
//static int ToonRenderingPixel;

static int efBackHandle;

static EffectBase *efLaserBase;
static Effect efLaser;

static const int GoblinNum = 20;
Chara_Goblin Goblin[GoblinNum];
static const VECTOR GoblinCovorBox = { 7.0f,16.5f,7.0f };

static bool MenuFlag;
static bool BloomFlag;

enum eMenu {
	eMenu_Item,
	eMenu_Status,
	//eMenu_System,
	eMenu_Back,
	eMenu_Exit,

	eMenu_Num,
};
static int vMenu = eMenu_Item;
static int MenuCursor_Y;

/* �����܂ŕϐ��錾 */

//������
void Action::Initialize() {
	SetFontSize(30);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	// �e�����̏�����
	ShadowRender_Initialize();

	// �������̏�����
	Mirror_Initialize();

	// �u���[�����ʂ̏�����
	Bloom_Init();

	extern drawFuncPtr StaticRenderProcess, MovableRenderProcess;
	StaticRenderProcess = &DrawModelStatic;
	MovableRenderProcess = &DrawModelMovable;

	SetUseASyncLoadFlag(TRUE);
	InitPreLoad();	//�񓯊��ǂݍ��݂���t�@�C�����ꊇ�ǂݍ���
	NowLoading();	//NowLoading�\������
	InitLoaded();	//�񓯊��ǂݍ��݂����t�@�C���̌㏈��

	MenuFlag = false;
	BloomFlag = false;

	efLaserBase = new EffectBase("resource/Effect/Laser.efk");
	efLaser = Effect(*efLaserBase);

	ActionBGM->play(true);
}

//�I������
void Action::Finalize() {
	delete ActionBGM;
	for (int i = 0; i < 4; ++i) {
		delete SE_Walk[i];
	}
	delete SE_Jump, SE_Land;

	delete efLaserBase;

	// �e�����̌�n��
	ShadowRender_Terminate();

	// �������̌�n��
	Mirror_Finalize();

	// �u���[�����ʂ̌�n��
	Bloom_Finalize();
}

//�X�V
void Action::Update() {
	if (KeyFrame[KEY_INPUT_ESCAPE] == 1) MenuFlag = !MenuFlag;
	
	if (MenuFlag) {

		//Camera_Step(1.0f);

		if (KeyFrame[KEY_INPUT_DOWN] == 1) {
			vMenu = (vMenu + 1) % eMenu_Num;
		}
		if (KeyFrame[KEY_INPUT_UP] == 1) {
			vMenu = (vMenu + (eMenu_Num - 1)) % eMenu_Num;
		}
		if (KeyFrame[KEY_INPUT_RETURN] == 1) {
			switch (vMenu) {
			case eMenu_Item:
				break;
			case eMenu_Status:
				break;
			case eMenu_Back:
				MenuFlag = false;
				break;
			case eMenu_Exit:
				FORCEEND;
			default:
				mythrow("���j���[��ʑI���O");
				break;
			}
		}

		return;
	}
	
	//Camera_Setup();

	ActionPlayer.PlayerControl();

	if (Debugflag && KeyFrame[KEY_INPUT_LCONTROL] > 0) {
		ActionPlayer.speed.x *= 10.0f;
		ActionPlayer.speed.z *= 10.0f;
	}
	
	ActionPlayer.AnimProcess();
	ActionPlayer.MoveProcess();
	ActionPlayer.UpdateCollCap();
	ActionPlayer.JumpControl();
	ActionPlayer.UpdateCollCap();
	ActionPlayer.RotateProcess();

	ActionPlayer.weaponmodel.SetModelFramePosition(ActionPlayer.handle, "�E���w�P");

	if (KeyFrame[KEY_INPUT_F3] == 1) Debugflag = !Debugflag;
	if (KeyFrame[KEY_INPUT_F4] == 1) BloomFlag = !BloomFlag;

	Camera_Step(6.0f / 60.0f);
	
	if (ActionPlayer.state == CharaState::Walk) {
		int cnt = static_cast<int>(ActionPlayer.NowAnim.time) % 20;
		if (cnt == 0) SE_Walk[cnt % sizeof(*SE_Walk)]->play();
	}
	else if (ActionPlayer.state == CharaState::Run) {
		int cnt = static_cast<int>(ActionPlayer.NowAnim.time) % 12;
		if (cnt == 0) SE_Walk[cnt % sizeof(*SE_Walk)]->play();
	}

	if (ActionPlayer.state == CharaState::Jump && ActionPlayer.NowAnim.time == 0.0f) {
		SE_Jump->play();
	}
	if (ActionPlayer.state == CharaState::Landing && ActionPlayer.NowAnim.time == 0.0f) {
		SE_Land->play();
	}

	// DX���C�u�����̃J������Effekseer�̃J�����𓯊�����B
	Effekseer_Sync3DSetting();

	if ((KeyFrame[KEY_INPUT_E] == 1) && IsEffekseer3DEffectPlaying(efLaser.playinghandle) == -1) {
		efLaser.Play();
	}
	if (IsEffekseer3DEffectPlaying(efLaser.playinghandle) != -1) {
		efLaser.SetPos(ActionPlayer.pos.cYadd(ActionPlayer.collCap.height));
		efLaser.SetRotate(ActionPlayer.rotate.cYadd(radf(90.0f)));
		efLaser.SetScale(3.0f);
	}

	for (int i = 0; i < GoblinNum; ++i) {
		
		if (Goblin[i].deadflag) {
			Goblin[i].DeadProcess();
			continue;
		}

		if (Goblin[i].state == CharaState::Damage) {
			if (Goblin[i].NowAnim.time >= Goblin[i].NowAnim.totaltime) {
				Goblin[i].SetState(CharaState::Neutral);
			}
		}

		if (VSize(ActionPlayer.pos - Goblin[i].pos) <= Goblin[i].visibility) {
			Goblin[i].FoundPl = true;
			if (Goblin[i].state == CharaState::Neutral) {
				Goblin[i].SetState(CharaState::Run);
			}
		}
		else {
			Goblin[i].FoundPl = false;
			if (Goblin[i].state == CharaState::Run) {
				Goblin[i].SetSpeed(VGet(0.0f, 0.0f, 0.0f));

				Goblin[i].SetState(CharaState::Neutral);
			}
		}

		if (Goblin[i].FoundPl && (CharaState::Neutral == Goblin[i].state || Goblin[i].state == CharaState::Run)) {
			Goblin[i].SetRotate(VGet(0, radf(180.0f) + atan2f(ActionPlayer.pos.x - Goblin[i].pos.x, ActionPlayer.pos.z - Goblin[i].pos.z), 0));
		}

		if (Goblin[i].state == CharaState::Walk) {
			Goblin[i].speed.x = -Goblin[i].walkspeed*sinf(Goblin[i].rotate.y);
			Goblin[i].speed.z = -Goblin[i].walkspeed*cosf(Goblin[i].rotate.y);
		}
		if (Goblin[i].state == CharaState::Run) {
			Goblin[i].speed.x = -Goblin[i].dashspeed*sinf(Goblin[i].rotate.y);
			Goblin[i].speed.z = -Goblin[i].dashspeed*cosf(Goblin[i].rotate.y);
		}

		Goblin[i].MoveProcess();
		Goblin[i].AnimProcess();

		Goblin[i].UpdateCollCap();
		Goblin[i].UpdateCoverbox();

		// ���탂�f�����E�r�̎w�̏ꏊ�ɐݒu����
		Goblin[i].weaponmodel.SetModelFramePosition(Goblin[i].handle, "hansocketR");
	}

	// Effekseer�ɂ��Đ����̃G�t�F�N�g���X�V����B
	UpdateEffekseer3D();

}

//�`��
void Action::Draw() {
	
	ShadowRender_Setup(ActionPlayer.pos);

	// ��p�R�c���f����`��
	SetFogEnable(FALSE);
	Stage.Skydome.draw();
	SetFogEnable(TRUE);

	ShadowRender_DrawStart();

	extern ConfigData cConfigData;
	if (cConfigData.ReflectionLevel != Lowest) {
		const int DSbuf = GetDrawScreen();
		// ���ɉf��f����`��
		for (int i = 0; i < MIRROR_NUM; ++i) {
			//extern SCameraInfo g_CamInfo;
			extern int MirrorHandle[MIRROR_NUM];

			// �`�������ɉf��f���̎擾�Ɏg�p����摜��`�悷��X�N���[���ɂ���
			SetDrawScreen(MirrorHandle[i]);

			// ��ʂ�����������
			ClearDrawScreen();

			Camera_Setup();

			// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��
			Mirror_SetupCamera(i, GetCameraPosition(), GetCameraTarget());

			// �R�c��Ԃ̕`��
			DrawModelAll();
		}
		SetDrawScreen(DSbuf);
		Camera_Setup();
	}

	DrawModelAll(true);

	// ���̕`��
	for (int i = 0; i < MIRROR_NUM; ++i) {
		Mirror_Render(i);
	}

	ShadowRender_DrawEnd();

	// ���ł������̂ŉ摜��`�悷��B
	// �������ĕ`�悵����łȂ��ƁAEffekseer�͕`��ł��Ȃ��B
	DrawGraph(0, 0, efBackHandle, TRUE);

	// Effekseer�ɂ��Đ����̃G�t�F�N�g��`�悷��B
	DrawEffekseer3D();

	// �u���[������
	if (BloomFlag) Bloom_Setup();

#ifdef _DEBUG
	if (Debugflag) {
		ActionPlayer.drawCollCap();
		for (int i = 0; i < GoblinNum; ++i) {
			if (!Goblin[i].deadflag) {
				Goblin[i].drawCollCap();
				Goblin[i].drawCoverBox();
				VECTOR NamePlatePos = ConvWorldPosToScreenPos(Goblin[i].pos);
				if (0.0f < NamePlatePos.z && NamePlatePos.z < 1.0f) DrawFormatStringF(NamePlatePos.x, NamePlatePos.y, DxColor::White, "%d", i);
			}
		}

		// ��ʍ����ɋ��̃f�o�b�O�\��
		for (int i = 0; i < MIRROR_NUM; ++i) {
			Mirror_DebugRender(i, 480 * i, 810);
		}

		//ShadowRender_DrawDebug();
	}
#endif

	// �u���[������
	if (BloomFlag) Bloom_Draw();

	if (MenuFlag) {
		SetDrawZBuffer(FALSE);
		SetWriteZBufferFlag(FALSE);

		DrawBox(192, 108, 1728, 972, DxColor::Blue, TRUE);
		ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
		DrawString(384, 216, "�A�C�e��", DxColor::White, DxColor::Black);
		DrawString(384, 324, "�X�e�[�^�X", DxColor::White, DxColor::Black);
		DrawString(384, 540, "�߂�", DxColor::White, DxColor::Black);
		DrawString(384, 864, "�Q�[�����I��", DxColor::White, DxColor::Black);
		switch (vMenu) {
			case eMenu_Item:	MenuCursor_Y = 216; break;
			case eMenu_Status:	MenuCursor_Y = 324; break;
			case eMenu_Back:	MenuCursor_Y = 540; break;
			case eMenu_Exit:	MenuCursor_Y = 864; break;
		}
		DrawString(320, MenuCursor_Y, "��", DxColor::White, DxColor::Black);

		SetDrawZBuffer(TRUE);
		SetWriteZBufferFlag(TRUE);
	}

	clsDx();
#if _DEBUG
	if (Debugflag) DebugMode();
#endif

}

void Action::InitPreLoad() {
	/* �ȉ��񓯊��ǂݍ��݂���f�[�^ */

	// ��������~�N�֌W
	ActionPlayer.LoadModel("resource/Model/Appearance Miku/Appearance Miku.mv1");
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	ActionPlayer.weaponmodel.LoadModel("resource/Model/Rod/Rod.mv1");
	// �����܂Ń~�N�֌W

	// ��������X�e�[�W�֌W
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	//Stage.LoadModel("resource/�}���G���X�e�[�W/khBG3_a_unsky_nounderwater.mv1");
	Stage.LoadModel("resource/Model/�}���G���X�e�[�W/khBG3_a_buildingonly.mv1");
	Stage.LoadObjectModel("resource/Model/�}���G���X�e�[�W/khBG3_a_groundonly.mv1");
	//Stage.LoadObjectModel("resource/�}���G���X�e�[�W/khBG3_a_waterbottomonly.mv1");
	Stage.LoadObjectModel("resource/Model/�}���G���X�e�[�W/khBG3_b_deleted_underwater.mv1");
	Stage.LoadObjectModel("resource/Model/�}���G���X�e�[�W/khBG3_c_nowater_noLotusLeaf.mv1", OBJ_DRAW_FLAG_BIT);
	Stage.LoadObjectModel("resource/Model/�}���G���X�e�[�W/khBG3_c_wateronly.mv1", OBJ_COLL_FLAG_BIT);
	
	Stage.Skydome.LoadModel("resource/Model/�}���G���X�e�[�W/khBG3_a_sky.mv1");
	// �����܂ŃX�e�[�W�֌W

	// ��������G�֌W
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
	Goblin[0].LoadModel("resource/Model/Goblin/Goblin.mv1");
	Goblin[0].LoadAnimModel("Attack", "resource/Model/Goblin/Anim_Attack1.mv1");
	Goblin[0].LoadAnimModel("BlowIn", "resource/Model/Goblin/Anim_Blow_In.mv1");
	Goblin[0].LoadAnimModel("Blowing", "resource/Model/Goblin/Anim_Blow_Loop.mv1");
	Goblin[0].LoadAnimModel("BlowOut", "resource/Model/Goblin/Anim_Blow_Out.mv1");
	Goblin[0].LoadAnimModel("Damage", "resource/Model/Goblin/Anim_Damage.mv1");
	Goblin[0].LoadAnimModel("DownLoop", "resource/Model/Goblin/Anim_Down_Loop.mv1");
	Goblin[0].LoadAnimModel("Neutral", "resource/Model/Goblin/Anim_Neutral.mv1");
	Goblin[0].LoadAnimModel("Run", "resource/Model/Goblin/Anim_Run.mv1");
	Goblin[0].LoadAnimModel("Walk", "resource/Model/Goblin/Anim_Walk.mv1");

	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	Goblin[0].weaponmodel.LoadModel("resource/Model/Goblin/Axe.mv1");
	// �����܂œG�֌W

	// �������特�֌W
	ActionBGM = new SoundAsset("resource/Sound/BGM/bgm_maoudamashii_healing08.ogg", SoundType::BGM);

	SE_Walk[0] = new SoundAsset("resource/Sound/SE/Footstep01.wav", SoundType::SE);
	SE_Walk[1] = new SoundAsset("resource/Sound/SE/Footstep02.wav", SoundType::SE);
	SE_Walk[2] = new SoundAsset("resource/Sound/SE/Footstep03.wav", SoundType::SE);
	SE_Walk[3] = new SoundAsset("resource/Sound/SE/Footstep04.wav", SoundType::SE);
	SE_Jump = new SoundAsset("resource/Sound/SE/Jump.wav", SoundType::SE);
	SE_Land = new SoundAsset("resource/Sound/SE/Land.wav", SoundType::SE);
	// �����܂ŉ��֌W

	/*ToonRenderingVertex = LoadVertexShader(_T("Shader/ToonRenderingVertex.vso"));
	ToonRenderingPixel = LoadPixelShader(_T("Shader/ToonRenderingPixel.pso"));
	MV1SetUseOrigShader(TRUE);*/

	/* �����܂Ŕ񓯊��ǂݍ��݂���f�[�^ */
}

void Action::InitLoaded() {
	const float PCwalkspeed = 0.35f;
	const float PCdashspeed = 1.00f;
	
	//��������~�N�֌W
	ActionPlayer.respawnpos = VGet(0.0f, 32.0f, 0.0f);
	ActionPlayer.SetPos(ActionPlayer.respawnpos)
				.SetRotate(VGet(0.0f, 0.0f, 0.0f))
				.SetScale(1.0f)
				.OutLineResize(ActionPlayer.scale.y*(10.0f / 2.5f));
	ActionPlayer.AdjustAnimPos = 8.0f*ActionPlayer.scale.y;
	ActionPlayer.SetState(CharaState::Neutral);
	ActionPlayer.walkspeed = PCwalkspeed * ActionPlayer.scale.y;
	ActionPlayer.dashspeed = PCdashspeed * ActionPlayer.scale.y;
	ActionPlayer.SetSpeed(VGet(0.0f, 0.0f, 0.0f));
	ActionPlayer.SetJumppow(ActionPlayer.scale.y*2.0f);
	ActionPlayer.getoverheight = ActionPlayer.scale.y*7.5f;

	ActionPlayer.SetCollCap(ActionPlayer.scale.y*12.0f, ActionPlayer.scale.x*4.0f);
	//Player.controllableFlag = true;

	ActionPlayer.weaponmodel.SetScale(ActionPlayer.scale*0.0125f);
	ActionPlayer.weaponmodel.SetPos(VGet(0.0f, 0.0f, 8.0f));
	ActionPlayer.weaponmodel.SetRotate(VGet(radf(-90.0f), 0.0f, 0.0f));

	//Player.ToonFlag(true);

	//�����܂Ń~�N�֌W
	
	Stage.SetScale(2.0f);
	Stage.SetupCollInfo();
	
	Stage.Skydome.SetScale(Stage.scale);

	//��������J�����֌W
	Camera_Initialize();

	SetFogEnable(TRUE);
	SetFogStartEnd(1000.0f, 5000.0f);
	//�����܂ŃJ�����֌W

	//������������A�e�֌W
	//SetUseLighting(FALSE);
	DirLightDirection = VGet(-1.0f, -1.0f, -1.0f);
	ChangeLightTypeDir(DirLightDirection);

	const float Goblinwalkspeed = 0.25f;
	const float Goblindashspeed = 0.75f;
	for (int i = 0; i < GoblinNum; ++i) {
		if (Goblin[i].deadflag) continue;

		if (i != 0) {
			Goblin[i].DupModel(Goblin[0]);
			Goblin[i].DupAnimModel(Goblin[0].animhandle);
			Goblin[i].weaponmodel.DupModel(Goblin[0].weaponmodel);
		}

		Goblin[i].SetScale(1.0f);

		Goblin[i].SetState(CharaState::Neutral);

		Goblin[i].NowAnim.time = static_cast<float>(GetRand(6000)) / 100.0f; // �Đ����Ԃ̏�����

		Goblin[i].visibility = 50.0f;

		do {
			Goblin[i].pos.x = Goblin[i].scale.y * (250 - GetRand(500));
			Goblin[i].pos.z = Goblin[i].scale.y * (250 - GetRand(500));
			Goblin[i].SetPos(Goblin[i].pos);
		} while (Goblin[i].visibility*2.0f >= Segment_Point_MinLength(ActionPlayer.pos, ActionPlayer.pos, Goblin[i].pos));

		Goblin[i].SetRotate(VGet(0.0f, radf(static_cast<float>(GetRand(360))), 0.0f));

		Goblin[i].walkspeed = Goblinwalkspeed * Goblin[i].scale.y;
		Goblin[i].dashspeed = Goblindashspeed * Goblin[i].scale.y;
		Goblin[i].getoverheight = Goblin[i].scale.y*4.0f;

		Goblin[i].SetCollCap(2.5f * Goblin[i].scale.y, 6.5f * Goblin[i].scale.y);

		Goblin[i].SetCoverbox(GoblinCovorBox);

		MV1SetFrameUserLocalMatrix(Goblin[i].handle, MV1SearchFrame(Goblin[i].handle, "root"), MGetIdent());

		Goblin[i].weaponmodel.SetScale(Goblin[i].scale);

		Goblin[i].weaponmodel.SetModelFramePosition(Goblin[i].handle, "hansocketR");
	}

	ShadowRender_StageSetup(DirLightDirection);

}

void DrawModelStatic(const bool drawcollobj) {
	if (!drawcollobj) Stage.draw();
	else Stage.drawall();
}

void DrawModelMovable(const bool IsNearFlag) {
	extern ShadowMapStr NearShadowMap;

	if (IsNearFlag) {
		ActionPlayer.draw();
		ActionPlayer.weaponmodel.draw();
	}

	for (int i = 0; i < GoblinNum; ++i) {
		//if (!IsNearFlag) continue;
		if (!IsNearFlag) {
			if (Goblin[i].pos.x <= fabs(NearShadowMap.MaxPos.x) &&
				Goblin[i].pos.y <= fabs(NearShadowMap.MaxPos.y) &&
				Goblin[i].pos.z <= fabs(NearShadowMap.MaxPos.z))	continue;
		}

		if (!CheckCameraViewClip_Box(Goblin[i].coverbox.Pos1, Goblin[i].coverbox.Pos2) && (MV1GetOpacityRate(Goblin[i].handle) > 0.0f)) {
			Goblin[i].draw();

			// ����̕`��
			MV1SetOpacityRate(Goblin[i].weaponmodel.handle, MV1GetOpacityRate(Goblin[i].handle));
			Goblin[i].weaponmodel.draw();
		}
	}

}

void DrawModelAll(const bool drawcollobj) {
	// �������v�f�̖���������`��
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	DrawModelStatic(drawcollobj);
	DrawModelMovable(true);

	// �������v�f�̂��镔����`��
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	DrawModelStatic(drawcollobj);
	DrawModelMovable(true);

	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}
