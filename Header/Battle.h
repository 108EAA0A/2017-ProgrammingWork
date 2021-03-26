#pragma once

#include "GameBaseScene.h"

#include "Table.h"
#include "BattleChara.h"
#include "PlayerData.h"

#define ENEMY_MAX_NUM 4

class Battle : public GameBaseScene {

private:
	enum eBattleType {
		eBattleType_Tutorial,
		eBattleType_Normal,
		eBattleType_Boss,

		eBattleType_Num,
	}; int mBattleType;

	enum ePhase {
		ePhase_StartProcess,		//戦闘開始
		ePhase_EventProcess,		//会話イベントなど
		ePhase_PreCommandProcess,	//状態異常ダメージなどのコマンド入力前処理
		ePhase_Player_Command,		//プレイヤー側の行動決定
		ePhase_Enemy_Command,		//敵側の行動決定
		ePhase_Action,				//戦闘シーン
		ePhase_Result,				//リザルト画面
		ePhase_EndProcess,			//戦闘終了

		ePhase_Num,					//フェーズ数
	}mPhase;

	bool winflag = false;
	bool eventflag = false;
	bool escapeflag = false;
	bool gameoverflag = false;
	bool preprocessflag = false;
	unsigned int Turn = 1;

	BattleChara* Player[PLAYER_MAX_NUM];
	BattleChara* Enemy[ENEMY_MAX_NUM];

public:
	Battle(IGameSceneChanger *const changer);
	void Initialize() override;    //初期化処理をオーバーライド。
	void Finalize() override;	   //終了処理をオーバーライド。
	void Update() override;        //更新処理をオーバーライド。
	void Draw() override;          //描画処理をオーバーライド。

	void InitPreLoad();
	void InitLoaded();
};