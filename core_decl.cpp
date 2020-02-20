// Included in index_fn.cpp

#define CUTE_C2_IMPLEMENTATION
#include "3rdparty/cute_c2.h"

#include "objects.h"
#include "objects_items.h"
#include "memoryC.h"

struct Index
{
	struct cell
	{
		mem::idbuf ents;
	};
	cell cells[WORLD_SIZE][WORLD_SIZE];
};

namespace index
{
	// Environment lightmap texture
	// R: Unused | G: Lightsources | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvLightmap;
	// R: Height | G: Unused | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvHeightmap;

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- GLOBAL VARIABLES -----------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	float networkTimerTemp;

	unsigned int activePlayer = 0u;
	btID players[2];
	m::Vector2 viewpos;
	btID viewtarget[2]{ ID_NULL,ID_NULL };
	btID viewtarget_last_tick[2]{ ID_NULL,ID_NULL };

	env::EnvNode editor_node_copy;

	struct cell
	{
		mem::idbuf ents;
	};
	cell cells[WORLD_SIZE][WORLD_SIZE];

	// inventory stuff
	graphics::GUIBox guibox;
	graphics::GUIText text_temp;
	graphics::GUIText text_version;
	graphics::GUIText text_fps;

	graphics::GUIText text_message[2];
	btf64 message_time[2];

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- ENTITY BUFFERS -------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	//-------------------------------- ENTITIES

	//block of IDs in memory, tracks the numbers and IDs of any type of object
	mem::objbuf block_entity; // Entity buffer
	EntAddr block_entity_data[BUF_SIZE];
	// TODO: using fixed size arrays is a big memory hog, and the amount of space allocated here can never be filled
	// just get it working for now, but use dynamic size arrays when possible
	//mem::CkBuffer2<Chara> buf_entities;
	mem::objbuf buf_resting_item;
	RestingItem buf_resting_item_data[BUF_SIZE];
	mem::objbuf buf_chara;
	Chara       buf_chara_data[BUF_SIZE];

	mem::objbuf* BufPtr[ENTITY_TYPE_COUNT] = { &buf_chara, &buf_resting_item, &buf_chara };
	void* BufDataPtr[ENTITY_TYPE_COUNT] = { &buf_chara_data, &buf_resting_item_data, &buf_chara_data };
	unsigned long long BufDataSize[ENTITY_TYPE_COUNT] = { sizeof(EditorPawn), sizeof(RestingItem), sizeof(Chara) };
	// Return a string which will be printed to the screen when this entity is looked at
	char*(*fpName[ENTITY_TYPE_COUNT])(void* self) { DisplayNameActor, DisplayNameRestingItem, DisplayNameActor };
	// Tick this entity
	void(*fpTick[ENTITY_TYPE_COUNT])(void* self, btf32 dt) { TickEditorPawn, TickRestingItem, TickChara };
	// Render graphics of this entity
	void(*fpDraw[ENTITY_TYPE_COUNT])(void* self) { DrawEditorPawn, DrawRestingItem, DrawChara };
	// Get Entity address from ID
	void* getEntEditorPawn(btID id) { return &buf_chara_data[id]; }
	void* getEntRestingItem(btID id) { return &buf_resting_item_data[id]; }
	void* getEntChara(btID id) { return &buf_chara_data[id]; }
	void*(*GetEntArray[ENTITY_TYPE_COUNT])(btID) = { getEntEditorPawn, getEntRestingItem, getEntChara };
	void* GetEntityPtr(btID id)
	{
		return GetEntArray[block_entity_data[id].type](block_entity_data[id].type_buffer_index);
	}

	//-------------------------------- ITEMS

	ObjBuf block_item; // Item buffer
	EntAddr block_item_data[BUF_SIZE];

	ObjBuf buf_item_misc; // Item buffer
	HeldItem buf_item_misc_data[BUF_SIZE];

	ObjBuf buf_item_melee; // Item buffer
	HeldMel buf_item_melee_data[BUF_SIZE];

	ObjBuf buf_item_gun; // Item buffer
	HeldGun buf_item_gun_data[BUF_SIZE];

	ObjBuf buf_item_mgc; // Item buffer
	HeldMgc buf_item_mgc_data[BUF_SIZE];

	ObjBuf buf_item_con; // Item buffer
	HeldCons buf_item_con_data[BUF_SIZE];

	void* getItemMis(btID id) { return &buf_item_misc_data[id]; }
	void* getItemEqp(btID id) { return &buf_item_misc_data[id]; }
	void* getItemMel(btID id) { return &buf_item_melee_data[id]; }
	void* getItemGun(btID id) { return &buf_item_gun_data[id]; }
	void* getItemMgc(btID id) { return &buf_item_mgc_data[id]; }
	void* getItemCon(btID id) { return &buf_item_con_data[id]; }
	void*(*GetItemArray[])(btID) = { getItemMis, getItemEqp, getItemMel, getItemGun, getItemMgc, getItemCon };
	ObjBuf* ItemBufPtr[] = { &buf_item_misc, &buf_item_misc, &buf_item_melee, &buf_item_gun, &buf_item_mgc, &buf_item_con };
	void* GetItemPtr(btID id)
	{
		return GetItemArray[block_item_data[id].type](block_item_data[id].type_buffer_index);
	}
	ItemType GetItemType(btID id)
	{
		return block_item_data[id].type;
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//------------- DEFINES FOR 'EASY ACCESS' ;3 ---------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	// TODO: fuck this honestly
	namespace def
	{
		#define accel 0.025f // Accelleration speed (now not relevant due to animation based motion)
		#define rotdeg 4.f // Rotation in degrees per frame when we hit a wall
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- FUNCTION DECLARATIONS ------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	btID GetClosestPlayer(btID id);
	btID GetClosestEntity(btID id, btf32 dist);
	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance);

	void ProjectileTick(btf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(btID id);
}