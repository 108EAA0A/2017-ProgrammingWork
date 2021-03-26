#include "EffectBase.h"

#include "_EffekseerForDXLib.h"

EffectBase::EffectBase(const char *const fileName) {
	handle = LoadEffekseerEffect(_T(fileName));
	if (handle == -1) mythrow("エフェクトの読み込みに失敗しました");
}

EffectBase::~EffectBase() {
	DeleteEffekseerEffect(handle);
}

/*void EffectBase::LoadEffect(const char *const fileName) {
	if (handle != -1) return;
	handle = LoadEffekseerEffect(_T(fileName));
}*/