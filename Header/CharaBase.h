#pragma once

#include "AnimModel.h"

#include "_DxLib.h"
#include <map>
#include <string>

typedef enum class CharaState {
	Neutral,

	Walk,
	Run,

	JumpPreparation,
	Jump,
	Fall,
	Falling,
	Landing,

	Attack,

	Damage,

	BlowIn,
	Blowing,
	BlowOut,

	DownLoop,

	StateNum,
}StateEnum;

class CharaBase : public AnimModel {

public:
	CharaBase();
	//CharaBase(const char *const Path);
	CharaBase(const CharaBase& other);
	~CharaBase();

	void LoadModel(const char *const Path);
	Model& DupModel(const Model& other);
	void UnLoadModel();

	static std::map<int, const CharaBase *const> CharaArrey;

	static const std::map<const StateEnum, const char *const> Statestr;

	Vector speed;
	float walkspeed;
	float dashspeed;
	float jumppow;
	float getoverheight;
	Vector direction;
	StateEnum state;
	StateEnum prevstate;
	Vector respawnpos;
	Capsule collCap;
	Cube coverbox;

	float AdjustAnimPos;

	void SetSpeed(const VECTOR Speed);
	void SetJumppow(const float SetParam);
	void SetDirection(const VECTOR NewDirection);
	virtual void SetState(const StateEnum NewState) = 0;	//åpè≥êÊÇ≈ïKÇ∏é¿ëï
	void SetRespawnPos(const VECTOR SetPos);
	
	//void SetCollCap(const VECTOR PosD, const VECTOR PosU, const float R);
	void SetCollCap(const float Height, const float R);
	void UpdateCollCap();
	
	void SetCoverbox(const VECTOR VEC);
	void SetCoverbox(const float x, const float y, const float z);
	void SetCoverbox(const VECTOR Pos1, const VECTOR Pos2);
	void UpdateCoverbox();

	void MoveProcess();
	void drawCollCap();
	void drawCoverBox();

private:
	void Coll_Chara_vs_Chara(const CharaBase& vsCheckChara);

};