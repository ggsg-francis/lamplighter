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

	btID players[2];
	m::Vector2 viewpos;

	btID activeplayer;

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

	void* getEntEditorPawn(btID id) { return nullptr; }
	void* getEntChara(btID id) { return &buf_chara_data[id]; }
	void* getEntRestingItem(btID id) { return &buf_resting_item_data[id]; }
	void*(*GetEntArray[])(btID) = { getEntEditorPawn, getEntRestingItem, getEntChara };
	void* BufferPtr[] = { nullptr, &buf_resting_item_data, &buf_chara_data };
	void* GetEntityPtr(btID id)
	{
		return GetEntArray[block_entity_data[id].type](block_entity_data[id].type_buffer_index);
		//return (BufferPtr[type])[]
	}

	//-------------------------------- ITEMS

	//temp
	//HeldItem* items[BUF_SIZE];

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

	void* getItemMis(btID id) { return &buf_item_misc_data[id]; }
	void* getItemMel(btID id) { return &buf_item_melee_data[id]; }
	void* getItemGun(btID id) { return &buf_item_gun_data[id]; }
	void* getItemMgc(btID id) { return &buf_item_mgc_data[id]; }
	void*(*GetItemArray[])(btID) = { getItemMis, getItemMis, getItemMis, getItemMel, getItemGun, getItemMgc, getItemMis };
	void* GetItemPtr(btID id)
	{
		//return items[id];
		return GetItemArray[block_item_data[id].type](block_item_data[id].type_buffer_index);
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