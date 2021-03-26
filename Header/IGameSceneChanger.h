#pragma once

typedef enum _eGameScene {
	eGameScene_Action,	//�A�N�V�������
	eGameScene_Battle,	//�퓬���

	eGameScene_None,		//����
} eGameScene;

//�V�[����ύX���邽�߂̃C���^�[�t�F�C�X�N���X
class IGameSceneChanger {
public:
	virtual ~IGameSceneChanger() {}
	virtual void ChangeGameScene(const eGameScene NextScene) = 0;	//�w��V�[���ɕύX����
};