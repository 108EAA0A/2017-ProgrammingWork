#include "SoundAsset.h"

#include "ConfigData.h"

SoundAsset::SoundAsset() {
	soundtype = SoundType::SoundTypeNum;
	volume = 0xFF;
}

SoundAsset::SoundAsset(const SoundAsset& other) {
	handle = DuplicateSoundMem(other.handle);
	if (handle == -1) mythrow("サウンド複製ロードエラー");
	soundtype = other.soundtype;
	volume = other.volume;
}

SoundAsset::SoundAsset(const char *const FileName, const SoundEnum SoundType) {
	handle = LoadSoundMem(_T(FileName));
	if (handle == -1) mythrow("サウンドロードエラー");
	soundtype = SoundType;
	volume = 0xFF;
}

SoundAsset::~SoundAsset() {
	DeleteSoundMem(handle);
}

SoundAsset& SoundAsset::operator=(const SoundAsset& other) {
	handle = other.handle;
	soundtype = other.soundtype;
	volume = other.volume;

	return *this;
}

/*void SoundAsset::load(const char *const FileName, const SoundEnum SoundType) {
	if (handle == -1) return;
	
	handle = LoadSoundMem(_T(FileName));
	if (handle == -1) mythrow("サウンドロードエラー");
	soundtype = SoundType;
}

void SoundAsset::dupload(const SoundAsset& other) {
	if (handle == -1) return;

	handle = DuplicateSoundMem(other.handle);
	if (handle == -1) mythrow("サウンド複製ロードエラー");
	soundtype = other.soundtype;
	volume = other.volume;
}

void SoundAsset::unload() {
	DeleteSoundMem(handle);
}*/

void SoundAsset::play(const bool LoopFlag) const {
	if (Isplay()) return;

	if (LoopFlag) PlaySoundMem(handle, DX_PLAYTYPE_LOOP);
	else PlaySoundMem(handle, DX_PLAYTYPE_BACK);
}

void SoundAsset::playMulti() const {
	PlaySoundMem(handle, DX_PLAYTYPE_BACK);
}

void SoundAsset::stop() const {
	StopSoundMem(handle);
}

SoundAsset& SoundAsset::changevol(const unsigned char Volume) {
	extern ConfigData cConfigData;

	volume = Volume;
	
	switch (soundtype) {
	case SoundType::BGM:
		ChangeVolumeSoundMem((Volume / cConfigData.BGMVol) / cConfigData.MasterVol, handle);
		break;
	case SoundType::SE:
		ChangeVolumeSoundMem((Volume / cConfigData.SEVol) / cConfigData.MasterVol, handle);
		break;
	default:
		mythrow("音タイプ例外");
		break;
	}

	return (*this);
}

// 再生中ならtrueを返す
bool SoundAsset::Isplay() const {
	return (CheckSoundMem(handle) == 1);
}