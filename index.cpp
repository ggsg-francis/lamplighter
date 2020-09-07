#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#include "ec_actor.h"
#include "ec_misc.h"
#include "objects_items.h"
#include "objects_activators.h"

//________________________________________________________________________________________________________________________________
// OBJECT BUFFERS ----------------------------------------------------------------------------------------------------------------

//-------------------------------- ENTITIES

//block of IDs in memory, tracks the numbers and IDs of any type of object
mem::ObjBuf<EntAddr, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> block_entity; // Entity address buffer - holds indexes of the other arrays
// Actor class buffers
mem::ObjBuf<ECSingleItem, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> buf_resting_item;
mem::ObjBuf<ECActor, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> buf_chara;

// Return a string which will be printed to the screen when this entity is looked at
char*(*fpName[ENTITY_TYPE_COUNT])(void* self) = {
	ActorName,
	RestingItemName,
	ActorName,
};

// Tick this entity
void(*fpTick[ENTITY_TYPE_COUNT])(btID id, void* self, btf32 dt) = {
	TickEditorPawn,
	RestingItemTick,
	ActorTick,
};

// Render graphics of this entity
void(*fpDraw[ENTITY_TYPE_COUNT])(btID id, void* self) = {
	DrawEditorPawn,
	RestingItemDraw,
	ActorDraw,
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
btID InitEntity(EntityType type) {
	btID id = block_entity.Add(type);

	block_entity.Data(id).type = type;
	switch (type)
	{
	case ENTITY_TYPE_EDITOR_PAWN:
		memset(ENT_VOID(id), 0, sizeof(EditorPawn));
		break;
	case ENTITY_TYPE_RESTING_ITEM:
		block_entity.Data(id).type_buffer_index = buf_resting_item.Add(ENTITY_TYPE_RESTING_ITEM);
		memset(ENT_VOID(id), 0, sizeof(ECSingleItem));
		break;
	case ENTITY_TYPE_ACTOR:
		block_entity.Data(id).type_buffer_index = buf_chara.Add(ENTITY_TYPE_ACTOR);
		memset(ENT_VOID(id), 0, sizeof(ECActor));
		break;
	default:
		std::cout << "Tried to initialize entity of no valid type" << std::endl;
		break;
	}

	return id;
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
	return block_entity.Type(id);
}
void IndexFreeEntity(btID id)
{
	if (block_entity.Used(id))
	{
		switch (GetEntityType(id))
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

//-------------------------------- ITEMS

mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE> buf_iteminst;
btID InitItemInstance(ItemType type) {
	return buf_iteminst.Add(type);
}
bool ItemInstanceExists(btID id) {
	return buf_iteminst.Used(id);
}
void* GetItemInstance(btID id) {
	return &buf_iteminst.Data(id);
}
ItemType GetItemInstanceType(btID id) {
	return buf_iteminst.Type(id);
}
void FreeItemInstance(btID id) {
	if (buf_iteminst.Used(id))
		buf_iteminst.Remove(id);
}

//-------------------------------- ACTIVATORS

mem::ObjBuf<StaticActivator, ActivatorType, (ActivatorType)255u, BUF_SIZE> activators;
btID InitActivator(ActivatorType type) {
	return activators.Add(type);
}
bool GetActivatorExists(btID id) {
	return activators.Used(id);
}
void* GetActivatorPtr(btID id) {
	return &activators.Data(id);
}
ActivatorType GetActivatorType(btID id) {
	return activators.Type(id);
}
void FreeActivator(btID id) {
	if (activators.Used(id))
		activators.Remove(id);
}

//________________________________________________________________________________________________________________________________
// INITIALIZATION ----------------------------------------------------------------------------------------------------------------

PrjID MakePrjID(int i)
{
	PrjID h;
	h.id = (btID)i;
	return h;
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

//________________________________________________________________________________________________________________________________
// NEW INDEX ---------------------------------------------------------------------------------------------------------------------

void MakeEntity(EntityType type) {
	Entity entity;
	//switch (type)
	//{
	//case ENTITY_TYPE_ACTOR:
	//	entity.Init<ECActor>();
	//	break;
	//}
}
