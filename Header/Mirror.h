#pragma once

#include "_DxLib.h"

#define MIRROR_NUM			1		// ‹¾‚Ì”


// ‹¾‚Ì‰Šú‰»
void Mirror_Initialize();

void Mirror_Finalize();

// ‹¾‚É‰f‚é‰f‘œ‚ğ•`‰æ‚·‚é‚½‚ß‚ÌƒJƒƒ‰‚Ìİ’è‚ğs‚¤
void Mirror_SetupCamera(const int MirrorNo, const VECTOR CameraEyePos, const VECTOR CameraTargetPos);

// ‹¾‚Ì•`‰æ
void Mirror_Render(const int MirrorNo);

// ‹¾‚ÌƒfƒoƒbƒO•`‰æ
void Mirror_DebugRender(const int MirrorNo, const int DrawX, const int DrawY);