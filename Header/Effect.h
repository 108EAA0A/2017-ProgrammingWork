#pragma once

#include "_DxLib.h"
#include "EffectBase.h"

class Effect {

public:
	Effect();
	Effect(const EffectBase &PlayEffect);
	~Effect();

	const EffectBase *mother;
	int playinghandle;

	Vector pos;
	Vector scale;
	Vector rotate;

	void SetPos(const VECTOR Pos);
	void SetScale(const float Scale);
	void SetScale(const VECTOR Scale);
	void SetRotate(const VECTOR Rotate);

	void Play();
	void Loop();
	void Stop() const;

};
