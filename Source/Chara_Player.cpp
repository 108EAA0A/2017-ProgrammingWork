#include "Chara_Player.h"
#include "DxMath.h"
#include "UpdateKey.h"

void Chara_Player::SetState(const StateEnum NewState) {
	if (state == NewState) return;
	prevstate = state;
	state = NewState;

	int NewAnim;
	switch (NewState) {
		case CharaState::Neutral:			NewAnim = 0; break;
		case CharaState::Walk:				NewAnim = 1; break;
		case CharaState::Run:				NewAnim = 2; break;
		case CharaState::JumpPreparation:	NewAnim = 3; break;
		case CharaState::Jump:				NewAnim = 4; break;
		case CharaState::Fall:				NewAnim = 5; break;
		case CharaState::Falling:			NewAnim = 6; break;
		case CharaState::Landing:			NewAnim = 7; break;
		case CharaState::Attack:			NewAnim = 8; break;
		default:							NewAnim = -1;break;
	}
	SetAnim(NewAnim);
}

void Chara_Player::RotateProcess() {
	ParamChangeAngle(&rotate.y, direction.y, 1.0f, PLAYER_ROTATE_SPEED);
	MV1SetRotationXYZ(handle, rotate);
}

void Chara_Player::JumpControl() {
	
	if (NowAnim.time >= NowAnim.totaltime - ANIM_PLAY_SPEED) {
		switch (state) {
			case CharaState::JumpPreparation:
				SetState(CharaState::Jump);
				speed.y += jumppow;
				break;
			case CharaState::Jump:
				SetState(CharaState::Fall);
				break;
			case CharaState::Fall:
				SetState(CharaState::Falling);
				AnimBlendRate = 1.0f;
				break;
			case CharaState::Landing:
				SetState(CharaState::Neutral);
				AnimBlendRate = 1.0f;
				//pos.y -= AdjustAnimPos;
				break;
		}
	}
	
	if (fabs(pos.y) > 500.0f) {
		pos = respawnpos;
		speed = VGet(0, 0, 0);
		SetState(CharaState::Neutral);
		AnimBlendRate = 1.0f;
		MV1ResetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "センター"));
		pos.y -= AdjustAnimPos;
		MV1SetPosition(handle, pos);
	}
}

void Chara_Player::PlayerControl() {
	extern unsigned int KeyFrame[256];
	extern char DirectionX, DirectionZ;

	bool SprintFlag = false;

	InputDirection();

	if (KeyFrame[KEY_INPUT_SPACE] == 1) {

		if (CharaState::JumpPreparation <= state && state <= CharaState::Fall) return;

		speed.x = 0.0f;
		speed.z = 0.0f;

		SetState(CharaState::JumpPreparation);

		MV1SetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "センター"), MGetTranslate(VGet(0.0f, AdjustAnimPos, 0.0f)));
		return;
	}

	if (DirectionX == '\0') speed.x = 0.0f;
	if (DirectionZ == '\0') speed.z = 0.0f;

	if (KeyFrame[KEY_INPUT_LSHIFT] > 0) SprintFlag = true;

	if ((DirectionX != '\0') || (DirectionZ != '\0')) {
		if (state == CharaState::JumpPreparation) return;

		float rotaterad = 0.0f;
		if (DirectionZ == 'D') rotaterad += radf(180.0f);
		if (DirectionX == 'L') rotaterad -= radf(90.0f);
		if (DirectionX == 'R') rotaterad += radf(90.0f);
		if (DirectionX != '\0' && DirectionZ != '\0') rotaterad /= 2.0f;
		if (DirectionZ == 'D' && DirectionX == 'L') rotaterad = radf(225.0f);

		direction.y = GetCameraAngleHRotate() + DX_PI_F + rotaterad;

		if (SprintFlag) {
			speed.x = -dashspeed*sinf(direction.y);
			speed.z = -dashspeed*cosf(direction.y);
		}
		else {
			speed.x = -walkspeed*sinf(direction.y);
			speed.z = -walkspeed*cosf(direction.y);
		}

		if (CharaState::Jump <= state && state <= CharaState::Falling) return;

		if (SprintFlag) {
			SetState(CharaState::Run);
		}
		else {
			SetState(CharaState::Walk);
		}
		return;
	}

	//以下無操作状態の処理
	if (state == CharaState::Neutral) return;

	speed.x = 0.0f;
	speed.z = 0.0f;

	if (CharaState::JumpPreparation <= state && state <= CharaState::Landing) return;
	SetState(CharaState::Neutral);

	return;
}