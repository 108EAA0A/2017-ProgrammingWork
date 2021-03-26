#include "Battle.h"

#include "_DxLib.h"
#include "_EffekseerForDXLib.h"
#include "Camera.h"
#include "DxMath.h"
#include "NowLoading.h"
#include "Chara_Player.h"
#include "Chara_Goblin.h"
#include "MainInfo.h"
#include "StageModel.h"
#include "ShadowControl.h"

extern unsigned int KeyFrame[256];

extern drawFuncPtr StaticRenderProcess;
extern drawFuncPtr MovableRenderProcess;

extern SCameraInfo g_CamInfo;

extern Chara_Player ActionPlayer;
//static Chara_Player PrevPlayer;
extern Chara_Goblin Goblin[20];
//static Chara_Goblin PrevGoblin[20];

EnemyBase HitEnemy;

extern StageModel Stage; 

extern bool Debugflag;
static bool prevDebugflag;

std::string encountID;           //�G���J�E���g���Ɏg�p����ϐ�

static Table encount, skill;


static void DrawStaticModel(const bool Flag = false);
static void DrawMovableModel(const bool Flag = false);


Battle::Battle(IGameSceneChanger *const changer) : GameBaseScene(changer) {
}

void Battle::Initialize() {

	//PrevPlayer = Player;
	//for (int i = 0; i < 20; ++i) PrevGoblin[i] = Goblin[i];

	SetFontSize(30);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	StaticRenderProcess = DrawStaticModel;
	MovableRenderProcess = DrawMovableModel;

	encount.MakeTable(_T("resource/encount.csv"));
	skill.MakeTable(_T("resource/skill.csv"));

	SetUseASyncLoadFlag(TRUE);
	Battle::InitPreLoad();	//�񓯊��ǂݍ��݂���t�@�C�����ꊇ�ǂݍ���
	NowLoading();			//NowLoading�\������
	Battle::InitLoaded();	//�񓯊��ǂݍ��݂����t�@�C���̌㏈��
	
	encountID = "battle_1";

	mBattleType = encount[encountID.c_str()]["battle_type"];

	extern MainInfo cMainInfo;
	Player[0] = new BattleChara(cMainInfo.cPlayerData.status[0]);

	Enemy[0] = new EnemyBase(encount[encountID.c_str()]["enemyID_1"]);
	Enemy[1] = new EnemyBase(encount[encountID.c_str()]["enemyID_2"]);
	Enemy[2] = new EnemyBase(encount[encountID.c_str()]["enemyID_3"]);

	clsDx();
	printfDx("�퓬�J�n\n");
	ScreenFlip();
	WaitKey();

	mPhase = ePhase_StartProcess;

}

void Battle::Finalize() {

	clsDx();
	printfDx("�퓬�I��\n");
	ScreenFlip();
	WaitKey();

	mPhase = ePhase_EndProcess;

	mGameSceneChanger->ChangeGameScene(eGameScene_Action);

	Debugflag = prevDebugflag;

	g_CamInfo.ControllableFlag = true;

	//Player = PrevPlayer;
	//for (int i = 0; i < 20; ++i) Goblin[i] = PrevGoblin[i];
	HitEnemy.DeadProcess();
}

void Battle::Update() {
	if (KeyFrame[KEY_INPUT_ESCAPE] > 0) Battle::Finalize();

	ActionPlayer.AnimProcess();
	Goblin[0].AnimProcess();
	Goblin[0].weaponmodel.SetModelFramePosition(Goblin[0].handle, "hansocketR");

	switch (mPhase) {
		case ePhase_StartProcess:
			if (KeyFrame[KEY_INPUT_SPACE] == 1) {
				if (eventflag) mPhase = ePhase_EventProcess;
				else mPhase = ePhase_PreCommandProcess;
			}
			break;
		case ePhase_EventProcess:
			break;
		case ePhase_PreCommandProcess:
			if (KeyFrame[KEY_INPUT_SPACE] == 1) {
				mPhase = ePhase_Player_Command;
			}
			break;
		case ePhase_Player_Command:
			if (KeyFrame[KEY_INPUT_SPACE] == 1) {
				Enemy[GetRand(3)]->status.HP -= GetRand(20);
				mPhase = ePhase_Enemy_Command;
			}
			break;
		case ePhase_Enemy_Command:
			if (KeyFrame[KEY_INPUT_SPACE] == 1) {
				Player[0]->status.HP -= GetRand(10);
				mPhase = ePhase_Action;
			}
			break;
		case ePhase_Action:
			if (KeyFrame[KEY_INPUT_SPACE] == 1) {
				if (eventflag) mPhase = ePhase_EventProcess;
				else {
					mPhase = ePhase_PreCommandProcess;
					++Turn;
				}
			}

			if (Player[0]->status.HP <= 0) gameoverflag = true;
			for (int i = 0; i < ENEMY_MAX_NUM; ++i) {
				if (Enemy[i]->status.HP <= 0) winflag = true;
			}
			/*for (auto& e : Enemy) {
				if (e->status.HP <= 0) winflag = true;
			}*/
			//if (EnemyHP <= 0) winflag = true;

			if (winflag || gameoverflag || escapeflag) mPhase = ePhase_Result;
			break;
		case ePhase_Result:
			if (winflag) {
				if (KeyFrame[KEY_INPUT_SPACE] == 1) mPhase = ePhase_EndProcess;
			}
			else if (escapeflag) {
				if (KeyFrame[KEY_INPUT_SPACE] == 1) mPhase = ePhase_EndProcess;
			}
			else if (gameoverflag) {
				if (KeyFrame[KEY_INPUT_SPACE] == 1) mPhase = ePhase_EndProcess;
			}
			else mythrow("BattleResult��O");
			break;
		case ePhase_EndProcess:
			Battle::Finalize();
			break;
		default:
			mythrow("mPhase��O");
			break;
	}

	Camera_Step(1.0f / 60.0f);

}

void Battle::Draw() {

	// ��p�R�c���f����`��
	SetFogEnable(FALSE);
	Stage.Skydome.draw();
	SetFogEnable(TRUE);

	ShadowRender_Setup(ActionPlayer.pos);

	ShadowRender_DrawStart();

	DrawStaticModel();
	DrawMovableModel();

	ShadowRender_DrawEnd();


	clsDx();
	printfDx("SPACE�Ń^�[���i�s\n");
	printfDx("ESC�Ő퓬�I��\n\n");
	printfDx("���݂̃t�F�[�Y:%d\n", mPhase);
	printfDx("���݂̃^�[��:%d\n", Turn);
	
	printfDx("Player1:name:%s\n", Player[0]->status.name.c_str());
	printfDx("Player1:HP:%d\n", Player[0]->status.HP);
	printfDx("Enemy1:name:%s\n", Enemy[0]->status.name.c_str());
	printfDx("Enemy1:HP:%d\n", Enemy[0]->status.HP);
	printfDx("Enemy2:name:%s\n", Enemy[1]->status.name.c_str());
	printfDx("Enemy2:HP:%d\n", Enemy[1]->status.HP);
	printfDx("Enemy3:name:%s\n", Enemy[2]->status.name.c_str());
	printfDx("Enemy3:HP:%d\n", Enemy[2]->status.HP);

	GameBaseScene::Draw();

}

void Battle::InitPreLoad() {

	ActionPlayer.LoadModel("resource/Ammiku_snow_2014_Release/Appearance_Snow_Miku_2014.mv1");
	ActionPlayer.weaponmodel.LoadModel("resource/Ammiku_snow_2014_Release/rod.mv1");
	
	Goblin[0].LoadModel("resource/Goblin/Goblin.mv1");
	Goblin[0].LoadAnimModel("Attack", "resource/Goblin/Anim_Attack1.mv1");
	Goblin[0].LoadAnimModel("BlowIn", "resource/Goblin/Anim_Blow_In.mv1");
	Goblin[0].LoadAnimModel("Blowing", "resource/Goblin/Anim_Blow_Loop.mv1");
	Goblin[0].LoadAnimModel("BlowOut", "resource/Goblin/Anim_Blow_Out.mv1");
	Goblin[0].LoadAnimModel("Damage", "resource/Goblin/Anim_Damage.mv1");
	Goblin[0].LoadAnimModel("DownLoop", "resource/Goblin/Anim_Down_Loop.mv1");
	Goblin[0].LoadAnimModel("Neutral", "resource/Goblin/Anim_Neutral.mv1");
	Goblin[0].LoadAnimModel("Run", "resource/Goblin/Anim_Run.mv1");
	Goblin[0].LoadAnimModel("Walk", "resource/Goblin/Anim_Walk.mv1");
	Goblin[0].weaponmodel.LoadModel("resource/Goblin/Axe.mv1");
	
	Stage.LoadModel("resource/�}���G���X�e�[�W/khBG3_a_buildingonly.mv1");
	Stage.LoadObjectModel("resource/�}���G���X�e�[�W/khBG3_a_groundonly.mv1");
	Stage.LoadObjectModel("resource/�}���G���X�e�[�W/khBG3_b_deleted_underwater.mv1");
	Stage.LoadObjectModel("resource/�}���G���X�e�[�W/khBG3_c_nowater_noLotusLeaf.mv1", OBJ_DRAW_FLAG_BIT);
	Stage.LoadObjectModel("resource/�}���G���X�e�[�W/khBG3_c_wateronly.mv1", OBJ_COLL_FLAG_BIT);
	Stage.Skydome.LoadModel("resource/�}���G���X�e�[�W/khBG3_a_sky.mv1");
}

void Battle::InitLoaded() {

	ActionPlayer.SetPos(VGet(10, 20, 0));
	ActionPlayer.weaponmodel.SetModelFramePosition(ActionPlayer.handle, "�E�_�~�[");
	ActionPlayer.SetScale(1.0f);
	ActionPlayer.SetState(CharaState::Neutral);
	ActionPlayer.SetRotate(VGet(0, radf(90.0f), 0));

	Goblin[0].SetPos(VGet(-10, 20, 0));
	Goblin[0].weaponmodel.SetModelFramePosition(Goblin[0].handle, "hansocketR");
	Goblin[0].SetScale(1.0f);
	Goblin[0].SetState(CharaState::Neutral);
	Goblin[0].SetRotate(VGet(0, radf(-90.0f), 0));

	Stage.SetScale(1.0f);

	g_CamInfo.OrigPosition = VGet(-100, 0, -100);
	g_CamInfo.OrigLookAtPosition = VGet(0, 20, 0);
	g_CamInfo.ControllableFlag = false;

	//SetCameraNearFar(0.1f, 10000);
	//SetCameraPositionAndTarget_UpVecY(VGet(0, 0, -100), VGet(0, 0, 0));

	prevDebugflag = Debugflag;
	Debugflag = true;

}

static void DrawStaticModel(const bool Flag) {
	Stage.draw();
}

static void DrawMovableModel(const bool Flag) {
	ActionPlayer.draw();
	//ActionPlayer.weaponmodel.draw();

	Goblin[0].draw();
	Goblin[0].weaponmodel.draw();
}