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

	ObjBuf block_item; // Item buffer
	HeldItem* items[BUF_SIZE];

	HeldItem* GetItemPtr(btID id)
	{
		return items[id];
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