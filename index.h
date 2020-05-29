#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

//-------------------------------- ENTITIES

typedef struct EntAddr
{
	EntityType type;
	btID type_buffer_index;
} EntAddr;

// Block of IDs in memory, tracks the numbers and types of entities
extern mem::objbuf block_entity;
extern EntAddr block_entity_data[BUF_SIZE];
// Get the pointer address of the entity at X ID
void* GetEntityPtr(btID ID);

// Block of IDs in memory, tracks the numbers and types of items
extern mem::objbuf block_item;
// Get the pointer address of the item at ID
void* GetItemPtr(btID ID);
// Get the type of the item at ID
ItemType GetItemType(btID ID);

// Return a string which will be printed to the screen when this entity is looked at
extern char*(*fpName[ENTITY_TYPE_COUNT])(void* self);
// Tick this entity
extern void(*fpTick[ENTITY_TYPE_COUNT])(void* self, btf32 dt);
// Render graphics of this entity
extern void(*fpDraw[ENTITY_TYPE_COUNT])(void* self);

// TODO: working on reducing the use of these functions
//  ok
#define ENT_VOID(a) (GetEntityPtr(a))
// not ok
#define ACTOR(a) ((Actor*)GetEntityPtr(a))
// not ok
#define ENTITY(a) ((Entity*)GetEntityPtr(a))
// not ok
#define ITEM(a) ((RestingItem*)GetEntityPtr(a))

#define GETITEM_VOID(a) (GetItemPtr(a))
#define GETITEM_MISC(a) ((HeldItem*)GetItemPtr(a))
#define GETITEM_MELEE(a) ((HeldMel*)GetItemPtr(a))
#define GETITEM_GUN(a) ((HeldGun*)GetItemPtr(a))
#define GETITEM_MAGIC(a) ((HeldMgc*)GetItemPtr(a))
#define GETITEM_CONS(a) ((HeldCons*)GetItemPtr(a))

void IndexInitEntity(btID ID, EntityType TYPE);
void IndexFreeEntity(btID ID);
void IndexInitItem(btID ID, ItemType TYPE);
void IndexFreeItem(btID ID);

//-------------------------------- PROJECTILES



typedef struct _prjid { btID id; } PrjID;

PrjID MakePrjID(int i);

void IndexInitialize();

// Transform structure used for game simulation (C version)
typedef struct
{
	// world space position
	btf32 position_x;
	btf32 position_y;
	btf32 position_h;
	btf32 velocity_x;
	btf32 velocity_y;
	btf32 velocity_h;
	btui8 cellx;
	btui8 celly;
} TransformC;

typedef struct
{
	TransformC t;
	btui64 ttd;
	btui32 faction;
	btID type;
} Projectile;

PrjID IndexSpawnProjectile();
void IndexDestroyProjectileC(PrjID id);

Projectile* GetProj(PrjID id);

extern mem::objbuf_caterpillar block_proj; // Projectile buffer
extern Projectile proj[BUF_SIZE];

#endif // END OF FILE
