#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

#include "ec_actor.h"
#include "ec_misc.h"
#include "objects_items.h"

typedef struct EntAddr
{
	EntityType type;
	btID type_buffer_index;
} EntAddr;

//-------------------------------- ENTITIES

// Make an entity, return ID
btID InitEntity(EntityType TYPE);
// Get whether an entity with this ID exists
bool GetEntityExists(btID ID);
// Get the ID of the last entity
btID GetLastEntity();
// Get the pointer address of the entity at X ID
void* GetEntityPtr(btID ID);
// Get the type of the entity at ID
EntityType GetEntityType(btID ID);
//
void IndexFreeEntity(btID ID);

//-------------------------------- ITEMS

// Make an item, return ID
btID InitItemInstance(ItemType TYPE);
// Get whether an item with this ID exists
bool ItemInstanceExists(btID ID);
// Get the pointer address of the item at ID
void* GetItemInstance(btID ID);
// Get the type of the item at ID
ItemType GetItemInstanceType(btID ID);
//
void FreeItemInstance(btID ID);

//-------------------------------- ACTIVATORS

// Make an item, return ID
btID InitActivator(ActivatorType type);
// Get whether an item with this ID exists
bool GetActivatorExists(btID id);
// Get the pointer address of the item at ID
void* GetActivatorPtr(btID id);
// Get the type of the item at ID
ActivatorType GetActivatorType(btID id);
//
void FreeActivator(btID ID);

//-------------------------------- OTHER STUFF

// Return a string which will be printed to the screen when this entity is looked at
extern char*(*fpName[ENTITY_TYPE_COUNT])(void* self);
// Tick this entity
extern void(*fpTick[ENTITY_TYPE_COUNT])(btID id, void* self, btf32 dt);
// Render graphics of this entity
extern void(*fpDraw[ENTITY_TYPE_COUNT])(btID id, void* self);

// TODO: working on reducing the use of these functions
//  ok
#define ENT_VOID(a) (GetEntityPtr(a))
// not ok
#define ACTOR(a) ((ECActor*)GetEntityPtr(a))
// not ok
#define ENTITY(a) ((ECCommon*)GetEntityPtr(a))
// not ok
#define ITEM(a) ((ECSingleItem*)GetEntityPtr(a))

// Get address of item instance by index
#define GETITEMINST(a) ((HeldItem*)GetItemInstance(a))

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


void MakeEntity(EntityType type);


#endif // END OF FILE
