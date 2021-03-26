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

/* ここから変数宣言 */
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

/* ここまで変数宣言 */

//初期化
void Action::Initialize() {
	SetFontSize(30);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	// 影処理の初期化
	ShadowRender_Initialize();

	// 鏡処理の初期化
	Mirror_Initialize();

	// ブルーム効果の初期化
	Bloom_Init();

	extern drawFuncPtr StaticRenderProcess, MovableRenderProcess;
	StaticRenderProcess = &DrawModelStatic;
	MovableRenderProcess = &DrawModelMovable;

	SetUseASyncLoadFlag(TRUE);
	InitPreLoad();	//非同期読み込みするファイルを一括読み込み
	NowLoading();	//NowLoading表示処理
	InitLoaded();	//非同期読み込みしたファイルの後処理

	MenuFlag = false;
	BloomFlag = false;

	efLaserBase = new EffectBase("resource/Effect/Laser.efk");
	efLaser = Effect(*efLaserBase);

	ActionBGM->play(true);
}

//終了処理
void Action::Finalize() {
	delete ActionBGM;
	for (int i = 0; i < 4; ++i) {
		delete SE_Walk[i];
	}
	delete SE_Jump, SE_Land;

	delete efLaserBase;

	// 影処理の後始末
	ShadowRender_Terminate();

	// 鏡処理の後始末
	Mirror_Finalize();

	// ブルーム効果の後始末
	Bloom_Finalize();
}

//更新
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
				mythrow("メニュー画面選択例外");
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

	ActionPlayer.weaponmodel.SetModelFramePosition(ActionPlayer.handle, "右中指１");

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

	// DXライブラリのカメラとEffekseerのカメラを同期する。
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

		// 武器モデルを右腕の指の場所に設置する
		Goblin[i].weaponmodel.SetModelFramePosition(Goblin[i].handle, "hansocketR");
	}

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

}

//描画
void Action::Draw() {
	
	ShadowRender_Setup(ActionPlayer.pos);

	// 空用３Ｄモデルを描画
	SetFogEnable(FALSE);
	Stage.Skydome.draw();
	SetFogEnable(TRUE);

	ShadowRender_DrawStart();

	extern ConfigData cConfigData;
	if (cConfigData.ReflectionLevel != Lowest) {
		const int DSbuf = GetDrawScreen();
		// 鏡に映る映像を描画
		for (int i = 0; i < MIRROR_NUM; ++i) {
			//extern SCameraInfo g_CamInfo;
			extern int MirrorHandle[MIRROR_NUM];

			// 描画先を鏡に映る映像の取得に使用する画像を描画するスクリーンにする
			SetDrawScreen(MirrorHandle[i]);

			// 画面を初期化する
			ClearDrawScreen();

			Camera_Setup();

			// 鏡に映る映像を描画する際に使用するカメラの設定を行う
			Mirror_SetupCamera(i, GetCameraPosition(), GetCameraTarget());

			// ３Ｄ空間の描画
			DrawModelAll();
		}
		SetDrawScreen(DSbuf);
		Camera_Setup();
	}

	DrawModelAll(true);

	// 鏡の描画
	for (int i = 0; i < MIRROR_NUM; ++i) {
		Mirror_Render(i);
	}

	ShadowRender_DrawEnd();

	// 何でもいいので画像を描画する。
	// こうして描画した後でないと、Effekseerは描画できない。
	DrawGraph(0, 0, efBackHandle, TRUE);

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	// ブルーム効果
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

		// 画面左下に鏡のデバッグ表示
		for (int i = 0; i < MIRROR_NUM; ++i) {
			Mirror_DebugRender(i, 480 * i, 810);
		}

		//ShadowRender_DrawDebug();
	}
#endif

	// ブルーム効果
	if (BloomFlag) Bloom_Draw();

	if (MenuFlag) {
		SetDrawZBuffer(FALSE);
		SetWriteZBufferFlag(FALSE);

		DrawBox(192, 108, 1728, 972, DxColor::Blue, TRUE);
		ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
		DrawString(384, 216, "アイテム", DxColor::White, DxColor::Black);
		DrawString(384, 324, "ステータス", DxColor::White, DxColor::Black);
		DrawString(384, 540, "戻る", DxColor::White, DxColor::Black);
		DrawString(384, 864, "ゲームを終了", DxColor::White, DxColor::Black);
		switch (vMenu) {
			case eMenu_Item:	MenuCursor_Y = 216; break;
			case eMenu_Status:	MenuCursor_Y = 324; break;
			case eMenu_Back:	MenuCursor_Y = 540; break;
			case eMenu_Exit:	MenuCursor_Y = 864; break;
		}
		DrawString(320, MenuCursor_Y, "■", DxColor::White, DxColor::Black);

		SetDrawZBuffer(TRUE);
		SetWriteZBufferFlag(TRUE);
	}

	clsDx();
#if _DEBUG
	if (Debugflag) DebugMode();
#endif

}

void Action::InitPreLoad() {
	/* 以下非同期読み込みするデータ */

	// ここからミク関係
	ActionPlayer.LoadModel("resource/Model/Appearance Miku/Appearance Miku.mv1");
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	ActionPlayer.weaponmodel.LoadModel("resource/Model/Rod/Rod.mv1");
	// ここまでミク関係

	// ここからステージ関係
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	//Stage.LoadModel("resource/マリエルステージ/khBG3_a_unsky_nounderwater.mv1");
	Stage.LoadModel("resource/Model/マリエルステージ/khBG3_a_buildingonly.mv1");
	Stage.LoadObjectModel("resource/Model/マリエルステージ/khBG3_a_groundonly.mv1");
	//Stage.LoadObjectModel("resource/マリエルステージ/khBG3_a_waterbottomonly.mv1");
	Stage.LoadObjectModel("resource/Model/マリエルステージ/khBG3_b_deleted_underwater.mv1");
	Stage.LoadObjectModel("resource/Model/マリエルステージ/khBG3_c_nowater_noLotusLeaf.mv1", OBJ_DRAW_FLAG_BIT);
	Stage.LoadObjectModel("resource/Model/マリエルステージ/khBG3_c_wateronly.mv1", OBJ_COLL_FLAG_BIT);
	
	Stage.Skydome.LoadModel("resource/Model/マリエルステージ/khBG3_a_sky.mv1");
	// ここまでステージ関係

	// ここから敵関係
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
	// ここまで敵関係

	// ここから音関係
	ActionBGM = new SoundAsset("resource/Sound/BGM/bgm_maoudamashii_healing08.ogg", SoundType::BGM);

	SE_Walk[0] = new SoundAsset("resource/Sound/SE/Footstep01.wav", SoundType::SE);
	SE_Walk[1] = new SoundAsset("resource/Sound/SE/Footstep02.wav", SoundType::SE);
	SE_Walk[2] = new SoundAsset("resource/Sound/SE/Footstep03.wav", SoundType::SE);
	SE_Walk[3] = new SoundAsset("resource/Sound/SE/Footstep04.wav", SoundType::SE);
	SE_Jump = new SoundAsset("resource/Sound/SE/Jump.wav", SoundType::SE);
	SE_Land = new SoundAsset("resource/Sound/SE/Land.wav", SoundType::SE);
	// ここまで音関係

	/*ToonRenderingVertex = LoadVertexShader(_T("Shader/ToonRenderingVertex.vso"));
	ToonRenderingPixel = LoadPixelShader(_T("Shader/ToonRenderingPixel.pso"));
	MV1SetUseOrigShader(TRUE);*/

	/* ここまで非同期読み込みするデータ */
}

void Action::InitLoaded() {
	const float PCwalkspeed = 0.35f;
	const float PCdashspeed = 1.00f;
	
	//ここからミク関係
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

	//ここまでミク関係
	
	Stage.SetScale(2.0f);
	Stage.SetupCollInfo();
	
	Stage.Skydome.SetScale(Stage.scale);

	//ここからカメラ関係
	Camera_Initialize();

	SetFogEnable(TRUE);
	SetFogStartEnd(1000.0f, 5000.0f);
	//ここまでカメラ関係

	//ここから光源、影関係
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

		Goblin[i].NowAnim.time = static_cast<float>(GetRand(6000)) / 100.0f; // 再生時間の初期化

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

			// 武器の描画
			MV1SetOpacityRate(Goblin[i].weaponmodel.handle, MV1GetOpacityRate(Goblin[i].handle));
			Goblin[i].weaponmodel.draw();
		}
	}

}

void DrawModelAll(const bool drawcollobj) {
	// 半透明要素の無い部分を描画
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY);
	DrawModelStatic(drawcollobj);
	DrawModelMovable(true);

	// 半透明要素のある部分を描画
	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY);
	DrawModelStatic(drawcollobj);
	DrawModelMovable(true);

	MV1SetSemiTransDrawMode(DX_SEMITRANSDRAWMODE_ALWAYS);
}
