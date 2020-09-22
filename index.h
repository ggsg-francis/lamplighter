#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

#include "entity.h"
#include "objects_items.h"

//-------------------------------- ENTITIES

// Return a string which will be printed to the screen when this entity is looked at
char* EntityName(btID id);
// Tick this entity
void EntityTick(btID id, btf32 dt);
// Render graphics of this entity
void EntityDraw(btID id);
// Set properties of an entity so that we can use it in-game
void IndexRegisterEntityMeta(EntityType type, btui32 size,
	char*(*_fpName)(void*), void(*_fpTick)(btID, void*, btf32), void(*_fpDraw)(btID, void*));
// Make an entity at this specified address
void IndexSpawnEntityFixedID(EntityType TYPE, btID ID);
// Make an entity, return ID
btID IndexSpawnEntity(EntityType TYPE);
// Get whether an entity with this ID exists
bool GetEntityExists(btID ID);
// Get the ID of the last entity
btID GetLastEntity();
// Get the pointer address of the entity at X ID
void* GetEntityPtr(btID ID);
// Replaces the old ENTITY() etc. macros
template <typename Type> Type* GetEntity(btID id) {
	return (Type*)GetEntityPtr(id);
}
// Get the type of the entity at ID
EntityType GetEntityType(btID ID);
//
void IndexDeleteEntity(btID ID);
//
void IndexClearEntities();

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
void IndexEnd();

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
