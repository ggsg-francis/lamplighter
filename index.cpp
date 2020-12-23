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
lui32 sizes[ENTITY_TYPE_COUNT];
// Return a string which will be printed to the screen when this entity is looked at
char*(*fpName[ENTITY_TYPE_COUNT])(void*);
// Tick this entity
void(*fpTick[ENTITY_TYPE_COUNT])(LtrID, void*, lf32);
// Render graphics of this entity
void(*fpDraw[ENTITY_TYPE_COUNT])(LtrID, void*);

char* EntityName(lui32 index) {
	LtrID id = eb->GetID(index);
	#ifdef _DEBUG
	EntityType t = GetEntityType(index);
	if (!GetEntityExists(id))
		int breakkk = 0;
	#endif
	return fpName[GetEntityType(index)](ENT_VOID(id));
}
void EntityTick(lui32 index, lf32 dt) {
	LtrID id = eb->GetID(index);
	fpTick[GetEntityType(index)](id, ENTITY(id), dt); // Call tick on entity
}
void EntityDraw(lui32 index) {
	LtrID id = eb->GetID(index);
	fpDraw[GetEntityType(index)](id, ENTITY(id)); // Call draw on entity
}
void IndexRegisterEntityMeta(EntityType type, lui32 size,
	char*(*_fpName)(void*), void(*_fpTick)(LtrID, void*, lf32), void(*_fpDraw)(LtrID, void*)) {
	sizes[type] = size;
	fpName[type] = _fpName;
	fpTick[type] = _fpTick;
	fpDraw[type] = _fpDraw;
}
void IndexSpawnEntityFixedID(EntityType type, LtrID id) {
	eb->AddEntForceID(sizes[type], type, id);
}
LtrID IndexSpawnEntity(EntityType type) {
	return eb->AddEnt(sizes[type], type);
}
bool GetEntityExists(LtrID id) {
	return eb->EntExists(id);
}
bool AnyEntityHere(lui32 index) {
	return eb->AnyEntHere(index);
}
lui32 GetEntityArraySize() {
	return eb->GetSize();
}
void* GetEntityPtr(lui32 index) {
	return eb->GetEnt(index);
}
LtrID GetEntityID(lui32 index) {
	return eb->GetID(index);
}
EntityType GetEntityType(lui32 index) {
	return eb->GetType(index);
}
void IndexDeleteEntity(lui32 index) {
	if (eb->AnyEntHere(index))
		eb->RmvEnt(index);
	else printf("Tried to delete nonexistent entity at %i\n", index);
}
void IndexClearEntities() {
	eb->Clear();
}

//-------------------------------- ITEMS

LtrID InitItemInstance(ItemType type) {
	return buf_iteminst->Add(type);
}
bool ItemInstanceExists(LtrID id) {
	return buf_iteminst->Exists(id);
}
void* GetItemInstance(lui32 index) {
	return &buf_iteminst->Data(index);
}
LtrID GetItemInstanceID(lui32 index) {
	return buf_iteminst->GetID(index);
}
ItemType GetItemInstanceType(lui32 index) {
	return buf_iteminst->Type(index);
}
void FreeItemInstance(lui32 index) {
	if (buf_iteminst->AnyHere(index))
		buf_iteminst->Remove(index);
}
void IndexClearItemInstances() {
	buf_iteminst->Clear();
}

//________________________________________________________________________________________________________________________________
// INITIALIZATION ----------------------------------------------------------------------------------------------------------------

PrjID MakePrjID(int i)
{
	PrjID h;
	h.id = (ID16)i;
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
