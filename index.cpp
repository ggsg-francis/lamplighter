#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#include "objects_entities.h"
#include "objects_items.h"
#include "objects_activators.h"

//________________________________________________________________________________________________________________________________
// OBJECT BUFFERS ----------------------------------------------------------------------------------------------------------------

//-------------------------------- ENTITIES

//block of IDs in memory, tracks the numbers and IDs of any type of object
mem::ObjBuf<EntAddr, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> block_entity; // Entity address buffer - holds indexes of the other arrays
// Actor class buffers
mem::ObjBuf<RestingItem, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> buf_resting_item;
mem::ObjBuf<Actor, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> buf_chara;

// Return a string which will be printed to the screen when this entity is looked at
char*(*fpName[ENTITY_TYPE_COUNT])(void* self) = {
	DisplayNameActor,
	DisplayNameRestingItem,
	DisplayNameActor,
};

// Tick this entity
void(*fpTick[ENTITY_TYPE_COUNT])(void* self, btf32 dt) = {
	TickEditorPawn,
	TickRestingItem,
	TickChara,
};

// Render graphics of this entity
void(*fpDraw[ENTITY_TYPE_COUNT])(void* self) = {
	DrawEditorPawn,
	DrawRestingItem,
	DrawChara,
};

// Get Entity address from ID
void* getEntEditorPawn(btID id) { return &buf_chara.Data(id); }
void* getEntRestingItem(btID id) { return &buf_resting_item.Data(id); }
void* getEntActor(btID id) { return &buf_chara.Data(id); }
// Array lookup for the above functions
void*(*GetEntArray[ENTITY_TYPE_COUNT])(btID) = {
	getEntEditorPawn,
	getEntRestingItem,
	getEntActor,
};
// Array lookup function
btID AssignEntityID(EntityType type) {
	return block_entity.Add(type);
}
bool GetEntityExists(btID id) {
	return block_entity.Used(id);
}
btID GetLastEntity() {
	return block_entity.index_end;
}
void* GetEntityPtr(btID id) {
	return GetEntArray[block_entity.Data(id).type](block_entity.Data(id).type_buffer_index);
}
EntityType GetEntityType(btID id) {
	return block_entity.Data(id).type;
}

//-------------------------------- ITEMS

// (idk which is which and at this point i'm too afraid to ask)
mem::ObjBuf<EntAddr, ItemType, ENTITY_TYPE_NULL, BUF_SIZE> block_item; // Item buffer
mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE> buf_iteminst; // Item buffer
btID AssignItemID(ItemType type) {
	return block_item.Add(type);
}
bool GetItemExists(btID id) {
	return block_item.Used(id);
}
void* GetItemPtr(btID id) {
	return &buf_iteminst.Data(id);
}
ItemType GetItemType(btID id) {
	return block_item.Data(id).type;
}

//-------------------------------- ACTIVATORS

mem::ObjBuf<StaticActivator, ActivatorType2, (ActivatorType2)255u, BUF_SIZE> activators;

//________________________________________________________________________________________________________________________________
// INITIALIZATION ----------------------------------------------------------------------------------------------------------------

void IndexInitEntity(btID id, EntityType type)
{
	block_entity.Data(id).type = type;
	switch (type)
	{
	case ENTITY_TYPE_EDITOR_PAWN:
		memset(ENT_VOID(id), 0, sizeof(EditorPawn));
		break;
	case ENTITY_TYPE_RESTING_ITEM:
		block_entity.Data(id).type_buffer_index = buf_resting_item.Add(ENTITY_TYPE_RESTING_ITEM);
		memset(ENT_VOID(id), 0, sizeof(RestingItem));
		break;
	case ENTITY_TYPE_ACTOR:
		block_entity.Data(id).type_buffer_index = buf_chara.Add(ENTITY_TYPE_ACTOR);
		memset(ENT_VOID(id), 0, sizeof(Actor));
		break;
	default:
		std::cout << "Tried to initialize entity of no valid type" << std::endl;
		break;
	}

	ENTITY(id)->id = id;
	ENTITY(id)->type = type;
}
void IndexFreeEntity(btID id)
{
	if (block_entity.Used(id))
	{
		switch (ENTITY(id)->type)
		{
		case ENTITY_TYPE_EDITOR_PAWN:
			break;
		case ENTITY_TYPE_RESTING_ITEM:
			buf_resting_item.Remove(block_entity.Data(id).type_buffer_index);
			break;
		case ENTITY_TYPE_ACTOR:
			buf_chara.Remove(block_entity.Data(id).type_buffer_index);
			break;
		}
	}
	block_entity.Remove(id);
}
void IndexInitItemInstance(btID id, ItemType type)
{
	block_item.Data(id).type = type;
}
void IndexFreeItem(btID id)
{
	if (block_item.Used(id))
	{
		buf_iteminst.Remove(block_item.Data(id).type_buffer_index);
	}
	block_item.Remove(id);
}

PrjID MakePrjID(int i)
{
	PrjID h;
	h.id = (btID)i;
	return h;
}

//________________________________________________________________________________________________________________________________
// TEMP MATHS --------------------------------------------------------------------------------------------------------------------

// temp
#define CONV_RAD 0.01745329251994329576923690768489
#define CONV_DEG 57.295779513082320876798154814105

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

//________________________________________________________________________________________________________________________________
// INDEX -------------------------------------------------------------------------------------------------------------------------

mem::objbuf_caterpillar block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

Projectile* GetProj(PrjID id)
{
	return &(proj[id.id]);
}

void IndexInitialize()
{
	//
}

PrjID IndexSpawnProjectile()
{
	return MakePrjID(block_proj.add());
}

void IndexDestroyProjectileC(PrjID id)
{
	block_proj.remove(id.id);
}
