#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"


class testFixedPoint
{
public:
	testFixedPoint()
	{
	
	};
	~testFixedPoint()
	{
	
	};


private:
	bti64 val;
};

#define FPSCALE32    0b00000000000000010000000000000000
#define FPINTEGRAL32 0b11111111111111110000000000000000
#define FPDECIMAL32  0b00000000000000001111111111111111
//#define FPSCALE32 (1 << 16)

typedef struct FixedPointU32 {
private:
	bti32 val;
public:
	void SetF(btf64 f)
	{
		val = (bti32)(f * (btf64)FPSCALE32);
	}
	btf64 GetF()
	{
		return (btf64)val / (btf64)FPSCALE32;
	}
	bti32 Get()
	{
		return val;
	}
	bti32 GetIntegral()
	{
		return (val & FPINTEGRAL32) / FPSCALE32;
	}
	bti32 GetDecimal()
	{
		return val & FPDECIMAL32;
	}
	bool operator==(FixedPointU32& other)
	{
		return val == other.Get();
	}
	void operator=(FixedPointU32& other)
	{
		val = other.Get();
	}
	void operator+(FixedPointU32& other)
	{
		val = val + other.Get();
	}
	void operator-(FixedPointU32& other)
	{
		val = val - other.Get();
	}
	void operator%(FixedPointU32& other)
	{
		val = val % other.Get();
	}
	void operator*(FixedPointU32& other)
	{
		val = ((int64_t)val * (int64_t)other.Get()) / FPSCALE32;
	}
	void operator/(FixedPointU32& other)
	{
		val = ((int64_t)val * (1 << 16)) / other.Get();
	}
} FixedPointU32;

void InitTest()
{
	btf32 float_determinism_test_1 = ((8.586f + 1.2345f) + 4.4233f);
	btf32 float_determinism_test_2 = (8.586f + (1.2345f + 4.4233f));

	btui32 i;

	// Test things here
	btf32 f = INFINITY;

	//testFixedPoint fp;
	FixedPointU32 fp;
	fp.SetF(6.24);

	bti32 i1 = fp.GetIntegral();
	bti32 i2 = fp.GetDecimal();
	btf64 ff = (btf64)i2 / (btf64)FPSCALE32;

	FixedPointU32 fp2;
	fp2.SetF(6.f + ff);

	int dddd = 0;
	//fp.mult = 1000;
}

struct p_settarget
{
	btui8 id;
	btui16 px;
	btui16 py;
};

// Maximum IPv4 size: 576 bytes

struct packet // can set pose of so many units in one packet
{
	btui8 msgtype;
	btui8 spcount;
	p_settarget sp[95];
};

size_t sz = sizeof(p_settarget);
size_t sz2 = sizeof(packet);