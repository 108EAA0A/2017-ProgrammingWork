#include "SceneMgr.h"

#include "Action.h"
#include "Config.h"
#include "Title.h"

SceneMgr::SceneMgr() {
	mScene = (BaseScene*) new Title(this);
	mNextScene = eScene_None;
}

//初期化
void SceneMgr::Initialize() {
	mScene->Initialize();
}

//終了処理
void SceneMgr::Finalize() {
	mScene->Finalize();
}

//更新
void SceneMgr::Update() {
	if (mNextScene != eScene_None) {    //次のシーンがセットされていたら
		mScene->Finalize();				//現在のシーンの終了処理を実行
		delete mScene;
		switch (mNextScene) {
		case eScene_Action:
			mScene = new Action(this);
			break;
		case eScene_Title:
			mScene = new Title(this);
			break;
		case eScene_Config:
			mScene = new Config(this);
			break;
		default:
			mythrow("mScene例外");
			break;
		}
		mNextScene = eScene_None;    //次のシーン情報をクリア
		mScene->Initialize();    //シーンを初期化
	}

	mScene->Update(); //シーンの更新
}

//描画
void SceneMgr::Draw() {
	mScene->Draw(); //シーンの描画
}

// 引数 nextScene にシーンを変更する
void SceneMgr::ChangeScene(const eScene NextScene) {
	mNextScene = NextScene;    //次のシーンをセットする
}