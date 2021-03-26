#pragma once

#include "Model.h"

#define STAGE_OBJECT_MAX_NUM 64

#define OBJ_DRAW_FLAG_BIT 0b01
#define OBJ_COLL_FLAG_BIT 0b10
#define OBJ_DUAL_FLAG_BIT (OBJ_DRAW_FLAG_BIT | OBJ_COLL_FLAG_BIT)

class StageModel : public Model {

	/*struct ObjStr{
		Model* pModel;
		bool objflag : 1;
	};*/

public:
	StageModel();
	~StageModel();

	Vector MaxPos, MinPos;
	float width;
	int ObjectNum, CollObjectNum;
	Model* Object[STAGE_OBJECT_MAX_NUM];
	Model* CollObject[STAGE_OBJECT_MAX_NUM];
	bool objflag[STAGE_OBJECT_MAX_NUM];

	Model Skydome;

	void LoadObjectModel(const char *const Path, const char Flag = OBJ_DUAL_FLAG_BIT);
	void SetupCollInfo(const int XDivNum = 32, const int YDivNum = 8, const int ZDivNum = 32);

	void MakeStageInfo();

	void SetScale(const float Scale);
	void SetScale(const VECTOR Scale);
	void draw() const;
	void drawall() const;

	bool HitCheck_Line(const VECTOR PosStart, const VECTOR PosEnd) const;	//線分とのコリジョン判定
	bool HitCheck_Sphere(const VECTOR CenterPos, const float r) const;	//球とのコリジョン判定
	bool HitCheck_Capsule(const Capsule& Cap) const;	//カプセルとのコリジョン判定
	
};