#pragma once

#include "Task.h"
#include "IGameSceneChanger.h"

//�V�[���̊��N���X�B
class GameBaseScene : private Task {

protected:
	IGameSceneChanger* mGameSceneChanger;    //�N���X���L���ɃV�[���؂�ւ���`����C���^�[�t�F�C�X

public:
	GameBaseScene(IGameSceneChanger *const changer);
	virtual ~GameBaseScene() {}
	virtual void Initialize() override {}    //�������������I�[�o�[���C�h�B
	virtual void Finalize() override {}      //�I���������I�[�o�[���C�h�B
	virtual void Update() override {}        //�X�V�������I�[�o�[���C�h�B
	virtual void Draw() override;            //�`�揈�����I�[�o�[���C�h�B

};