#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"
#include "env.h"
#include <vector>

#include "Transform.h"
#include "maths.hpp"

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

// Some test stuff

PACKED_STRUCT(MyStruct3{
btui32 a;
btui8 c;
btui32 b;
});

struct EntityMkII {
	// Contains the data of this entity
	void* component;
	// Return a string which will be printed to the screen when this entity is looked at
	char*(*Name)(void* self);
	// Tick this entity
	void(*Tick)(btID id, void* self, btf32 dt);
	// Render graphics of this entity
	void(*Draw)(btID id, void* self);
	// Read / write
	void(*RFile)(btID id, void* self, FILE* file, btui32 version);
	void(*WFile)(btID id, void* self, FILE* file, btui32 version);
	// initialize function pointers
	template <typename ComponentType> void Init(
		char*(*_Name)(void*),
		void(*_Tick)(btID, void*, btf32),
		void(*_Draw)(btID, void*),
		void(*_RFile)(btID id, void* self, FILE* file, btui32 version),
		void(*_WFile)(btID id, void* self, FILE* file, btui32 version)) {
		component = new ComponentType;
		Name = _Name;
		Tick = _Tick;
		Draw = _Draw;
		RFile = _RFile;
		WFile = _WFile;
	}
	// Free from memory
	void End() {
		delete component;
	}
};





struct EntityComponent {
	virtual char* Name() {};
	virtual void Tick() {};
	virtual void Draw() {};
	virtual void RFile(FILE* file, btui32 version) {};
	virtual void WFile(FILE* file, btui32 version) {};
};

struct ECTest : EntityComponent {
	char* Name() {
	};
	void Tick() {
		int i = 0;
	};
	void Draw() {
	};
};

struct EntityMkIII {
	// Contains the data of this entity
	EntityComponent* component;	
	template <typename ComponentType> void Init() {
		component = new ComponentType;
	}
	void End() {
		delete component;
	}
};

void MakeEntity(EntityType type) {
	EntityMkIII entity;
	switch (type)
	{
	case ENTITY_TYPE_ACTOR:
		entity.Init<ECTest>();
		break;
	}
}

void InitTest() {
	ECTest ec;
	ec.Tick();
}
