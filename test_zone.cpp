#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"
#include "env.h"
#include <vector>

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

#define STOREBLOCK_SIZE 1024
typedef struct StoreBlock {
	btui8 bytes[STOREBLOCK_SIZE];
	bool byte_used[STOREBLOCK_SIZE];
} StoreBlock;
typedef struct StoreBlockAddr {
	btui16 addr;
	btui16 size;
} StoreBlockAddr;
void StoreBlockAdd(StoreBlock* block, StoreBlockAddr* out_addr, btui16 size)
{
	
}

struct teststr1
{
	int i;
	int i2;
};

struct teststr2
{
	int i;
};

void InitTest()
{
	std::vector<path::PathNode> vec;
	path::PathFind(&vec, 1024, 1024, 1026, 1026);

	//StoreBlock store;

	//StoreBlockAddr addr[16];


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
