#ifndef LAMPLIGHTER_RENDER_H
#define LAMPLIGHTER_RENDER_H

#include <math.h>

// (LAMPLIGHTER RENDER)

// oof you just posted triange
// you are going to lose millisecond

namespace lr {

	#if defined __GNUC__
	// Signed integers
	typedef signed char li8;
	typedef signed short int li16;
	typedef signed int li32;
	typedef signed long long int li64;
	// Unsigned integers
	typedef unsigned char lui8;
	typedef unsigned short int lui16;
	typedef unsigned int lui32;
	typedef unsigned long long lui64;
	// Floating points
	typedef float lf32;
	typedef double lf64;
	typedef long double lf96;
	// For specific uses
	typedef unsigned short int lID;
	#elif defined _MSC_VER
	// Signed integers
	typedef signed __int8 li8;
	typedef signed __int16 li16;
	typedef signed __int32 li32;
	typedef signed __int64 li64;
	// Unsigned integers
	typedef unsigned __int8 lui8;
	typedef unsigned __int16 lui16;
	typedef unsigned __int32 lui32;
	typedef unsigned __int64 lui64;
	// Floating points
	typedef float lf32;
	typedef double lf64;
	#else
	#error Using different c++ compiler than written for, please review! (ltrrender.h)
	#endif

	#define LR_RENDER_W 400
	#define LR_RENDER_H 300

	// turn on to debug display overdraw

	#define LR_FILL_MODE_COVERAGE 0
	#define LR_FILL_MODE_WIREFRAME 1
	#define LR_FILL_MODE_SOLID 2
	#define LR_FILL_MODE LR_FILL_MODE_SOLID

	#define LR_FORCE_CLEAR 1
	#define LR_DITHER 0
	#define LR_POSTERIZE 0
	#define LR_DEBUG_HELP 0
	#define LR_PERSP_CORRECT_UV 1
	#define LR_BILINEAR 0

	#define LR_MODE_INT_BHM 0
	#define LR_MODE_FLO_LERP 1

	#define LR_MODE LR_MODE_FLO_LERP

	struct LRVec2 {
		lf32 x, y;
		LRVec2(lf32 X = 0.f, lf32 Y = 0.f) : x{ X }, y{ Y } {};
	};
	struct LRVec3 {
		lf32 x, y, z;
		LRVec3() : x{ 0.f }, y{ 0.f }, z{ 0.f } {};
		LRVec3(lf32 X) : x{ X }, y{ X }, z{ X } {};
		LRVec3(lf32 X, lf32 Y, lf32 Z) : x{ X }, y{ Y }, z{ Z } {};
	};
	struct LRCol {
		lui8 r, g, b, a;
		LRCol(lui8 x = 0u, lui8 y = 0u, lui8 z = 0u, lui8 w = 0u) : r{ x }, g{ y }, b{ z }, a{ w } {}
	};

	LRVec2 operator-(const LRVec2& a, const LRVec2& b);
	LRVec3 operator*(const LRVec3& a, const LRVec3& b);
	LRVec3 operator*(const lf32& f, const LRVec3& v);
	LRVec3 operator+(const LRVec3& a, const LRVec3& b);

	// Vertex used to render a single triangle
	struct LRVert {
		#if LR_MODE == LR_MODE_FLO_LERP
		lf32 pos_x, pos_y, pos_z, pos_w;
		#else
		li32 pos_x, pos_y, pos_z;
		#endif
		LRVec2 uv;
		LRVec3 col;
		LRVert(LRVec3 _pos, LRVec2 _uv, LRVec3 _col, lf32 w)
		{
			#if LR_MODE == LR_MODE_FLO_LERP
			pos_x = _pos.x;
			pos_y = _pos.y;
			pos_z = _pos.z;
			pos_w = w;
			#else
			pos_x = (li32)roundf(_pos.x);
			pos_y = (li32)roundf(_pos.y);
			pos_z = (li32)roundf(_pos.z);
			#endif
			uv.x = _uv.x;
			uv.y = _uv.y;
			col.x = _col.x;
			col.y = _col.y;
			col.z = _col.z;
		}
	};

	//extern inline void DoThing() {
	//	int i = 0;
	//}

	void LRClear();

	void LRDrawTxtr(li32 X, li32 Y, void* tx, lui32 width, lui32 height);

	void LRDrawTri(LRVert point_a, LRVert point_b, LRVert point_c, void* txtr, li32 tw, li32 th);

	void LRSetBufferTemp(void* colour, void* depth);
}

#endif // END OF FILE
