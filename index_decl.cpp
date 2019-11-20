// Included in index_fn.cpp

#define CUTE_C2_IMPLEMENTATION
#include "3rdparty/cute_c2.h"

#include "objects.h"

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

	//block of IDs in memory, tracks the numbers and IDs of any type of object
	mem::objbuf block_entity; // Entity buffer
	Entity* _entities[BUF_SIZE]; // Character buffer

	mem::objbuf_caterpillar block_proj; // Projectile buffer
	// Projectile variables
	#define PROJ_TRAIL_NUM 32
	struct Proj
	{
		Transform2D t;
		btf64 ttd = 0.f;
		fac::faction faction = fac::none;
		graphics::Matrix4x4 smokePoints[PROJ_TRAIL_NUM];
		btf32 smokePointTime[PROJ_TRAIL_NUM] { 0.f };
		btui8 smokePointIndex = 0ui8;
		btf32 distance_travelled = 0.f;
	};
	Proj proj[BUF_SIZE];

	/*
	mem::objbuf_caterpillar block_effect; // Effect buffer
	// Effect variables
	struct Effect
	{
		Transform2D t;
		btf64 time2;
	};
	Effect effects[BUF_SIZE];
	*/

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//------------- DEFINES FOR 'EASY ACCESS' ;3 ---------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	namespace def
	{
		btui8 index = 0ui8; // waste of a byte
		btui8 a = 0ui8; // waste of a byte

		#define eCSI ((Entity*)_entities[index])->csi
		#define ePos ((Entity*)_entities[index])->t.position
		#define eHgt ((Entity*)_entities[index])->t.height
		#define eYaw2 ((Entity*)_entities[index])->yaw

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