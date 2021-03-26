#pragma once

#include "_DxLib.h"

class Model {

private:
	std::string filepath;

protected:
	int CollSetupFrameIndex;	//コリジョン情報が構築されているフレームインデックス格納用変数

public:
	int handle = -1;
	Vector pos;
	Vector scale;
	Vector rotate;
	Vector speed;

	Model() {}
	Model(const Model& other);
	Model(const char *const Path);
	~Model();

	void LoadModel(const char *const Path);
	Model& DupModel(const Model& other);
	void UnLoadModel();
	
	void SetupCollInfo(const int FrameIndex = -1, const int XDivNum = 32, const int YDivNum = 8, const int ZDivNum = 32);	//コリジョン情報を構築する
	void TerminateCollInfo();	//コリジョン情報の後始末をする
	void RefreshCollInfo() const;		//コリジョン情報を更新する
	MV1_COLL_RESULT_POLY CollCheck_Line(const VECTOR PosStart, const VECTOR PosEnd) const;	//線分とのコリジョン判定
	MV1_COLL_RESULT_POLY_DIM CollCheck_Sphere(const VECTOR CenterPos, const float r) const;	//球とのコリジョン判定
	MV1_COLL_RESULT_POLY_DIM CollCheck_Capsule(const Capsule& Cap) const;	//カプセルとのコリジョン判定

	void draw() const;

	void ToonFlag(const bool Is) const;
	/* エッジをリサイズ */
	void OutLineResize(const float Scale) const;

	Model& SetPos(const VECTOR Pos);
	Model& SetScale(const float Scale);
	Model& SetScale(const VECTOR Scale);
	Model& SetRotate(const VECTOR Rotate);

	/* モデルの指定のフレームの座標を名前を使用して取得する */
	Vector GetFrameNamePos(const char *const FrameName) const;

	std::string GetFilePath() const;
};

class AttachModel : public Model {

private:
	int MotherHandle;

public:
	AttachModel() {}
	AttachModel(const AttachModel& other);
	~AttachModel() {}

	/* あるモデルの指定のフレームの位置に別のモデルを設置する */
	void SetModelFramePosition(const int ModelHandle, const char *const FrameName) const;

	void SetMotherHandle(const int MotherModelHandle);
	void SetMotherFrame(const char *const FrameName) const;

};