#pragma once

typedef enum _eScene {
	eScene_Title,	//�^�C�g�����
	eScene_Config,	//�ݒ���
	eScene_Action,	//�Q�[�����
	
	eScene_None,    //����
} eScene;

//�V�[����ύX���邽�߂̃C���^�[�t�F�C�X�N���X
class ISceneChanger {
public:
	virtual ~ISceneChanger() {}
	virtual void ChangeScene(const eScene NextScene) = 0;	//�w��V�[���ɕύX����
};