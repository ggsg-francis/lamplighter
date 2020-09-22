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
// Location and size of entity
struct EntPtr {
	btui32 pos;
	btui32 size;
};
// Entity container
struct EntityBuffer {
private:
	mem::ObjBuf<EntPtr, EntityType, ENTITY_TYPE_NULL, BUF_SIZE> eptrs;
	btui8 buf[ENT_MEM_SZ];
	inline btui32 AllocateSpace(btui32 size) {
		// Search for empty space between ents (or at the end, it just works)
		// For all assigned spaces
		for (int i = 0; i <= eptrs.index_end; ++i) {
			// Skip self or unused
			if (!eptrs.Used(i)) continue;
			btui32 this_end = eptrs.Data(i).pos + eptrs.Data(i).size;
			btui32 nearest_start = ENT_MEM_SZ - 1;
			// For all other spaces
			for (int j = 0; j <= eptrs.index_end; ++j) {
				// Skip self or comparing same or unused
				if (j == i || !eptrs.Used(j)) continue;
				btui32 next_start = eptrs.Data(j).pos;
				if (next_start < nearest_start && next_start >= this_end)
					nearest_start = next_start;
			}
			if (nearest_start - this_end >= size) return this_end;
		}
		// else return the first element
		if (size >= ENT_MEM_SZ) {
			printf("Tried to add an entity bigger than the buffer itself, what??\n");
			return ID_NULL; // not enough room
		}
		return 0;
	}
public:
	EntityBuffer() {
		memset(&buf[0], 0, ENT_MEM_SZ);
	}
	btui32 AddEntForceID(btui32 size, EntityType type, btui32 id) {
		btui32 pos = AllocateSpace(size);
		eptrs.AddForceID(type, id);
		eptrs.Data(id).pos = pos;
		eptrs.Data(id).size = size;
		return id;
	}
	btui32 AddEnt(btui32 size, EntityType type) {
		btui32 pos = AllocateSpace(size);
		btui32 index = eptrs.Add(type);
		eptrs.Data(index).pos = pos;
		eptrs.Data(index).size = size;
		printf("Add ent ID:%i Pos:%i Size: %i\n", index, eptrs.Data(index).pos, eptrs.Data(index).size);
		return index;
	}
	void RmvEnt(btui32 index) {
		printf("Rmv ent ID:%i Pos:%i Size: %i\n", index, eptrs.Data(index).pos, eptrs.Data(index).size);
		memset(&buf[eptrs.Data(index).pos], 0, eptrs.Data(index).size);
		eptrs.Remove(index);
	}
	void Clear() {
		for (int i = 0; i < BUF_SIZE; ++i) {
			eptrs.Data(i).pos = 0;
			eptrs.Data(i).size = 0;
		}
		eptrs.Clear();
		memset(&buf[0], 0, ENT_MEM_SZ);
	}
	void* GetEnt(btui32 index) {
		return &buf[eptrs.Data(index).pos];
	}
	EntityType GetType(btui32 index) {
		return eptrs.Type(index);
	}
	bool EntExists(btui32 index) {
		return eptrs.Used(index);
	}
	btui32 GetLastIndex() {
		return eptrs.index_end;
	}
};

EntityBuffer* eb;
mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>* buf_iteminst;
mem::ObjBuf<StaticActivator, ActivatorType, (ActivatorType)255u, BUF_SIZE>* activators;
mem::objbuf_caterpillar block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

void IndexInitialize()
{
	buf_iteminst = new mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>();
	eb = new EntityBuffer();
	InitEntityMeta();
	for (int i = 0; i < BUF_SIZE; ++i)
		block_proj.used[i] = false;
	activators = new mem::ObjBuf<StaticActivator, ActivatorType, (ActivatorType)255u, BUF_SIZE>();
}

void IndexEnd()
{
	delete eb;
	delete buf_iteminst;
	delete activators;
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

//-------------------------------- ACTIVATORS

btID InitActivator(ActivatorType type) {
	return activators->Add(type);
}
bool GetActivatorExists(btID id) {
	return activators->Used(id);
}
void* GetActivatorPtr(btID id) {
	return &activators->Data(id);
}
ActivatorType GetActivatorType(btID id) {
	return activators->Type(id);
}
void FreeActivator(btID id) {
	if (activators->Used(id))
		activators->Remove(id);
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
