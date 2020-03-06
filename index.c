#include "index.h"

#define INLINE __forceinline

//________________________________________________________________________________________________________________________________
// TEMP MATHS --------------------------------------------------------------------------------------------------------------------

// temp
#define CONV_RAD 0.01745329251994329576923690768489
#define CONV_DEG 57.295779513082320876798154814105

#define NULL 0

//(copied from std)
// Maximum value that can be returned by the rand function:
#define RAND_MAX 0x7fff

btf32 Random(btf32 min, btf32 max)
{
	//return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return min + (btf32)(rand()) / (((btf32)RAND_MAX) / (max - min));
};
inline btf32 Radians(btf32 value)
{
	return value * CONV_RAD;
}
inline btf32 Degrees(btf32 value)
{
	return value * CONV_DEG;
}

void AngToVec2(btf32 angle, btf32* out_x, btf32* out_y)
{
	*out_x = sin(angle);
	*out_y = cos(angle);
}

typedef struct vec2
{
	btf32 x, y;
} vec2;
typedef struct vec3
{
	btf32 x, y, z;
} vec3;

vec3 Vec3Add(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x + a.x; c.y = a.y + a.y; c.z = a.z + a.z;
	return c;
}
vec3 Vec3Sub(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}
vec3 Vec3Mul(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}
vec3 Vec3Div(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}

//________________________________________________________________________________________________________________________________
// TEMP OBJECTS ------------------------------------------------------------------------------------------------------------------

#define MAX_COMPONENT_COUNT 4ui8

#define TYPE_NULL 0ui8
#define TYPE_CHARA 1ui8
#define TYPE_ITEMS 2ui8
typedef btui8 EntityType;

//-------------------------------- ENTITY COMPONENT

typedef struct EntityComponent
{
	EntityType type;
	void* data;
	void(*ptrTick)(void* ent);
	void(*ptrDraw)(void* ent);
} EntityComponent;

//-------------------------------- ENTITY

typedef struct EntityC
{
	EntityType type;
	
	vec3 position;
	vec3 velocity;

	btui32 component_count;
	EntityComponent components[MAX_COMPONENT_COUNT];
} EntityC;
void TickEntity(EntityC* const ent)
{
	for (btui32 i = 0u; i < ent->component_count; ++i)
		ent->components[i].ptrTick(ent);
};
void DrawEntity(EntityC* const ent)
{
	for (btui32 i = 0u; i < ent->component_count; ++i)
		ent->components[i].ptrDraw(ent);
};

//-------------------------------- ENTITY COMPONENT STRUCTURES / FUNCTIONS

void ECDNothing_Draw(void* ent, void* cmp)
{
	//
}

typedef struct ECDController
{
	btui8 temp;
} ECDController;
void ECDController_Tick(void* ent, void* cmp)
{
	((EntityC*)ent)->velocity.x = 0.005f;
	((EntityC*)ent)->velocity.y = 0.f;
	((EntityC*)ent)->velocity.z = 0.f;
}
void ECDController_Draw(void* ent, void* cmp)
{
	//
}

typedef struct ECDDynamics
{
	btui8 temp;
} ECDDynamics;
void ECDDynamics_Tick(void* ent, void* cmp)
{
	((EntityC*)ent)->position = Vec3Add(((EntityC*)ent)->position, ((EntityC*)ent)->velocity);
}

//-------------------------------- INITIALIZATION

void EntityInit(void* const ent, EntityType type)
{
	((EntityC*)ent)->type = type;
	switch (type)
	{
	case TYPE_CHARA:
		((EntityC*)ent)->components[0].data = malloc(sizeof(ECDController));
		((EntityC*)ent)->components[0].ptrTick = ECDController_Tick;
		((EntityC*)ent)->components[0].ptrDraw = ECDController_Draw;
		((EntityC*)ent)->components[1].data = malloc(sizeof(ECDDynamics));
		((EntityC*)ent)->components[1].ptrTick = ECDDynamics_Tick;
		((EntityC*)ent)->components[1].ptrDraw = ECDNothing_Draw;
		((EntityC*)ent)->component_count = 2u;
		break;
	//case TYPE_ITEMS:
	//	((Entity*)ent)->ptrTick = TickActor;
	//	((Entity*)ent)->ptrDraw = DrawActor;
	//	break;
	}
};

void EntityEnd(void* const ent)
{
	free(((EntityC*)ent)->components[0].data);
	((EntityC*)ent)->component_count = 0u;
};

//________________________________________________________________________________________________________________________________
// INDEX -------------------------------------------------------------------------------------------------------------------------

ObjBufCP block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

void IndexInitialize()
{
	ObjBufCP_init(&block_proj);

	EntityC entity;
	EntityInit(&entity, TYPE_CHARA);
}

btID IndexSpawnProjectile()
{
	//yaw += Radians(Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
	//pitch += Radians(Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

	//btID id = block_proj.add();
	return ObjBufCP_add(&block_proj);
}

void IndexDestroyProjectileC(btID id)
{
	//block_proj.remove(id);
	ObjBufCP_remove(&block_proj, id);
}
