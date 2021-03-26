#pragma once

#include "BattleChara.h"

class EnemyBase : public BattleChara {

public:
	EnemyBase() {}
	EnemyBase(const std::string& EnemyID);

	std::string battleID;

	float visibility;
	bool FoundPl : 1;
	bool deadflag : 1;

	void DeadProcess();
};