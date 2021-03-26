#pragma once

//タスククラス。何かのモジュールはすべてこのTaskクラスを継承する。
class Task {
protected:
	virtual ~Task() {}
	virtual void Initialize() {}     //初期化処理は実装してもしなくてもいい
	virtual void Finalize()	  {}     //終了処理は実装してもしなくてもいい
	virtual void Update()	  = 0;   //更新処理は必ず継承先で実装する
	virtual void Draw()		  = 0;   //描画処理は必ず継承先で実装する
};