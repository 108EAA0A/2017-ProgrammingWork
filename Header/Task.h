#pragma once

//�^�X�N�N���X�B�����̃��W���[���͂��ׂĂ���Task�N���X���p������B
class Task {
protected:
	virtual ~Task() {}
	virtual void Initialize() {}     //�����������͎������Ă����Ȃ��Ă�����
	virtual void Finalize()	  {}     //�I�������͎������Ă����Ȃ��Ă�����
	virtual void Update()	  = 0;   //�X�V�����͕K���p����Ŏ�������
	virtual void Draw()		  = 0;   //�`�揈���͕K���p����Ŏ�������
};