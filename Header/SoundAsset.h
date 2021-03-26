#pragma once

#include "_DxLib.h"
#include <vector>

typedef enum class SoundType {
	BGM,
	SE,
	//VOICE,	//Žg—p—\’è–³‚µ
	ENVIRONMENTAL,	//ŠÂ‹«‰¹

	SoundTypeNum,
}SoundEnum;

class SoundAsset {

//private:
public:
	int handle = -1;
	SoundEnum soundtype;
	unsigned char volume;
	
public:

	SoundAsset();
	SoundAsset(const SoundAsset& other);
	SoundAsset(const char *const FileName, const SoundEnum SoundType);
	~SoundAsset();

	SoundAsset& operator=(const SoundAsset& other);

	/*void load(const char *const FileName, const SoundEnum SoundType);
	void dupload(const SoundAsset& other);
	void unload();*/

	void play(const bool LoopFlag = false) const;
	void playMulti() const;
	void stop() const;
	SoundAsset& changevol(const unsigned char Volume);

	bool Isplay() const;
};