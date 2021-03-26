#include "AnimModel.h"

AnimModel::AnimModel(const char *const ModelPath) : Model(ModelPath) {
}

AnimModel::AnimModel(const AnimModel& CopyModel) : Model(CopyModel) {
	for (auto it = CopyModel.animhandle.begin(); it != CopyModel.animhandle.end(); ++it) {
		animhandle.insert(std::map<std::string, int>::value_type(it->first, MV1DuplicateModel(it->second)));
	}
}

AnimModel::~AnimModel() {
	UnLoadAnimModel();
}

void AnimModel::LoadAnimModel(const char *const MapKey, const char *const AnimPath) {
	if (animhandle.find(MapKey) != animhandle.end()) return; //�L�[���ݒ肳��Ă���̂Ń��[�h���Ȃ�
	animhandle.insert(std::map<std::string, int>::value_type(MapKey, MV1LoadModel(_T(AnimPath))));
}

AnimModel& AnimModel::DupAnimModel(const std::map<std::string, int>& CopyAnimHandle) {
	for (auto it = CopyAnimHandle.begin(); it != CopyAnimHandle.end(); ++it) {
		animhandle.insert(std::map<std::string, int>::value_type(it->first, MV1DuplicateModel(it->second)));
	}

	return *this;
}

void AnimModel::UnLoadAnimModel() {
	animhandle.clear();
}

void AnimModel::SetAnim(const int PlayAnimNum) {
	
	if (NowAnim.playnum == PlayAnimNum) return;

	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (PrevAnim.index != -1) {
		MV1DetachAnim(handle, PrevAnim.index);
		PrevAnim.index = -1;
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	PrevAnim = NowAnim;
	
	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	NowAnim.playnum = PlayAnimNum;
	NowAnim.index = MV1AttachAnim(handle, NowAnim.playnum);
	if (NowAnim.index == -1) mythrow("AnimModel�A�j���[�V�����A�^�b�`�G���[");
	NowAnim.time = 0.0f;
	NowAnim.totaltime = MV1GetAttachAnimTotalTime(handle, NowAnim.index);

	//�����]�����̃��[�V�������Z�b�g��j�~
	/*if (!((m_pChara->prevstate == Walk && m_pChara->state == Walk) || (m_pChara->prevstate == Run && m_pChara->state == Run))) {
		animtime = 0.0f;
	}*/

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	AnimBlendRate = PrevAnim.index == -1 ? 1.0f : 0.0f;
}
void AnimModel::SetAnimHandle(const char *const SetStatestr) {

	//if (CheckNeedNewAttach(playanim, PlayAnimNum)) return;

	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (PrevAnim.index != -1) {
		MV1DetachAnim(handle, PrevAnim.index);
		PrevAnim.index = -1;
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	PrevAnim = NowAnim;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	if (animhandle.count(SetStatestr) == 0) {
		char buf[128];
		snprintf(buf, sizeof(buf), "�ustd::map animhandle�v�ɃL�[�u%s�v�͓o�^����Ă��܂���", SetStatestr);
		mythrow(buf);
	}
	NowAnim.index = MV1AttachAnim(handle, 0, animhandle[SetStatestr]);
	if (NowAnim.index == -1) mythrow("AnimModel�n���h���A�j���[�V�����A�^�b�`�G���[");
	NowAnim.totaltime = MV1GetAttachAnimTotalTime(handle, NowAnim.index);
	
	//�����]�����̃��[�V�������Z�b�g��j�~
	/*if (!((m_pChara->prevstate == Walk && m_pChara->state == Walk) || (m_pChara->prevstate == Run && m_pChara->state == Run))) {
	animtime = 0.0f;
	}*/

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	AnimBlendRate = PrevAnim.index == -1 ? 1.0f : 0.0f;
}

void AnimModel::AnimProcess() {
	
	// �u�����h�����P�ȉ��̏ꍇ�͂P�ɋ߂Â���
	if (AnimBlendRate < 1.0f) {
		AnimBlendRate += ANIM_BLEND_SPEED;
		if (AnimBlendRate > 1.0f) {
			AnimBlendRate = 1.0f;
		}
	}

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (NowAnim.index != -1) {

		// �Đ����Ԃ�i�߂�
		NowAnim.time += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (NowAnim.time >= NowAnim.totaltime) {
			NowAnim.time = 0.0f;
		}
		
		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(handle, NowAnim.index, NowAnim.time);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(handle, NowAnim.index, AnimBlendRate);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (PrevAnim.index != -1) {

		// �Đ����Ԃ�i�߂�
		PrevAnim.time += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (PrevAnim.time > PrevAnim.totaltime) {
			PrevAnim.time = 0.0f;
		}
		
		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(handle, PrevAnim.index, PrevAnim.time);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(handle, PrevAnim.index, 1.0f - AnimBlendRate);
	}
}