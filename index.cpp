#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#include "objects_items.h"
#include "objects_activators.h"

//________________________________________________________________________________________________________________________________
// OBJECT BUFFERS ----------------------------------------------------------------------------------------------------------------

// Amount of storage allocate for entities
#define ENT_MEM_SZ 0b00000000000001000000000000000000

#define DYNALLOC_TEST 1

#if DYNALLOC_TEST
mem::Lump<EntityType, ENTITY_TYPE_NULL, BUF_SIZE, ENT_MEM_SZ>* eb;
mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>* buf_iteminst;
#else
mem::Lump<EntityType, ENTITY_TYPE_NULL, BUF_SIZE, ENT_MEM_SZ> eb_static;
mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE> buf_iteminst_static;
mem::Lump<EntityType, ENTITY_TYPE_NULL, BUF_SIZE, ENT_MEM_SZ>* eb;
mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>* buf_iteminst;
#endif
bool initialized = false;

mem::objbuf_caterpillar block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

void IndexInitialize()
{
	#if DYNALLOC_TEST
	eb = new mem::Lump<EntityType, ENTITY_TYPE_NULL, BUF_SIZE, ENT_MEM_SZ>();
	buf_iteminst = new mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>();
	#else
	eb_static = mem::Lump<EntityType, ENTITY_TYPE_NULL, BUF_SIZE, ENT_MEM_SZ>();
	eb = &eb_static;
	buf_iteminst_static = mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>();
	buf_iteminst = &buf_iteminst_static;
	#endif
	initialized = true;
	InitEntityMeta();
	for (int i = 0; i < BUF_SIZE; ++i)
		block_proj.used[i] = false;
}

void IndexEnd()
{
	#if DYNALLOC_TEST
	delete eb;
	eb = nullptr;
	delete buf_iteminst;
	buf_iteminst = nullptr;
	initialized = false;
	#else
	eb = nullptr;
	buf_iteminst = nullptr;
	#endif
}

//-------------------------------- ENTITIES

// Sizes of the various entity types
btui32 sizes[ENTITY_TYPE_COUNT];
// Return a string which will be printed to the screen when this entity is looked at
char*(*fpName[ENTITY_TYPE_COUNT])(void*);
// Tick this entity
void(*fpTick[ENTITY_TYPE_COUNT])(btID, void*, btf32);
// Render graphics of this entity
void(*fpDraw[ENTITY_TYPE_COUNT])(btID, void*);

char* EntityName(btID id) {
	return fpName[GetEntityType(id)](ENT_VOID(id));
}
void EntityTick(btID id, btf32 dt) {
	fpTick[GetEntityType(id)](id, ENTITY(id), dt); // Call tick on entity
}
void EntityDraw(btID id) {
	fpDraw[GetEntityType(id)](id, ENTITY(id)); // Call draw on entity
}
void IndexRegisterEntityMeta(EntityType type, btui32 size,
	char*(*_fpName)(void*), void(*_fpTick)(btID, void*, btf32), void(*_fpDraw)(btID, void*)) {
	sizes[type] = size;
	fpName[type] = _fpName;
	fpTick[type] = _fpTick;
	fpDraw[type] = _fpDraw;
}
void IndexSpawnEntityFixedID(EntityType type, btID id) {
	eb->AddEntForceID(sizes[type], type, id);
}
btID IndexSpawnEntity(EntityType type) {
	return eb->AddEnt(sizes[type], type);
}
bool GetEntityExists(btID id) {
	return eb->EntExists(id);
}
btID GetLastEntity() {
	return eb->GetLastIndex();
}
void* GetEntityPtr(btID id) {
	return eb->GetEnt(id);
}
EntityType GetEntityType(btID id) {
	return eb->GetType(id);
}
void IndexDeleteEntity(btID id) {
	eb->RmvEnt(id);
}
void IndexClearEntities() {
	eb->Clear();
}

//-------------------------------- ITEMS

btID InitItemInstance(ItemType type) {
	return buf_iteminst->Add(type);
}
bool ItemInstanceExists(btID id) {
	return buf_iteminst->Used(id);
}
void* GetItemInstance(btID id) {
	return &buf_iteminst->Data(id);
}
ItemType GetItemInstanceType(btID id) {
	return buf_iteminst->Type(id);
}
void FreeItemInstance(btID id) {
	if (buf_iteminst->Used(id))
		buf_iteminst->Remove(id);
}
void IndexClearItemInstances() {
	buf_iteminst->Clear();
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

Projectile* GetProj(PrjID id)
{
	return &(proj[id.id]);
}

PrjID IndexSpawnProjectile()
{
	return MakePrjID(block_proj.add());
}

void IndexDestroyProjectileC(PrjID id)
{
	block_proj.remove(id.id);
}
