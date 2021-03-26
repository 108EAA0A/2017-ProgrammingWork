#pragma once

#include "Model.h"
#include <map>

#define ANIM_PLAY_SPEED 0.5f
#define ANIM_BLEND_SPEED 0.1f

class AnimModel : public Model {

	struct AnimInfo	{
		int playnum = -1;
		int index = -1;
		float time = 0.0f;
		float totaltime;
	};

public:
	AnimModel() {}
	AnimModel(const char *const ModelPath);
	AnimModel(const AnimModel& CopyModel);
	~AnimModel();

	void LoadAnimModel(const char *const MapKey, const char *const AnimPath);
	AnimModel& DupAnimModel(const std::map<std::string, int>& CopyAnimHandle);
	void UnLoadAnimModel();

	void SetAnim(const int PlayAnimNum);
	void SetAnimHandle(const char *const SetStatestr);
	void AnimProcess();

	std::map<std::string, int> animhandle;

	AnimInfo NowAnim;
	AnimInfo PrevAnim;
	float AnimBlendRate = 1.0f;

};
