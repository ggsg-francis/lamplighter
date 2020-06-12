#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#include "objects_entities.h"
#include "objects_items.h"
#include "objects_statics.h"

//________________________________________________________________________________________________________________________________
// OBJECT BUFFERS ----------------------------------------------------------------------------------------------------------------

//-------------------------------- ENTITIES

//block of IDs in memory, tracks the numbers and IDs of any type of object
mem::objbuf block_entity; // Entity buffer
EntAddr block_entity_data[BUF_SIZE];

mem::objbuf buf_resting_item;
RestingItem buf_resting_item_data[BUF_SIZE];
mem::objbuf buf_chara;
Actor       buf_chara_data[BUF_SIZE];

mem::objbuf* BufPtr[ENTITY_TYPE_COUNT] = { &buf_chara, &buf_resting_item, &buf_chara };
void* BufDataPtr[ENTITY_TYPE_COUNT] = { &buf_chara_data, &buf_resting_item_data, &buf_chara_data };
unsigned long long BufDataSize[ENTITY_TYPE_COUNT] = { sizeof(EditorPawn), sizeof(RestingItem), sizeof(Actor) };

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
void* getEntEditorPawn(btID id) { return &buf_chara_data[id]; }
void* getEntRestingItem(btID id) { return &buf_resting_item_data[id]; }
void* getEntActor(btID id) { return &buf_chara_data[id]; }
// Array lookup for the above functions
void*(*GetEntArray[ENTITY_TYPE_COUNT])(btID) = {
	getEntEditorPawn,
	getEntRestingItem,
	getEntActor,
};
// Array lookup function
btID AssignEntityID() {
	return block_entity.add();
}
bool GetEntityExists(btID id) {
	return block_entity.used[id];
}
btID GetLastEntity() {
	return block_entity.index_end;
}
void* GetEntityPtr(btID id) {
	return GetEntArray[block_entity_data[id].type](block_entity_data[id].type_buffer_index);
}
EntityType GetEntityType(btID id) {
	return block_entity_data[id].type;
}

//-------------------------------- ITEMS

// (idk which is which and at this point i'm too afraid to ask)
mem::objbuf block_item; // Item buffer
EntAddr block_item_data[BUF_SIZE];
mem::objbuf buf_iteminst; // Item buffer
HeldItem buf_iteminst_data[BUF_SIZE];
btID AssignItemID() {
	return block_item.add();
}
bool GetItemExists(btID id) {
	return block_item.used[id];
}
void* GetItemPtr(btID id) {
	return &buf_iteminst_data[id];
}
ItemType GetItemType(btID id) {
	return block_item_data[id].type;
}

//-------------------------------- STATICS

mem::objbuf block_static;
EntAddr block_static_data[BUF_SIZE];
mem::objbuf buf_static;
HeldItem buf_static_data[BUF_SIZE];

//________________________________________________________________________________________________________________________________
// INITIALIZATION ----------------------------------------------------------------------------------------------------------------

void IndexInitEntity(btID id, EntityType type)
{
	block_entity.used[id] = true;
	block_entity_data[id].type = type;
	switch (type)
	{
	case ENTITY_TYPE_EDITOR_PAWN:
		memset(ENT_VOID(id), 0, sizeof(EditorPawn));
		break;
	case ENTITY_TYPE_RESTING_ITEM:
		block_entity_data[id].type_buffer_index = buf_resting_item.add();
		memset(ENT_VOID(id), 0, sizeof(RestingItem));
		break;
	case ENTITY_TYPE_ACTOR:
		block_entity_data[id].type_buffer_index = buf_chara.add();
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
	if (block_entity.used[id])
	{
		switch (ENTITY(id)->type)
		{
		case ENTITY_TYPE_EDITOR_PAWN:
			break;
		case ENTITY_TYPE_RESTING_ITEM:
			buf_resting_item.remove(block_entity_data[id].type_buffer_index);
			break;
		case ENTITY_TYPE_ACTOR:
			buf_chara.remove(block_entity_data[id].type_buffer_index);
			break;
		}
	}
	block_entity.remove(id);
}
void IndexInitItemInstance(btID id, ItemType type)
{
	block_item.used[id] = true;
	block_item_data[id].type = type;
}
void IndexFreeItem(btID id)
{
	if (block_item.used[id])
	{
		buf_iteminst.remove(block_item_data[id].type_buffer_index);
	}
	block_item.remove(id);
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
