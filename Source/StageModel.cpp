#include "StageModel.h"

StageModel::StageModel() {
	ObjectNum = 0;
	CollObjectNum = 0;
}

StageModel::~StageModel() {
	for (int i = 0; i < ObjectNum; ++i) {
		delete Object[i];
	}
	for (int i = 0; i < CollObjectNum; ++i) {
		if (objflag[i]) continue;
		delete CollObject[i];
	}
}

void StageModel::LoadObjectModel(const char *const Path, const char Flag) {
	if (Flag == OBJ_DUAL_FLAG_BIT) {
		Object[ObjectNum] = new Model(Path);
		CollObject[CollObjectNum] = Object[ObjectNum];
		objflag[CollObjectNum] = true;

		++ObjectNum;
		++CollObjectNum;
	}
	else if (Flag == OBJ_DRAW_FLAG_BIT) {
		Object[ObjectNum++] = new Model(Path);
	}
	else if (Flag == OBJ_COLL_FLAG_BIT) {
		CollObject[CollObjectNum++] = new Model(Path);
	}
}

void StageModel::SetupCollInfo(const int XDivNum, const int YDivNum, const int ZDivNum) {
	Model::SetupCollInfo(-1, XDivNum, YDivNum, ZDivNum);
	for (int i = 0; i < CollObjectNum; ++i) {
		CollObject[i]->SetupCollInfo(-1, XDivNum, YDivNum, ZDivNum);
	}
}

void StageModel::MakeStageInfo() {
	MV1_REF_POLYGONLIST REFLIST;

	MV1SetupReferenceMesh(handle, CollSetupFrameIndex, FALSE);
	REFLIST = MV1GetReferenceMesh(handle, CollSetupFrameIndex, FALSE);
	MaxPos = REFLIST.MaxPosition * scale;
	MinPos = REFLIST.MinPosition * scale;
	MV1TerminateReferenceMesh(handle, CollSetupFrameIndex, FALSE);

	width = VSize(MaxPos - MinPos);
}

void StageModel::SetScale(const float Scale) {
	Model::SetScale(Scale);
	for (int i = 0; i < ObjectNum; ++i) {
		Object[i]->SetScale(Scale);
	}
	for (int i = 0; i < CollObjectNum; ++i) {
		CollObject[i]->SetScale(Scale);
	}
}

void StageModel::SetScale(const VECTOR Scale) {
	Model::SetScale(Scale);
	for (int i = 0; i < ObjectNum; ++i) {
		Object[i]->SetScale(Scale);
	}
	for (int i = 0; i < CollObjectNum; ++i) {
		CollObject[i]->SetScale(Scale);
	}
}

void StageModel::draw() const {
	Model::draw();
	for (int i = 0; i < ObjectNum; ++i) {
		Object[i]->draw();
	}
}

void StageModel::drawall() const {
	StageModel::draw();
	for (int i = 0; i < CollObjectNum; ++i) {
		CollObject[i]->draw();
	}
}

bool StageModel::HitCheck_Line(const VECTOR PosStart, const VECTOR PosEnd) const {
	MV1_COLL_RESULT_POLY HitRes;
	
	// ステージ用コリジョン３Ｄモデルに当たるかをチェック
	HitRes = CollCheck_Line(PosStart, PosEnd);
	
	// 当たっていたらここで終了
	if (HitRes.HitFlag) return true;

	// ステージのコリジョン用ポリゴンに当たっていなかったら
	// ステージオブジェクトのコリジョン用ポリゴンにも当たっていないかチェック
	for (int i = 0; i < CollObjectNum; ++i) {
		HitRes = CollObject[i]->CollCheck_Line(PosStart, PosEnd);

		// 当たっていた時点で終了
		if (HitRes.HitFlag) return true;
	}

	// ここに来たら当たっていないということ
	return false;
}

bool StageModel::HitCheck_Sphere(const VECTOR CenterPos, const float r) const {
	MV1_COLL_RESULT_POLY_DIM HitDim;
	int                      HitNum;

	// ステージ用コリジョン３Ｄモデルに当たるかをチェック
	HitDim = CollCheck_Sphere(CenterPos, r);
	HitNum = HitDim.HitNum;
	MV1CollResultPolyDimTerminate(HitDim);

	// 当たっていたらここで終了
	if (HitNum > 0) return true;

	// ステージのコリジョン用ポリゴンに当たっていなかったら
	// ステージオブジェクトのコリジョン用ポリゴンにも当たっていないかチェック
	for (int i = 0; i < CollObjectNum; ++i) {
		HitDim = CollObject[i]->CollCheck_Sphere(CenterPos, r);
		HitNum = HitDim.HitNum;
		MV1CollResultPolyDimTerminate(HitDim);

		// 当たっていた時点で終了
		if (HitNum > 0) return true;
	}

	// ここに来たら当たっていないということ
	return false;
}

bool StageModel::HitCheck_Capsule(const Capsule& Cap) const {
	MV1_COLL_RESULT_POLY_DIM HitDim;
	int                      HitNum;
	
	// ステージ用コリジョン３Ｄモデルに当たるかをチェック
	HitDim = CollCheck_Capsule(Cap);
	HitNum = HitDim.HitNum;
	MV1CollResultPolyDimTerminate(HitDim);

	// 当たっていたらここで終了
	if (HitNum > 0) return true;

	// ステージのコリジョン用ポリゴンに当たっていなかったら
	// ステージオブジェクトのコリジョン用ポリゴンにも当たっていないかチェック
	for (int i = 0; i < CollObjectNum; ++i) {
		HitDim = CollObject[i]->CollCheck_Capsule(Cap);
		HitNum = HitDim.HitNum;
		MV1CollResultPolyDimTerminate(HitDim);

		// 当たっていた時点で終了
		if (HitNum > 0) return true;
	}

	// ここに来たら当たっていないということ
	return false;
}