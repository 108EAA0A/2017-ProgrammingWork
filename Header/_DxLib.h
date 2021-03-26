#pragma once

#include "DxLib.h"

namespace DxColor {
	static const unsigned int Black		= GetColor(0x00, 0x00, 0x00);
	static const unsigned int White		= GetColor(0xFF, 0xFF, 0xFF);
	static const unsigned int Red		= GetColor(0xFF, 0x00, 0x00);
	static const unsigned int Green		= GetColor(0x00, 0xFF, 0x00);
	static const unsigned int Blue		= GetColor(0x00, 0x00, 0xFF);
	static const unsigned int Yellow	= GetColor(0xFF, 0xFF, 0x00);
	static const unsigned int Purple	= GetColor(0xFF, 0x00, 0xFF);
	static const unsigned int Cyan		= GetColor(0x00, 0xFF, 0xFF);
}

static const MATERIALPARAM DefaultMaterial = {
	GetColorF(0.0f, 0.0f, 0.0f, 1.0f),
	GetColorF(0.0f, 0.0f, 0.0f, 0.0f),
	GetColorF(0.0f, 0.0f, 0.0f, 0.0f),
	GetColorF(0.0f, 0.0f, 0.0f, 0.0f),
	0.0f
};

inline FLOAT4 operator+  (const FLOAT4 FL4_1, const FLOAT4 FL4_2) { return F4Add(FL4_1, FL4_2); }
inline FLOAT4 operator+= (const FLOAT4 FL4_1, const FLOAT4 FL4_2) { return F4Add(FL4_1, FL4_2); }
inline FLOAT4 operator-  (const FLOAT4 FL4_1, const FLOAT4 FL4_2) { return F4Sub(FL4_1, FL4_2); }
inline FLOAT4 operator-= (const FLOAT4 FL4_1, const FLOAT4 FL4_2) { return F4Sub(FL4_1, FL4_2); }

inline VECTOR operator+	 (const VECTOR VEC1, const VECTOR VEC2) { return VAdd(VEC1, VEC2); }
inline VECTOR operator+= (const VECTOR VEC1, const VECTOR VEC2) { return VAdd(VEC1, VEC2); }
inline VECTOR operator-	 (const VECTOR VEC1, const VECTOR VEC2) { return VSub(VEC1, VEC2); }
inline VECTOR operator-= (const VECTOR VEC1, const VECTOR VEC2) { return VSub(VEC1, VEC2); }

// 内積外積ではなくスケーリング用
inline VECTOR operator*	 (const VECTOR VEC, const float FL)	{ return VScale(VEC, FL); }
inline VECTOR operator*	 (const VECTOR VEC1, const VECTOR VEC2)	{ return VGet(VEC1.x*VEC2.x, VEC1.y*VEC2.y, VEC1.z*VEC2.z); }
inline VECTOR operator*= (const VECTOR VEC, const float FL)	{ return VScale(VEC, FL); }
inline VECTOR operator*= (const VECTOR VEC1, const VECTOR VEC2)	{ return VGet(VEC1.x*VEC2.x, VEC1.y*VEC2.y, VEC1.z*VEC2.z); }

inline bool operator== (const VECTOR VEC1, const VECTOR VEC2) { return ((VEC1.x == VEC2.x) && (VEC1.y == VEC2.y) && (VEC1.z == VEC2.z)); }
inline bool operator!= (const VECTOR VEC1, const VECTOR VEC2) { return ((VEC1.x != VEC2.x) && (VEC1.y != VEC2.y) && (VEC1.z != VEC2.z)); }
inline bool operator<  (const VECTOR VEC1, const VECTOR VEC2) { return (VSquareSize(VEC1) <  VSquareSize(VEC2)); }
inline bool operator<= (const VECTOR VEC1, const VECTOR VEC2) { return (VSquareSize(VEC1) <  VSquareSize(VEC2)); }
inline bool operator>= (const VECTOR VEC1, const VECTOR VEC2) { return (VSquareSize(VEC1) >= VSquareSize(VEC2)); }
inline bool operator>  (const VECTOR VEC1, const VECTOR VEC2) { return (VSquareSize(VEC1) >= VSquareSize(VEC2)); }

struct Vector {
	float x, y, z;

	Vector();
	Vector(const float X, const float Y, const float Z);
	Vector(const VECTOR VEC);
	~Vector() {}

	Vector& operator=(const float F);
	//inline operator float() const { return VSize(VGet(x, y, z)); }

	//DxLibのVECTORとしても扱えるようにする
	operator VECTOR() const;
	bool operator==(VECTOR VEC) const;

	float size() const;

	Vector cXadd(const float addX) const;
	Vector cYadd(const float addY) const;
	Vector cZadd(const float addZ) const;

};

/* 線分雛形 */
typedef struct _Line {
	VECTOR StartPos, EndPos;
}Line;

/* 箱雛形 */
typedef struct _Cube {
	float x, y, z;
	VECTOR Pos1, Pos2;
}Cube;

/* 球体雛形 */
typedef struct _Sphere {
	VECTOR SphPos;
	float SphR;
}Sphere;

/* カプセル雛形 */
struct Capsule {
	Capsule() {}
	Capsule(const VECTOR PosD, const VECTOR PosU, const float R);
	
	VECTOR posD, posU;
	float r;
	float height;
	
	void moveby(const VECTOR VEC);
	bool HitCheck_Triangle(const VECTOR TriPos1, const VECTOR TriPos2, const VECTOR TriPos3) const;
	bool HitCheck_Capsule(const Capsule& vsCap) const;

	static Capsule CapGet(const VECTOR Pos1, const VECTOR Pos2, const float r);
};

/* カプセル雛形 */
struct Triangle {
	Triangle() {}
	Triangle(const VECTOR Pos1, const VECTOR Pos2, const VECTOR Pos3);

	//Triangle& operator()(const VECTOR Pos1, const VECTOR Pos2, const VECTOR Pos3);

	VECTOR pos[3];
	
	Triangle& SetPolyTri(const MV1_COLL_RESULT_POLY& Poly);
	bool HitCheck_Capsule(const VECTOR Pos1, const VECTOR Pos2, const float r) const;
	bool HitCheck_Capsule(const Capsule& vsCap) const;

	HITRESULT_LINE CollCheck_Line(const VECTOR Pos1, const VECTOR Pos2) const;

	static Triangle TriGet(const VECTOR Pos1, const VECTOR Pos2, const VECTOR Pos3);
};
