#include "BaseScene.h"

#include "DebugTools.h"

BaseScene::BaseScene(ISceneChanger *const changer) {
	mSceneChanger = changer;
}