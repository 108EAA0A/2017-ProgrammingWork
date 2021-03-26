#pragma once

#include "BaseScene.h"

//設定画面クラス
class Config : public BaseScene {

public:
	Config(ISceneChanger *const changer);
	void Initialize() override;    //初期化処理をオーバーライド。
	void Finalize() override;	   //終了処理をオーバーライド。
	void Update() override;        //更新処理をオーバーライド。
	void Draw() override;          //描画処理をオーバーライド。

};