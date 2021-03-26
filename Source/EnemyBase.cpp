#include "EnemyBase.h"

#include "Table.h"

EnemyBase::EnemyBase(const std::string& EnemyID) {
	Table EnemyStatus(_T("resource/enemystatus.csv"));

	status.name = EnemyStatus[EnemyID.c_str()]["name"];
	status.HP = EnemyStatus[EnemyID.c_str()]["HP"];
	status.MP = EnemyStatus[EnemyID.c_str()]["MP"];
	status.ATK = EnemyStatus[EnemyID.c_str()]["ATK"];
	status.DEF = EnemyStatus[EnemyID.c_str()]["DEF"];
	status.MAT = EnemyStatus[EnemyID.c_str()]["MAT"];
	status.MDF = EnemyStatus[EnemyID.c_str()]["MDF"];
	status.AGI = EnemyStatus[EnemyID.c_str()]["AGI"];

}


void EnemyBase::DeadProcess() {

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