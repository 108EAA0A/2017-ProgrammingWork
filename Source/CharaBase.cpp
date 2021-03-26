#include "CharaBase.h"

#include "StageModel.h"

#define GRAVITY					0.049f
#define CHARA_MAX_FALLSPEED		-100.0f		// �ő嗎�����x
#define CHARA_MAX_HITCOLL		2048		// ��������R���W�����|���S���̍ő吔
#define CHARA_HIT_TRYNUM		16			// �ǉ����o�������̍ő厎�s��
#define CHARA_HIT_PUSH_POWER	5.0f		// �L�����N�^�[���m�œ��������Ƃ��̉����o������

const std::map<const StateEnum, const char *const> CharaBase::Statestr = {
		{ CharaState::Neutral ,"Neutral"},
		{ CharaState::Walk ,"Walk"},
		{ CharaState::Run ,"Run"},
		{ CharaState::JumpPreparation ,"JumpPreparation"},
		{ CharaState::Jump ,"Jump"},
		{ CharaState::Fall ,"Fall"},
		{ CharaState::Falling ,"Falling" },
		{ CharaState::Landing ,"Landing"},
		{ CharaState::Attack ,"Attack"},
		{ CharaState::Damage ,"Damage"},
		{ CharaState::BlowIn ,"BlowIn"},
		{ CharaState::Blowing ,"Blowing"},
		{ CharaState::BlowOut ,"BlowOut"},
		{ CharaState::DownLoop ,"DownLoop"}
};

std::map<int, const CharaBase *const> CharaBase::CharaArrey;

CharaBase::CharaBase() {
	state = CharaState::StateNum;
}

CharaBase::CharaBase(const CharaBase& other) : AnimModel(other) {
	state = CharaState::StateNum;
	CharaArrey.insert(std::map<int, CharaBase*>::value_type(handle, this));
}

CharaBase::~CharaBase() {
	CharaArrey.erase(handle);
}

void CharaBase::LoadModel(const char *const Path) {
	Model::LoadModel(Path);
	CharaArrey.insert(std::map<int, CharaBase*>::value_type(handle, this));
}

Model& CharaBase::DupModel(const Model& other) {
	Model::DupModel(other);
	CharaArrey.insert(std::map<int, CharaBase*>::value_type(handle, this));

	return *this;
}

void CharaBase::UnLoadModel() {
	CharaArrey.erase(handle);
	Model::UnLoadModel();
}

void CharaBase::SetSpeed(const VECTOR Speed) {
	speed = Speed;
}

void CharaBase::SetJumppow(const float SetParam) {
	jumppow = SetParam;
}

void CharaBase::SetDirection(const VECTOR NewDirection) {
	direction = NewDirection;
}

void CharaBase::SetRespawnPos(const VECTOR SetPos) {
	respawnpos = SetPos;
}

/*void CharaBase::SetCollCap(const VECTOR PosD, const VECTOR PosU, const float R) {
	collCap = Capsule(PosD, PosU, R);
}*/

void CharaBase::SetCollCap(const float Height, const float R) {
	collCap.posD = pos.cYadd(R);
	collCap.posU = pos.cYadd(R + Height);
	collCap.r = R;
	collCap.height = Height;
}

void CharaBase::UpdateCollCap() {
	collCap.posD = pos.cYadd(collCap.r);
	collCap.posU = pos.cYadd(collCap.r + collCap.height);
}

void CharaBase::SetCoverbox(const VECTOR VEC) {
	coverbox.x = fabs(VEC.x * scale.x);
	coverbox.y = fabs(VEC.y * scale.y);
	coverbox.z = fabs(VEC.z * scale.z);
	coverbox.Pos1 = pos - VGet(coverbox.x, 0, coverbox.z);
	coverbox.Pos2 = pos + VGet(coverbox.x, coverbox.y, coverbox.z);
}

void CharaBase::SetCoverbox(const VECTOR Pos1, const VECTOR Pos2) {
	coverbox.x = fabs(Pos1.x - Pos2.x) / 2.0f;
	coverbox.y = fabs(Pos1.y - Pos2.y) / 2.0f;
	coverbox.z = fabs(Pos1.z - Pos2.z) / 2.0f;
	coverbox.Pos1 = Pos1;
	coverbox.Pos2 = Pos2;
}

void CharaBase::SetCoverbox(const float x, const float y, const float z) {
	coverbox.x = fabs(x * scale.x);
	coverbox.y = fabs(y * scale.y);
	coverbox.z = fabs(z * scale.z);
	coverbox.Pos1 = pos - VGet(coverbox.x, 0, coverbox.z);
	coverbox.Pos2 = pos + VGet(coverbox.x, coverbox.y, coverbox.z);
}

void CharaBase::UpdateCoverbox() {
	coverbox.Pos1 = pos - VGet(coverbox.x, 0, coverbox.z);
	coverbox.Pos2 = pos + VGet(coverbox.x, coverbox.y, coverbox.z);
}

void CharaBase::Coll_Chara_vs_Chara(const CharaBase& vsCheckChara) {
	VECTOR ChkChToChVec;
	VECTOR PushVec;
	VECTOR ChPosition;
	float  Distance;
	float  TempY;

	// �L�������m�̕��̍��v���Z�o
	const float TotalWidth = collCap.r + vsCheckChara.collCap.r;

	// �ړ���̃L���� CInfo �̍��W���Z�o
	ChPosition = pos + speed;

	// �������Ă��邩����
	if (collCap.HitCheck_Capsule(vsCheckChara.collCap)) {
		// �������Ă����� CInfo �� CheckCInfo ���痣��鏈��������

		// CheckCInfo ���� CInfo �ւ̃x�N�g�����Z�o
		ChkChToChVec = pos - vsCheckChara.pos;

		// �x���͌��Ȃ�
		ChkChToChVec.y = 0.0f;

		// ��l�̋������Z�o
		Distance = VSize(ChkChToChVec);

		// CheckCInfo ���� CInfo �ւ̃x�N�g���𐳋K��
		PushVec = VNorm(ChkChToChVec);

		//const int PushNum = (int)floorf(TotalWidth / Distance);

		// �����o���������Z�o�A������l�̋��������l�̑傫�����������l�ɉ����o���͂�
		// �����ė���Ă��܂��ꍇ�́A�҂����肭���������Ɉړ�����
		if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f) {
			TempY = ChPosition.y;
			ChPosition = vsCheckChara.pos + PushVec * TotalWidth;

			// �x���W�͕ω������Ȃ�
			ChPosition.y = TempY;
		}
		else {
			// �����o��
			ChPosition += PushVec * CHARA_HIT_PUSH_POWER;
		}
		/*if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f) {
			TempY = ChPosition.y;
			ChPosition = vsCheckChara.pos + PushVec * TotalWidth;

			// �x���W�͕ω������Ȃ�
			ChPosition.y = TempY;
		}
		else {
			for (int i = 0; i == PushNum; ++i) {
				ChPosition += PushVec;
			}
		}*/

	}

	//if (Distance == TotalWidth) return;
	// �����蔻�菈����̈ړ��x�N�g�����Z�b�g
	/*CharaBase* thistmp = const_cast<CharaBase*>(this);
	thistmp->SetSpeed(ChPosition - pos);*/
	SetSpeed(ChPosition - pos);
}

void CharaBase::MoveProcess() {
	int i, j, k;					// �ėp�J�E���^�ϐ�
	bool MoveFlag;					// ���������Ɉړ��������ǂ����̃t���O( �O:�ړ����Ă��Ȃ�  �P:�ړ����� )
	bool HitFlag;					// �|���S���ɓ����������ǂ������L�����Ă����̂Ɏg���ϐ�( �O:�������Ă��Ȃ�  �P:�������� )
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];	// �v���C���[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	int HitDimNum;					// HitDim �̗L���Ȕz��v�f��
	int KabeNum;					// �ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	int YukaNum;					// ���|���S���Ɣ��f���ꂽ�|���S���̐�
	MV1_COLL_RESULT_POLY *Kabe;		// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��̐擪�A�h���X
	MV1_COLL_RESULT_POLY *Yuka;		// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��̐擪�A�h���X
	VECTOR OldPos;					// �ړ��O�̍��W
	Vector NowPos;					// �ړ���̍��W
	Triangle PolyTri;


	for (auto it = CharaArrey.cbegin(); it != CharaArrey.cend(); ++it) {
		if (it->first == handle) continue;	//�������g�Ƃ̓����蔻��͂��Ȃ�
		if (VSize(pos - it->second->pos) > 10.0f*scale.y + speed.size()) continue;	//�߂��̓G�Ƃ��������蔻��

		Coll_Chara_vs_Chara(*it->second);
	}


	extern StageModel Stage;

	Kabe = new MV1_COLL_RESULT_POLY[CHARA_MAX_HITCOLL];
	Yuka = new MV1_COLL_RESULT_POLY[CHARA_MAX_HITCOLL];

	const float charaheight = collCap.height + collCap.r*2.0f;
	
	// �ړ��O�̍��W��ۑ�
	OldPos = pos;

	extern unsigned int KeyFrame[256];
	
	speed.y -= GRAVITY;

	// �������x���
	if (speed.y < CHARA_MAX_FALLSPEED) {
		speed.y = CHARA_MAX_FALLSPEED;
	}

	// �ړ���̍��W���Z�o
	NowPos = pos + speed;

	/*NowCap = collCap;
	NowCap.moveby(speed);*/

	// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
	// ( ���o����͈͈͂ړ��������l������ )
	HitDim[0] = Stage.CollCheck_Sphere(pos, 10.0f*scale.y + speed.size());
	
	// �v���C���[�̎��͂ɂ���R���W�����I�u�W�F�N�g�̃|���S�����擾����
	for (i = 0; i < Stage.CollObjectNum; ++i) {
		HitDim[i + 1] = Stage.CollObject[i]->CollCheck_Sphere(pos, 10.0f*scale.y + speed.size());
	}

	// HitDim �̗L���Ȑ��̓R���W�����I�u�W�F�N�g�̐��ƃX�e�[�W���̂̃R���W����
	HitDimNum = Stage.CollObjectNum + 1;

	// x����z�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
	if (fabs(speed.x) > 0.01f || fabs(speed.z) > 0.01f) {
		MoveFlag = true;
	}
	else {
		MoveFlag = false;
	}

	// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
	{
		// �ǃ|���S���Ə��|���S���̐�������������
		KabeNum = 0;
		YukaNum = 0;

		// ���o���ꂽ�|���S���̐������J��Ԃ�
		for (j = 0; j < HitDimNum; ++j) {
			for (i = 0; i < HitDim[j].HitNum; ++i) {
				// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
				//if (fabs(HitDim[j].Dim[i].Normal.y) < 0.000001f) {
				if (fabs(HitDim[j].Dim[i].Normal.y) < 0.5f) {	//45�x�����Ȃ��
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (KabeNum < CHARA_MAX_HITCOLL) {
						// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W��菭�������|���S���̂ݓ����蔻����s��
						if (HitDim[j].Dim[i].Position[0].y > pos.y + getoverheight ||
							HitDim[j].Dim[i].Position[1].y > pos.y + getoverheight ||
							HitDim[j].Dim[i].Position[2].y > pos.y + getoverheight) {

							// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
							Kabe[KabeNum] = HitDim[j].Dim[i];

							// �ǃ|���S���̐������Z����
							++KabeNum;
						}
					}
				}
				else {
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (YukaNum < CHARA_MAX_HITCOLL) {
						// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
						Yuka[YukaNum] = HitDim[j].Dim[i];

						// ���|���S���̐������Z����
						++YukaNum;
					}
				}
			}
		}
	}

	// ���o�����v���C���[�̎��͂̃|���S�������J������
	for (i = 0; i < HitDimNum; ++i) {
		MV1CollResultPolyDimTerminate(HitDim[i]);
	}

	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (KabeNum != 0) {
		// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
		HitFlag = false;

		// �ړ��������ǂ����ŏ����𕪊�
		if (MoveFlag) {
			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < KabeNum; ++i) {
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				
				//PolyTri.GetPolyTri(*Poly);
				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (!PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) continue;

				// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = true;

				// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
				{
					VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(speed, Kabe[i].Normal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
					// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
					SlideVec = VCross(Kabe[i].Normal, SlideVec);

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					NowPos = OldPos + SlideVec;
				}

				// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
				for (j = 0; j < KabeNum; ++j) {
					// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					// �������Ă����烋�[�v���甲����
					if (PolyTri.SetPolyTri(Kabe[j]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) break;
				}

				// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				// �ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == KabeNum) {
					HitFlag = false;
					break;
				}
			}
		}
		else {
			// �ړ����Ă��Ȃ��ꍇ�̏���

			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < KabeNum; ++i) {
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
				if (PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) {
					HitFlag = true;
					break;
				}
			}
		}

		// �ǂɓ������Ă�����ǂ��牟���o���������s��
		if (HitFlag) {
			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
			for (k = 0; k < CHARA_HIT_TRYNUM; ++k) {
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < KabeNum; ++i) {
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					
					// �v���C���[�Ɠ������Ă��邩�𔻒�
					if (!PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) continue;

					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
					NowPos += Kabe[i].Normal;

					// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					for (j = 0; j < KabeNum; ++j) {
						// �������Ă����烋�[�v�𔲂���
						if (PolyTri.SetPolyTri(Kabe[j]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) break;
					}

					// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
					if (j == KabeNum) break;
				}

				// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
				if (i != KabeNum) break;
			}
		}
	}

	delete[] Kabe;

	// ���|���S���Ƃ̓����蔻��
	if (YukaNum != 0) {

		// �W�����v�����㏸���̏ꍇ�͏����𕪊�
		if ((CharaState::Jump <= state && state <= CharaState::Fall) && speed.y > 0.0f) {
			float MinY;
			HITRESULT_LINE LineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
			//LineRes.HitFlag = false;
			//LineRes.Position.y = 0.0f;
			// �V��ɓ����Ԃ��鏈�����s��

			// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
			MinY = 0.0f;

			// �����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���
			HitFlag = false;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < YukaNum; ++i) {
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				
				// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos, NowPos.cYadd(charaheight));

				// �ڐG���Ă��Ȃ������牽�����Ȃ�
				if (!LineRes.HitFlag) continue;

				// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
				if (HitFlag && MinY < LineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MinY = LineRes.Position.y;
			}

			// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
			if (HitFlag) {
				// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
				NowPos.y = MinY - charaheight;

				// �x�������̑��x�͔��]
				speed.y = -speed.y;
			}
		}
		else {
			float MaxY;
			HITRESULT_LINE LineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
			
			// ���~�����W�����v���ł͂Ȃ��ꍇ�̏���

			// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			HitFlag = false;

			// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			MaxY = 0.0f;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < YukaNum; ++i) {
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				
				// �W�����v�����ǂ����ŏ����𕪊�
				if (CharaState::Jump <= state && state <= CharaState::Fall) {
					// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
					LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos.cYadd(charaheight), NowPos.cYadd(-1.0f*scale.y));
				}
				else {
					// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
					LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos.cYadd(charaheight), NowPos.cYadd(-charaheight));
				}

				// �������Ă��Ȃ������牽�����Ȃ�
				if (!LineRes.HitFlag) continue;

				// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (HitFlag && MaxY > LineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MaxY = LineRes.Position.y;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (HitFlag) {
				// ���������ꍇ

				// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				NowPos.y = MaxY;

				// �x�������̈ړ����x�͂O��
				speed.y = 0.0f;

				// �����������������ꍇ�͒��n��Ԃɂ���
				if (CharaState::Fall <= state && state <= CharaState::Falling) {
					// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
					if (MoveFlag) {
						if (KeyFrame[KEY_INPUT_LSHIFT] > 1) {
							SetState(CharaState::Run);
						}
						else {
							SetState(CharaState::Walk);
						}
					}
					else {
						// �ړ����Ă��Ȃ��ꍇ�͒��n���[�V������Ԃ�
						SetState(CharaState::Landing);
					}

					// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
					AnimBlendRate = 1.0f;

					MV1ResetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "�Z���^�["));
				}
			}
			else {
				// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
				if (!(CharaState::JumpPreparation <= state && state <= CharaState::Fall && state != CharaState::Falling)) {
					// �������ɂ���
					SetState(CharaState::Fall);

					MV1SetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "�Z���^�["), MGetTranslate(VGet(0.0f, AdjustAnimPos, 0.0f)));
				}
				/*else if (state != CharaState::Falling) {
					SetState(CharaState::Falling);

					MV1SetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "�Z���^�["), MGetTranslate(VGet(0.0f, AdjustAnimPos, 0.0f)));
				}*/
			}
		}
	}

	delete[] Yuka;

	// �V�������W��ۑ����違�v���C���[�̃��f���̍��W���X�V����
	SetPos(NowPos);
}

void CharaBase::drawCollCap() {
	SetMaterialParam(DefaultMaterial);

	DrawCapsule3D(collCap.posD, collCap.posU, collCap.r, 12, DxColor::Black, DxColor::Black, FALSE);
	DrawCube3D(pos + VGet(collCap.r, getoverheight, collCap.r), pos + VGet(-collCap.r, getoverheight, -collCap.r), DxColor::Black, DxColor::Black, FALSE);
}

void CharaBase::drawCoverBox() {
	SetMaterialParam(DefaultMaterial);

	DrawCube3D(coverbox.Pos1, coverbox.Pos2, DxColor::Black, DxColor::Black, FALSE);
}