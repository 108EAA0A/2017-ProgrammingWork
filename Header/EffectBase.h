#pragma once

class EffectBase {
public:
	int handle;

	EffectBase() = default;
	EffectBase(const char *fileName);
	~EffectBase();

	//void LoadEffect(const char *const fileName);
};