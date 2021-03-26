#pragma once

#include "_DxLib.h"
#include "CharaBase.h"

#define PLAYER_ROTATE_SPEED 0.2f //プレイヤー振り向き速度

class Chara_Player : public CharaBase {

public:
	
	AttachModel weaponmodel;

	Vector respawnpos;
	
	void SetState(const StateEnum NewState);
	void RotateProcess();
	void JumpControl();
	void PlayerControl();

};