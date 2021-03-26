#include "Model.h"

Model::Model(const Model& other) {
	DupModel(other);
}

Model::Model(const char *const Path) {
	LoadModel(Path);
}

Model::~Model() {
	MV1DeleteModel(handle);
}

void Model::draw() const {
	MV1DrawModel(handle);
}

void Model::LoadModel(const char *const Path) {
	if (handle != -1) return;
	handle = MV1LoadModel(_T(Path));
	if (handle == -1) mythrow("Modelロードエラー");
	MV1SetUseZBuffer(handle, TRUE);
	MV1SetWriteZBuffer(handle, TRUE);
	filepath = Path;
}

Model& Model::DupModel(const Model& other) {
	if (handle != -1) return *this;
	handle = MV1DuplicateModel(other.handle);
	if (handle == -1) mythrow("Model複製ロードエラー");
	MV1SetUseZBuffer(handle, TRUE);
	MV1SetWriteZBuffer(handle, TRUE);
	filepath = other.GetFilePath();

	return *this;
}

void Model::UnLoadModel() {
	MV1DeleteModel(handle);
	handle = -1;
}

void Model::SetupCollInfo(const int FrameIndex, const int XDivNum, const int YDivNum, const int ZDivNum) {
	CollSetupFrameIndex = FrameIndex;
	MV1SetupCollInfo(handle, FrameIndex, XDivNum, YDivNum, ZDivNum);
}

void Model::TerminateCollInfo() {
	MV1TerminateCollInfo(handle, CollSetupFrameIndex);
	CollSetupFrameIndex = -1;
}

void Model::RefreshCollInfo() const {
	MV1RefreshCollInfo(handle, CollSetupFrameIndex);
}

MV1_COLL_RESULT_POLY Model::CollCheck_Line(const VECTOR PosStart, const VECTOR PosEnd) const {
	return MV1CollCheck_Line(handle, CollSetupFrameIndex, PosStart, PosEnd);
}

MV1_COLL_RESULT_POLY_DIM Model::CollCheck_Sphere(const VECTOR CenterPos, const float r) const {
	return MV1CollCheck_Sphere(handle, CollSetupFrameIndex, CenterPos, r);
}

MV1_COLL_RESULT_POLY_DIM Model::CollCheck_Capsule(const Capsule& Cap) const {
	return MV1CollCheck_Capsule(handle, CollSetupFrameIndex, Cap.posD, Cap.posU, Cap.r);
}

void Model::OutLineResize(const float Scale) const {
	const int MaterialNum = MV1GetMaterialNum(handle);
	for (int j = 0; j < MaterialNum; ++j) {
		float dotwidth = MV1GetMaterialOutLineDotWidth(handle, j);	// マテリアルの輪郭線の太さを取得
		MV1SetMaterialOutLineDotWidth(handle, j, dotwidth / Scale);	// マテリアルの輪郭線の太さを拡大した分小さくする
	}
}

void Model::ToonFlag(const bool Is) const {
	const int MaterialNum = MV1GetMaterialNum(handle);
	if (Is) for (int j = 0; j < MaterialNum; ++j) MV1SetMaterialType(handle, j, DX_MATERIAL_TYPE_TOON);
	else for (int j = 0; j < MaterialNum; ++j) MV1SetMaterialType(handle, j, DX_MATERIAL_TYPE_NORMAL);
}

Model& Model::SetPos(const VECTOR Pos) {
	pos = Pos;
	MV1SetPosition(handle, Pos);

	return *this;
}

Model& Model::SetScale(const float Scale) {
	scale.x = scale.y = scale.z = Scale;
	MV1SetScale(handle, scale);
	OutLineResize(scale.y);

	return *this;
}

Model& Model::SetScale(const VECTOR Scale) {
	scale = Scale;
	MV1SetScale(handle, Scale);
	OutLineResize(scale.y);

	return *this;
}

Model& Model::SetRotate(const VECTOR Rotate) {
	rotate = Rotate;
	MV1SetRotationXYZ(handle, Rotate);

	return *this;
}

Vector Model::GetFrameNamePos(const char *const FrameName) const {
	return MV1GetFramePosition(handle, MV1SearchFrame(handle, FrameName));
}

std::string Model::GetFilePath() const {
	return filepath;
}

AttachModel::AttachModel(const AttachModel& other) : Model(other){
}

void AttachModel::SetModelFramePosition(const int ModelHandle, const char *const FrameName) const {
	int FrameIndex;
	MATRIX FrameMatrix;
	MATRIX RotateMatrix;
	MATRIX SetMatrix;

	// フレーム名からフレーム番号を取得する
	FrameIndex = MV1SearchFrame(ModelHandle, FrameName);

	// フレームの現在のワールドでの状態を示す行列を取得する
	FrameMatrix = MV1GetFrameLocalWorldMatrix(ModelHandle, FrameIndex);

	// フレームの回転状態を示す行列を取得する
	RotateMatrix = MMult(MMult(MGetRotX(rotate.x), MGetRotY(rotate.y)), MGetRotZ(rotate.z));

	// フレームの現在のワールドでの状態にモデルに設定されたパラメータを合成する
	SetMatrix = MMult(RotateMatrix, MMult(MGetScale(scale), MMult(MGetTranslate(pos), FrameMatrix)));

	// セットするモデルの状態を示す行列を適用する
	MV1SetMatrix(handle, SetMatrix);
}

void AttachModel::SetMotherHandle(const int MotherModelHandle) {
	MotherHandle = MotherModelHandle;
}

void AttachModel::SetMotherFrame(const char *const FrameName) const {
	int FrameIndex;
	MATRIX FrameMatrix;
	MATRIX RotateMatrix;
	MATRIX SetMatrix;

	// フレーム名からフレーム番号を取得する
	FrameIndex = MV1SearchFrame(MotherHandle, FrameName);

	// フレームの現在のワールドでの状態を示す行列を取得する
	FrameMatrix = MV1GetFrameLocalWorldMatrix(MotherHandle, FrameIndex);

	// フレームの回転状態を示す行列を取得する
	RotateMatrix = MMult(MMult(MGetRotX(rotate.x), MGetRotY(rotate.y)), MGetRotZ(rotate.z));

	// フレームの現在のワールドでの状態にモデルに設定されたパラメータを合成する
	SetMatrix = MMult(RotateMatrix, MMult(MGetScale(scale), MMult(MGetTranslate(pos), FrameMatrix)));

	// セットするモデルの状態を示す行列を適用する
	MV1SetMatrix(handle, SetMatrix);
}