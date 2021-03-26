#include "CharaBase.h"

#include "StageModel.h"

#define GRAVITY					0.049f
#define CHARA_MAX_FALLSPEED		-100.0f		// 最大落下速度
#define CHARA_MAX_HITCOLL		2048		// 処理するコリジョンポリゴンの最大数
#define CHARA_HIT_TRYNUM		16			// 壁押し出し処理の最大試行回数
#define CHARA_HIT_PUSH_POWER	5.0f		// キャラクター同士で当たったときの押し出される力

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

	// キャラ同士の幅の合計を算出
	const float TotalWidth = collCap.r + vsCheckChara.collCap.r;

	// 移動後のキャラ CInfo の座標を算出
	ChPosition = pos + speed;

	// 当たっているか判定
	if (collCap.HitCheck_Capsule(vsCheckChara.collCap)) {
		// 当たっていたら CInfo が CheckCInfo から離れる処理をする

		// CheckCInfo から CInfo へのベクトルを算出
		ChkChToChVec = pos - vsCheckChara.pos;

		// Ｙ軸は見ない
		ChkChToChVec.y = 0.0f;

		// 二人の距離を算出
		Distance = VSize(ChkChToChVec);

		// CheckCInfo から CInfo へのベクトルを正規化
		PushVec = VNorm(ChkChToChVec);

		//const int PushNum = (int)floorf(TotalWidth / Distance);

		// 押し出す距離を算出、もし二人の距離から二人の大きさを引いた値に押し出し力を
		// 足して離れてしまう場合は、ぴったりくっつく距離に移動する
		if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f) {
			TempY = ChPosition.y;
			ChPosition = vsCheckChara.pos + PushVec * TotalWidth;

			// Ｙ座標は変化させない
			ChPosition.y = TempY;
		}
		else {
			// 押し出し
			ChPosition += PushVec * CHARA_HIT_PUSH_POWER;
		}
		/*if (Distance - TotalWidth + CHARA_HIT_PUSH_POWER > 0.0f) {
			TempY = ChPosition.y;
			ChPosition = vsCheckChara.pos + PushVec * TotalWidth;

			// Ｙ座標は変化させない
			ChPosition.y = TempY;
		}
		else {
			for (int i = 0; i == PushNum; ++i) {
				ChPosition += PushVec;
			}
		}*/

	}

	//if (Distance == TotalWidth) return;
	// 当たり判定処理後の移動ベクトルをセット
	/*CharaBase* thistmp = const_cast<CharaBase*>(this);
	thistmp->SetSpeed(ChPosition - pos);*/
	SetSpeed(ChPosition - pos);
}

void CharaBase::MoveProcess() {
	int i, j, k;					// 汎用カウンタ変数
	bool MoveFlag;					// 水平方向に移動したかどうかのフラグ( ０:移動していない  １:移動した )
	bool HitFlag;					// ポリゴンに当たったかどうかを記憶しておくのに使う変数( ０:当たっていない  １:当たった )
	MV1_COLL_RESULT_POLY_DIM HitDim[STAGE_OBJECT_MAX_NUM + 1];	// プレイヤーの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	int HitDimNum;					// HitDim の有効な配列要素数
	int KabeNum;					// 壁ポリゴンと判断されたポリゴンの数
	int YukaNum;					// 床ポリゴンと判断されたポリゴンの数
	MV1_COLL_RESULT_POLY *Kabe;		// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列の先頭アドレス
	MV1_COLL_RESULT_POLY *Yuka;		// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列の先頭アドレス
	VECTOR OldPos;					// 移動前の座標
	Vector NowPos;					// 移動後の座標
	Triangle PolyTri;


	for (auto it = CharaArrey.cbegin(); it != CharaArrey.cend(); ++it) {
		if (it->first == handle) continue;	//自分自身との当たり判定はしない
		if (VSize(pos - it->second->pos) > 10.0f*scale.y + speed.size()) continue;	//近くの敵とだけ当たり判定

		Coll_Chara_vs_Chara(*it->second);
	}


	extern StageModel Stage;

	Kabe = new MV1_COLL_RESULT_POLY[CHARA_MAX_HITCOLL];
	Yuka = new MV1_COLL_RESULT_POLY[CHARA_MAX_HITCOLL];

	const float charaheight = collCap.height + collCap.r*2.0f;
	
	// 移動前の座標を保存
	OldPos = pos;

	extern unsigned int KeyFrame[256];
	
	speed.y -= GRAVITY;

	// 落下速度上限
	if (speed.y < CHARA_MAX_FALLSPEED) {
		speed.y = CHARA_MAX_FALLSPEED;
	}

	// 移動後の座標を算出
	NowPos = pos + speed;

	/*NowCap = collCap;
	NowCap.moveby(speed);*/

	// プレイヤーの周囲にあるステージポリゴンを取得する
	// ( 検出する範囲は移動距離も考慮する )
	HitDim[0] = Stage.CollCheck_Sphere(pos, 10.0f*scale.y + speed.size());
	
	// プレイヤーの周囲にあるコリジョンオブジェクトのポリゴンも取得する
	for (i = 0; i < Stage.CollObjectNum; ++i) {
		HitDim[i + 1] = Stage.CollObject[i]->CollCheck_Sphere(pos, 10.0f*scale.y + speed.size());
	}

	// HitDim の有効な数はコリジョンオブジェクトの数とステージ自体のコリジョン
	HitDimNum = Stage.CollObjectNum + 1;

	// x軸かz軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
	if (fabs(speed.x) > 0.01f || fabs(speed.z) > 0.01f) {
		MoveFlag = true;
	}
	else {
		MoveFlag = false;
	}

	// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
	{
		// 壁ポリゴンと床ポリゴンの数を初期化する
		KabeNum = 0;
		YukaNum = 0;

		// 検出されたポリゴンの数だけ繰り返し
		for (j = 0; j < HitDimNum; ++j) {
			for (i = 0; i < HitDim[j].HitNum; ++i) {
				// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
				//if (fabs(HitDim[j].Dim[i].Normal.y) < 0.000001f) {
				if (fabs(HitDim[j].Dim[i].Normal.y) < 0.5f) {	//45度未満なら壁
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (KabeNum < CHARA_MAX_HITCOLL) {
						// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標より少し高いポリゴンのみ当たり判定を行う
						if (HitDim[j].Dim[i].Position[0].y > pos.y + getoverheight ||
							HitDim[j].Dim[i].Position[1].y > pos.y + getoverheight ||
							HitDim[j].Dim[i].Position[2].y > pos.y + getoverheight) {

							// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
							Kabe[KabeNum] = HitDim[j].Dim[i];

							// 壁ポリゴンの数を加算する
							++KabeNum;
						}
					}
				}
				else {
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (YukaNum < CHARA_MAX_HITCOLL) {
						// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
						Yuka[YukaNum] = HitDim[j].Dim[i];

						// 床ポリゴンの数を加算する
						++YukaNum;
					}
				}
			}
		}
	}

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	for (i = 0; i < HitDimNum; ++i) {
		MV1CollResultPolyDimTerminate(HitDim[i]);
	}

	// 壁ポリゴンとの当たり判定処理
	if (KabeNum != 0) {
		// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
		HitFlag = false;

		// 移動したかどうかで処理を分岐
		if (MoveFlag) {
			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < KabeNum; ++i) {
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				
				//PolyTri.GetPolyTri(*Poly);
				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (!PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) continue;

				// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				HitFlag = true;

				// 壁に当たったら壁に遮られない移動成分分だけ移動する
				{
					VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(speed, Kabe[i].Normal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
					// 元の移動成分から壁方向の移動成分を抜いたベクトル
					SlideVec = VCross(Kabe[i].Normal, SlideVec);

					// それを移動前の座標に足したものを新たな座標とする
					NowPos = OldPos + SlideVec;
				}

				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < KabeNum; ++j) {
					// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					// 当たっていたらループから抜ける
					if (PolyTri.SetPolyTri(Kabe[j]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) break;
				}

				// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
				// 壁に当たったフラグを倒した上でループから抜ける
				if (j == KabeNum) {
					HitFlag = false;
					break;
				}
			}
		}
		else {
			// 移動していない場合の処理

			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < KabeNum; ++i) {
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
				if (PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) {
					HitFlag = true;
					break;
				}
			}
		}

		// 壁に当たっていたら壁から押し出す処理を行う
		if (HitFlag) {
			// 壁からの押し出し処理を試みる最大数だけ繰り返し
			for (k = 0; k < CHARA_HIT_TRYNUM; ++k) {
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < KabeNum; ++i) {
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					
					// プレイヤーと当たっているかを判定
					if (!PolyTri.SetPolyTri(Kabe[i]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) continue;

					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
					NowPos += Kabe[i].Normal;

					// 移動した上で壁ポリゴンと接触しているかどうかを判定
					for (j = 0; j < KabeNum; ++j) {
						// 当たっていたらループを抜ける
						if (PolyTri.SetPolyTri(Kabe[j]).HitCheck_Capsule(NowPos, NowPos.cYadd(charaheight), collCap.r)) break;
					}

					// 全てのポリゴンと当たっていなかったらここでループ終了
					if (j == KabeNum) break;
				}

				// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
				if (i != KabeNum) break;
			}
		}
	}

	delete[] Kabe;

	// 床ポリゴンとの当たり判定
	if (YukaNum != 0) {

		// ジャンプ中且つ上昇中の場合は処理を分岐
		if ((CharaState::Jump <= state && state <= CharaState::Fall) && speed.y > 0.0f) {
			float MinY;
			HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
			//LineRes.HitFlag = false;
			//LineRes.Position.y = 0.0f;
			// 天井に頭をぶつける処理を行う

			// 一番低い天井にぶつける為の判定用変数を初期化
			MinY = 0.0f;

			// 当たったかどうかのフラグを当たっていないを意味する０にしておく
			HitFlag = false;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < YukaNum; ++i) {
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				
				// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
				LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos, NowPos.cYadd(charaheight));

				// 接触していなかったら何もしない
				if (!LineRes.HitFlag) continue;

				// 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
				if (HitFlag && MinY < LineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				HitFlag = true;

				// 接触したＹ座標を保存する
				MinY = LineRes.Position.y;
			}

			// 接触したポリゴンがあったかどうかで処理を分岐
			if (HitFlag) {
				// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
				NowPos.y = MinY - charaheight;

				// Ｙ軸方向の速度は反転
				speed.y = -speed.y;
			}
		}
		else {
			float MaxY;
			HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
			
			// 下降中かジャンプ中ではない場合の処理

			// 床ポリゴンに当たったかどうかのフラグを倒しておく
			HitFlag = false;

			// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
			MaxY = 0.0f;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < YukaNum; ++i) {
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				
				// ジャンプ中かどうかで処理を分岐
				if (CharaState::Jump <= state && state <= CharaState::Fall) {
					// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
					LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos.cYadd(charaheight), NowPos.cYadd(-1.0f*scale.y));
				}
				else {
					// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
					LineRes = PolyTri.SetPolyTri(Yuka[i]).CollCheck_Line(NowPos.cYadd(charaheight), NowPos.cYadd(-charaheight));
				}

				// 当たっていなかったら何もしない
				if (!LineRes.HitFlag) continue;

				// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (HitFlag && MaxY > LineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				HitFlag = true;

				// 接触したＹ座標を保存する
				MaxY = LineRes.Position.y;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (HitFlag) {
				// 当たった場合

				// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
				NowPos.y = MaxY;

				// Ｙ軸方向の移動速度は０に
				speed.y = 0.0f;

				// もし落下中だった場合は着地状態にする
				if (CharaState::Fall <= state && state <= CharaState::Falling) {
					// 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
					if (MoveFlag) {
						if (KeyFrame[KEY_INPUT_LSHIFT] > 1) {
							SetState(CharaState::Run);
						}
						else {
							SetState(CharaState::Walk);
						}
					}
					else {
						// 移動していない場合は着地モーション状態に
						SetState(CharaState::Landing);
					}

					// 着地時はアニメーションのブレンドは行わない
					AnimBlendRate = 1.0f;

					MV1ResetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "センター"));
				}
			}
			else {
				// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
				if (!(CharaState::JumpPreparation <= state && state <= CharaState::Fall && state != CharaState::Falling)) {
					// 落下中にする
					SetState(CharaState::Fall);

					MV1SetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "センター"), MGetTranslate(VGet(0.0f, AdjustAnimPos, 0.0f)));
				}
				/*else if (state != CharaState::Falling) {
					SetState(CharaState::Falling);

					MV1SetFrameUserLocalMatrix(handle, MV1SearchFrame(handle, "センター"), MGetTranslate(VGet(0.0f, AdjustAnimPos, 0.0f)));
				}*/
			}
		}
	}

	delete[] Yuka;

	// 新しい座標を保存する＆プレイヤーのモデルの座標を更新する
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