#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

#include "entity.h"
#include "objects_items.h"

//-------------------------------- ENTITIES

// Return a string which will be printed to the screen when this entity is looked at
char* EntityName(lid id);
// Tick this entity
void EntityTick(lid id, lf32 dt);
// Render graphics of this entity
void EntityDraw(lid id);
// Set properties of an entity so that we can use it in-game
void IndexRegisterEntityMeta(EntityType type, lui32 size,
	char*(*_fpName)(void*), void(*_fpTick)(lid, void*, lf32), void(*_fpDraw)(lid, void*));
// Make an entity at this specified address
void IndexSpawnEntityFixedID(EntityType TYPE, lid ID);
// Make an entity, return ID
lid IndexSpawnEntity(EntityType TYPE);
// Get whether an entity with this ID exists
bool GetEntityExists(lid ID);
// Get the ID of the last entity
lid GetEntityArraySize();
// Get the pointer address of the entity at X ID
void* GetEntityPtr(lid ID);
// Replaces the old ENTITY() etc. macros
template <typename Type> Type* GetEntity(lid id) {
	return (Type*)GetEntityPtr(id);
}
// Get the type of the entity at ID
EntityType GetEntityType(lid ID);
//
void IndexDeleteEntity(lid ID);
//
void IndexClearEntities();

//-------------------------------- ITEMS

// Make an item, return ID
lid InitItemInstance(ItemType TYPE);
// Get whether an item with this ID exists
bool ItemInstanceExists(lid ID);
// Get the pointer address of the item at ID
void* GetItemInstance(lid ID);
// Get the type of the item at ID
ItemType GetItemInstanceType(lid ID);
//
void FreeItemInstance(lid ID);
//
void IndexClearItemInstances();

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

typedef struct _prjid { lid id; } PrjID;

PrjID MakePrjID(int i);

void IndexInitialize();
void IndexEnd();

// Transform structure used for game simulation (C version)
typedef struct
{
	// world space position
	lf32 position_x;
	lf32 position_y;
	lf32 position_h;
	lf32 velocity_x;
	lf32 velocity_y;
	lf32 velocity_h;
	lui8 cellx;
	lui8 celly;
} TransformC;

typedef struct
{
	TransformC t;
	lui64 ttd;
	lui32 faction;
	lid type;
} Projectile;

PrjID IndexSpawnProjectile();
void IndexDestroyProjectileC(PrjID id);

Projectile* GetProj(PrjID id);

extern mem::objbuf_caterpillar block_proj; // Projectile buffer
extern Projectile proj[BUF_SIZE];

#endif // END OF FILE
