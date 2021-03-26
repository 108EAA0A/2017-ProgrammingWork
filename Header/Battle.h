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
		ePhase_StartProcess,		//�퓬�J�n
		ePhase_EventProcess,		//��b�C�x���g�Ȃ�
		ePhase_PreCommandProcess,	//��Ԉُ�_���[�W�Ȃǂ̃R�}���h���͑O����
		ePhase_Player_Command,		//�v���C���[���̍s������
		ePhase_Enemy_Command,		//�G���̍s������
		ePhase_Action,				//�퓬�V�[��
		ePhase_Result,				//���U���g���
		ePhase_EndProcess,			//�퓬�I��

		ePhase_Num,					//�t�F�[�Y��
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
	void Initialize() override;    //�������������I�[�o�[���C�h�B
	void Finalize() override;	   //�I���������I�[�o�[���C�h�B
	void Update() override;        //�X�V�������I�[�o�[���C�h�B
	void Draw() override;          //�`�揈�����I�[�o�[���C�h�B

	void InitPreLoad();
	void InitLoaded();
};