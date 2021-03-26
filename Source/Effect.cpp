#include "Effect.h"

#include "_EffekseerForDXLib.h"

Effect::Effect() {
	playinghandle = -1;
	mother = nullptr;
}

Effect::Effect(const EffectBase &PlayEffect) {
	mother = &PlayEffect;
}

Effect::~Effect() {
	Stop();
}

void Effect::SetPos(const VECTOR Pos) {
	pos = Pos;
	SetPosPlayingEffekseer3DEffect(playinghandle, Pos.x, Pos.y, Pos.z);
}

void Effect::SetScale(const float Scale) {
	scale.x = scale.y = scale.z = Scale;
	SetScalePlayingEffekseer3DEffect(playinghandle, Scale, Scale, Scale);
}

void Effect::SetScale(const VECTOR Scale) {
	scale = Scale;
	SetScalePlayingEffekseer3DEffect(playinghandle, Scale.x, Scale.y, Scale.z);
}

void Effect::SetRotate(const VECTOR Rotate) {
	rotate = Rotate;
	SetRotationPlayingEffekseer3DEffect(playinghandle, Rotate.x, Rotate.y, Rotate.z);
}

void Effect::Play() {
	playinghandle = PlayEffekseer3DEffect(mother->handle);
	if (playinghandle == -1) mythrow("エフェクトの再生に失敗しました");
}

void Effect::Loop() {
	if(IsEffekseer3DEffectPlaying(playinghandle) == 0) return;

	playinghandle = PlayEffekseer3DEffect(mother->handle);
	if (playinghandle == -1) mythrow("エフェクトの再生に失敗しました");
}

void Effect::Stop() const {
	extern ::Effekseer::Manager *EfMgr;

	EfMgr->StopEffect(static_cast<Effekseer::Handle>(playinghandle));
}