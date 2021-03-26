#pragma once

#include "_DxLib.h"

class Model {

private:
	std::string filepath;

protected:
	int CollSetupFrameIndex;	//�R���W������񂪍\�z����Ă���t���[���C���f�b�N�X�i�[�p�ϐ�

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
	
	void SetupCollInfo(const int FrameIndex = -1, const int XDivNum = 32, const int YDivNum = 8, const int ZDivNum = 32);	//�R���W���������\�z����
	void TerminateCollInfo();	//�R���W�������̌�n��������
	void RefreshCollInfo() const;		//�R���W���������X�V����
	MV1_COLL_RESULT_POLY CollCheck_Line(const VECTOR PosStart, const VECTOR PosEnd) const;	//�����Ƃ̃R���W��������
	MV1_COLL_RESULT_POLY_DIM CollCheck_Sphere(const VECTOR CenterPos, const float r) const;	//���Ƃ̃R���W��������
	MV1_COLL_RESULT_POLY_DIM CollCheck_Capsule(const Capsule& Cap) const;	//�J�v�Z���Ƃ̃R���W��������

	void draw() const;

	void ToonFlag(const bool Is) const;
	/* �G�b�W�����T�C�Y */
	void OutLineResize(const float Scale) const;

	Model& SetPos(const VECTOR Pos);
	Model& SetScale(const float Scale);
	Model& SetScale(const VECTOR Scale);
	Model& SetRotate(const VECTOR Rotate);

	/* ���f���̎w��̃t���[���̍��W�𖼑O���g�p���Ď擾���� */
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

	/* ���郂�f���̎w��̃t���[���̈ʒu�ɕʂ̃��f����ݒu���� */
	void SetModelFramePosition(const int ModelHandle, const char *const FrameName) const;

	void SetMotherHandle(const int MotherModelHandle);
	void SetMotherFrame(const char *const FrameName) const;

};