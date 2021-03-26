#include "SceneMgr.h"

#include "Action.h"
#include "Config.h"
#include "Title.h"

SceneMgr::SceneMgr() {
	mScene = (BaseScene*) new Title(this);
	mNextScene = eScene_None;
}

//������
void SceneMgr::Initialize() {
	mScene->Initialize();
}

//�I������
void SceneMgr::Finalize() {
	mScene->Finalize();
}

//�X�V
void SceneMgr::Update() {
	if (mNextScene != eScene_None) {    //���̃V�[�����Z�b�g����Ă�����
		mScene->Finalize();				//���݂̃V�[���̏I�����������s
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
			mythrow("mScene��O");
			break;
		}
		mNextScene = eScene_None;    //���̃V�[�������N���A
		mScene->Initialize();    //�V�[����������
	}

	mScene->Update(); //�V�[���̍X�V
}

//�`��
void SceneMgr::Draw() {
	mScene->Draw(); //�V�[���̕`��
}

// ���� nextScene �ɃV�[����ύX����
void SceneMgr::ChangeScene(const eScene NextScene) {
	mNextScene = NextScene;    //���̃V�[�����Z�b�g����
}