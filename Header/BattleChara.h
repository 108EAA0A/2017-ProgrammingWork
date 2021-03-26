#pragma once

#include "CharaBase.h"

struct Status {
	std::string name;
	int HP, MP, ATK, DEF, MAT, MDF, AGI;
};

class BattleChara : public CharaBase {

public:
	BattleChara() = default;
	BattleChara(const Status Status);

	Status status;

};