#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

#include "entity.h"
#include "objects_items.h"

//-------------------------------- ENTITIES

// Return a string which will be printed to the screen when this entity is looked at
char* EntityName(lui32 index);
// Tick this entity
void EntityTick(lui32 index, lf32 dt);
// Render graphics of this entity
void EntityDraw(lui32 index);
// Set properties of an entity so that we can use it in-game
void IndexRegisterEntityMeta(EntityType type, lui32 size,
	char*(*_fpName)(void*), void(*_fpTick)(LtrID, void*, lf32), void(*_fpDraw)(LtrID, void*));
// Make an entity at this specified address
void IndexSpawnEntityFixedID(EntityType TYPE, LtrID ID);
// Make an entity, return ID
LtrID IndexSpawnEntity(EntityType TYPE);
// Get whether an entity with this ID exists
bool GetEntityExists(LtrID id);
// Get whether any entity is taking up this space (for iterating (temp?))
bool AnyEntityHere(lui32 index);
// Get the ID of the last entity
lui32 GetEntityArraySize();
// Get the pointer address of the entity at X ID
void* GetEntityPtr(lui32 index);
//
LtrID GetEntityID(lui32 index);
// Replaces the old ENTITY() etc. macros
template <typename Type> Type* GetEntity(lui32 index) {
	return (Type*)GetEntityPtr(index);
}
// Get the type of the entity at ID
EntityType GetEntityType(lui32 index);
//
void IndexDeleteEntity(lui32 index);
//
void IndexClearEntities();

//-------------------------------- ITEMS

// Make an item, return ID
LtrID InitItemInstance(ItemType TYPE);
// Get whether an item with this ID exists
bool ItemInstanceExists(LtrID ID);
// Get the pointer address of the item at ID
void* GetItemInstance(lui32 index);
//
LtrID GetItemInstanceID(lui32 index);
// Get the type of the item at ID
ItemType GetItemInstanceType(lui32 index);
//
void FreeItemInstance(lui32 index);
//
void IndexClearItemInstances();

//-------------------------------- OTHER STUFF

// TODO: working on reducing the use of these functions
//  ok
#define ENT_VOID(a) (GetEntityPtr(a.Index()))
// not ok
#define ACTOR(a) ((ECActor*)GetEntityPtr(a.Index()))
// not ok
#define ENTITY(a) ((ECCommon*)GetEntityPtr(a.Index()))
// not ok
#define ITEM(a) ((ECSingleItem*)GetEntityPtr(a.Index()))

// Get address of item instance by index
#define GETITEMINST(a) ((HeldItem*)GetItemInstance(a.Index()))

#define IDCHECK(a) (a.GUID() != ID2_NULL.GUID())
#define IDCOMPARE(a, b) (a.GUID() == b.GUID())

//-------------------------------- PROJECTILES

typedef struct _prjid { ID16 id; } PrjID;

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
	ID16 type;
} Projectile;

PrjID IndexSpawnProjectile();
void IndexDestroyProjectileC(PrjID id);

Projectile* GetProj(PrjID id);

extern mem::objbuf_caterpillar block_proj; // Projectile buffer
extern Projectile proj[BUF_SIZE];

#endif // END OF FILE
