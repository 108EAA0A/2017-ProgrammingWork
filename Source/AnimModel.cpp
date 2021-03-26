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
	if (animhandle.find(MapKey) != animhandle.end()) return; //キーが設定されているのでロードしない
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

	// 再生中のモーション２が有効だったらデタッチする
	if (PrevAnim.index != -1) {
		MV1DetachAnim(handle, PrevAnim.index);
		PrevAnim.index = -1;
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	PrevAnim = NowAnim;
	
	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	NowAnim.playnum = PlayAnimNum;
	NowAnim.index = MV1AttachAnim(handle, NowAnim.playnum);
	if (NowAnim.index == -1) mythrow("AnimModelアニメーションアタッチエラー");
	NowAnim.time = 0.0f;
	NowAnim.totaltime = MV1GetAttachAnimTotalTime(handle, NowAnim.index);

	//方向転換時のモーションリセットを阻止
	/*if (!((m_pChara->prevstate == Walk && m_pChara->state == Walk) || (m_pChara->prevstate == Run && m_pChara->state == Run))) {
		animtime = 0.0f;
	}*/

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	AnimBlendRate = PrevAnim.index == -1 ? 1.0f : 0.0f;
}
void AnimModel::SetAnimHandle(const char *const SetStatestr) {

	//if (CheckNeedNewAttach(playanim, PlayAnimNum)) return;

	// 再生中のモーション２が有効だったらデタッチする
	if (PrevAnim.index != -1) {
		MV1DetachAnim(handle, PrevAnim.index);
		PrevAnim.index = -1;
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	PrevAnim = NowAnim;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	if (animhandle.count(SetStatestr) == 0) {
		char buf[128];
		snprintf(buf, sizeof(buf), "「std::map animhandle」にキー「%s」は登録されていません", SetStatestr);
		mythrow(buf);
	}
	NowAnim.index = MV1AttachAnim(handle, 0, animhandle[SetStatestr]);
	if (NowAnim.index == -1) mythrow("AnimModelハンドルアニメーションアタッチエラー");
	NowAnim.totaltime = MV1GetAttachAnimTotalTime(handle, NowAnim.index);
	
	//方向転換時のモーションリセットを阻止
	/*if (!((m_pChara->prevstate == Walk && m_pChara->state == Walk) || (m_pChara->prevstate == Run && m_pChara->state == Run))) {
	animtime = 0.0f;
	}*/

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	AnimBlendRate = PrevAnim.index == -1 ? 1.0f : 0.0f;
}

void AnimModel::AnimProcess() {
	
	// ブレンド率が１以下の場合は１に近づける
	if (AnimBlendRate < 1.0f) {
		AnimBlendRate += ANIM_BLEND_SPEED;
		if (AnimBlendRate > 1.0f) {
			AnimBlendRate = 1.0f;
		}
	}

	// 再生しているアニメーション１の処理
	if (NowAnim.index != -1) {

		// 再生時間を進める
		NowAnim.time += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (NowAnim.time >= NowAnim.totaltime) {
			NowAnim.time = 0.0f;
		}
		
		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(handle, NowAnim.index, NowAnim.time);

		// アニメーション１のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(handle, NowAnim.index, AnimBlendRate);
	}

	// 再生しているアニメーション２の処理
	if (PrevAnim.index != -1) {

		// 再生時間を進める
		PrevAnim.time += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (PrevAnim.time > PrevAnim.totaltime) {
			PrevAnim.time = 0.0f;
		}
		
		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(handle, PrevAnim.index, PrevAnim.time);

		// アニメーション２のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(handle, PrevAnim.index, 1.0f - AnimBlendRate);
	}
}