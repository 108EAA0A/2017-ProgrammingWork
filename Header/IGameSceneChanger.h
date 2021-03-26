#pragma once

typedef enum _eGameScene {
	eGameScene_Action,	//アクション画面
	eGameScene_Battle,	//戦闘画面

	eGameScene_None,		//無し
} eGameScene;

//シーンを変更するためのインターフェイスクラス
class IGameSceneChanger {
public:
	virtual ~IGameSceneChanger() {}
	virtual void ChangeGameScene(const eGameScene NextScene) = 0;	//指定シーンに変更する
};