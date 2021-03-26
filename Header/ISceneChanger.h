#pragma once

typedef enum _eScene {
	eScene_Title,	//タイトル画面
	eScene_Config,	//設定画面
	eScene_Action,	//ゲーム画面
	
	eScene_None,    //無し
} eScene;

//シーンを変更するためのインターフェイスクラス
class ISceneChanger {
public:
	virtual ~ISceneChanger() {}
	virtual void ChangeScene(const eScene NextScene) = 0;	//指定シーンに変更する
};