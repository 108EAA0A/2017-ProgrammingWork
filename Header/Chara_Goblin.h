#pragma once

#include "CharaBase.h"

class Chara_Goblin : public CharaBase {

public:
	Chara_Goblin() {}
	~Chara_Goblin() {}

	AttachModel weaponmodel;

	float visibility;
	bool FoundPl : 1;
	bool deadflag : 1;

	void SetScale(const float Scale);
	void SetState(const StateEnum NewState);

	void DeadProcess();
};
