#include "_DxLib.h"

Vector::Vector() {
	x = y = z = 0;
}

Vector::Vector(const float X, const float Y, const float Z) {
	x = X;
	y = Y;
	z = Z;
}

Vector::Vector(const VECTOR VEC) {
	x = VEC.x;
	y = VEC.y;
	z = VEC.z;
}
	
Vector& Vector::operator=(const float F) {
	x = y = z = F;
	return *this;
}
//inline operator float() const { return VSize(VGet(x, y, z)); }

//DxLib‚ÌVECTOR‚Æ‚µ‚Ä‚àˆµ‚¦‚é‚æ‚¤‚É‚·‚é
Vector::operator VECTOR() const { return VGet(x, y, z); }
bool Vector::operator==(VECTOR VEC) const { return (VEC == VGet(x, y, z)); }

float Vector::size() const {
	return VSize(VGet(x, y, z));
}

Vector Vector::cXadd(const float addX) const {
	return { x + addX, y, z };
}

Vector Vector::cYadd(const float addY) const {
	return { x, y + addY, z };
}

Vector Vector::cZadd(const float addZ) const {
	return { x, y, z + addZ };
}

Capsule::Capsule(const VECTOR PosD, const VECTOR PosU, const float R) {
	posD = PosD;
	posU = PosU;
	r = R;
	height = 2.0f*R + PosU.y - PosD.y;
}

void Capsule::moveby(const VECTOR VEC) {
	posD += VEC;
	posU += VEC;
}

bool Capsule::HitCheck_Triangle(const VECTOR TriPos1, const VECTOR TriPos2, const VECTOR TriPos3) const {
	return (HitCheck_Capsule_Triangle(posD, posU, r, TriPos1, TriPos2, TriPos3) == TRUE);
}

bool Capsule::HitCheck_Capsule(const Capsule& vsCap) const {
	return (Segment_Segment_MinLength(posD, posU, vsCap.posD, vsCap.posU) <= (r + vsCap.r));
}

Capsule Capsule::CapGet(const VECTOR Pos1, const VECTOR Pos2, const float r) {
	return { Pos1, Pos2 , r };
}

Triangle::Triangle(const VECTOR Pos1, const VECTOR Pos2, const VECTOR Pos3) {
	pos[0] = Pos1;
	pos[1] = Pos2;
	pos[2] = Pos3;
}

Triangle& Triangle::SetPolyTri(const MV1_COLL_RESULT_POLY& Poly) {
	pos[0] = Poly.Position[0];
	pos[1] = Poly.Position[1];
	pos[2] = Poly.Position[2];

	return (*this);
}

bool Triangle::HitCheck_Capsule(const VECTOR Pos1, const VECTOR Pos2, const float r) const {
	return (HitCheck_Capsule_Triangle(Pos1, Pos2, r, pos[0], pos[1], pos[2]) == TRUE);
}

bool Triangle::HitCheck_Capsule(const Capsule& vsCap) const {
	return (HitCheck_Capsule_Triangle(vsCap.posD, vsCap.posU, vsCap.r, pos[0], pos[1], pos[2]) == TRUE);
}

HITRESULT_LINE Triangle::CollCheck_Line(const VECTOR Pos1, const VECTOR Pos2) const {
	return HitCheck_Line_Triangle(Pos1, Pos2, pos[0], pos[1], pos[2]);
}

Triangle Triangle::TriGet(const VECTOR Pos1, const VECTOR Pos2, const VECTOR Pos3) {
	return { Pos1, Pos2, Pos3 };
}