#pragma once

#include "Task.h"
#include "IGameSceneChanger.h"

//シーンの基底クラス。
class GameBaseScene : private Task {

protected:
	IGameSceneChanger* mGameSceneChanger;    //クラス所有元にシーン切り替えを伝えるインターフェイス

public:
	GameBaseScene(IGameSceneChanger *const changer);
	virtual ~GameBaseScene() {}
	virtual void Initialize() override {}    //初期化処理をオーバーライド。
	virtual void Finalize() override {}      //終了処理をオーバーライド。
	virtual void Update() override {}        //更新処理をオーバーライド。
	virtual void Draw() override;            //描画処理をオーバーライド。

};