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

// Some test stuff

///*



//__declspec(align(8))
struct myStruct2
{
	btui32 a;
	btui8 c;
	btui32 b;
};

PACKED_STRUCT(MyStruct3{
btui32 a;
btui8 c;
btui32 b;
});



//*/

// cursed shit but it works

/*

//typedef btui8 Name[4];


#define PACKED_STRUCT2(NAME, SIZE, VARS) struct NAME { btui8 bytes[SIZE]; VARS }
#define PACKED_VARIABLE2X(TYPE, NAME, OFFSET) __forceinline TYPE& NAME() { return (TYPE&)(bytes[OFFSET]); }
#define PACKED_VARIABLE2(TYPE, NAMEGET, NAMESET, OFFSET) \
	TYPE NAMEGET() { return (TYPE)(bytes[OFFSET]); } \
	void NAMESET(TYPE x) { *(TYPE*)(&bytes[OFFSET]) = x; }

PACKED_STRUCT2(Ps1, 9,
	PACKED_VARIABLE2(btui32, GetInt0, SetInt0, 0)
	PACKED_VARIABLE2(btui8, GetInt1, SetInt1, 4)
	PACKED_VARIABLE2(btui32, GetInt2, SetInt2, 5)
);

#define PACKED_STRUCT3(NAME, SIZE, ...) struct NAME { btui8 bytes[SIZE]; enum e { __VA_ARGS__ }; }
#define PACKED_GET(STRUCT, TYPE, OFFSET) ((TYPE)(STRUCT.bytes[OFFSET]))

PACKED_STRUCT3(PackedStruct2, 9, BYTE_0, BYTE_1 = 4, BYTE_2 = 5, );


#define PACKED_STRUCT4(SIZE, NAME) typedef btui8 NAME[SIZE];

PACKED_STRUCT4(8, PackedStruct4);


//*/

void InitTest()
{
	/*
	

	Ps1 stru;

	PackedStruct2 stru2;
	memset(stru2.bytes, 0, 8);

	for (int i = 0; i < 100000000; ++i)
		++PACKED_GET(stru2, btui8, PackedStruct2::BYTE_1);
	
	for (int i = 0; i < 100000000; ++i)
		stru.SetInt1(stru.GetInt1() + 1);

	int i = sizeof(PackedStruct2);
	int i4 = sizeof(Ps1);
	int i2 = sizeof(myStruct2);
	int i3 = sizeof(MyStruct3);
	//*/
	
	//path::Path vec;
	//path::PathFind(&vec, 1024, 1024, 1026, 1026);

}
