#pragma once

#include "BaseScene.h"

//�Q�[����ʃN���X
class Action : public BaseScene {

private:
	void InitPreLoad();
	void InitLoaded();

public:
	Action(ISceneChanger *const changer);
	void Initialize() override;    //�������������I�[�o�[���C�h�B
	void Finalize() override;      //�I���������I�[�o�[���C�h�B
	void Update() override;        //�X�V�������I�[�o�[���C�h�B
	void Draw() override;          //�`�揈�����I�[�o�[���C�h�B

};

/* ��������v���g�^�C�v�錾 */
void DrawModelStatic(const bool drawcollobj = false);
void DrawModelMovable(const bool IsNearFlag);
void DrawModelAll(const bool drawcollobj = false);
/* �����܂Ńv���g�^�C�v�錾 */