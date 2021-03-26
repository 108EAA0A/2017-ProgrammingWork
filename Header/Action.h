#pragma once

#include "BaseScene.h"

//ゲーム画面クラス
class Action : public BaseScene {

private:
	void InitPreLoad();
	void InitLoaded();

public:
	Action(ISceneChanger *const changer);
	void Initialize() override;    //初期化処理をオーバーライド。
	void Finalize() override;      //終了処理をオーバーライド。
	void Update() override;        //更新処理をオーバーライド。
	void Draw() override;          //描画処理をオーバーライド。

};

/* ここからプロトタイプ宣言 */
void DrawModelStatic(const bool drawcollobj = false);
void DrawModelMovable(const bool IsNearFlag);
void DrawModelAll(const bool drawcollobj = false);
/* ここまでプロトタイプ宣言 */