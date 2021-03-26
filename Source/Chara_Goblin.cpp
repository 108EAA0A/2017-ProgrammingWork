#include "Chara_Goblin.h"
#include "DxMath.h"

void Chara_Goblin::SetScale(const float Scale) {
	scale.x = scale.y = scale.z = Scale;
	MV1SetScale(handle, scale * 0.1f);
}

void Chara_Goblin::SetState(const StateEnum NewState) {
	prevstate = state;
	state = NewState;
	
	if (CharaState::JumpPreparation <= state && state <= CharaState::Landing) state = CharaState::Neutral;
	
	SetAnimHandle(Statestr.at(state));
}

void Chara_Goblin::DeadProcess() {

	if (!deadflag) {
		deadflag = true;

		SetState(CharaState::BlowIn);
	}
	if (deadflag && (state == CharaState::BlowIn) && (NowAnim.time >= NowAnim.totaltime)) {
		SetState(CharaState::BlowOut);
	}
	if (deadflag && (state == CharaState::BlowOut) && (NowAnim.time >= NowAnim.totaltime)) {
		SetState(CharaState::DownLoop);
	}
	if (deadflag && (state == CharaState::DownLoop)) {
		MV1SetOpacityRate(handle, 1.0f - (NowAnim.time / 60.0f));
		NowAnim.time += ANIM_PLAY_SPEED;
	}
}