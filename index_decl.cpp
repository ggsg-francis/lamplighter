// Included in index_fn.cpp

#define CUTE_C2_IMPLEMENTATION
#include "3rdparty/cute_c2.h"

#include "objects.h"

namespace index
{
	// Environment lightmap texture
	// R: Unused | G: Lightsources | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvLightmap;
	// R: Height | G: Unused | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvHeightmap;
	// R: Shadow Crest | G: Bounced Light Level | B: Heightmap | A: Unused
	graphics::ModifiableTexture t_envShadowMap;

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
	graphics::GUIBox guibox_selection;
	graphics::GUIText text_temp;
	graphics::GUIText text_inventory_temp;
	graphics::GUIText text_version;
	graphics::GUIText text_fps;
	btui16 inv_active_slot = 0u;

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- ENTITY BUFFERS -------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	//block of IDs in memory, tracks the numbers and IDs of any type of object
	mem::objbuf block_entity; // Entity buffer
	Entity* _entities[BUF_SIZE]; // Character buffer

	mem::objbuf block_ai; // AI buffer

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

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//------------- DEFINES FOR 'EASY ACCESS' ;3 ---------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	namespace def
	{
		btui8 index = 0ui8; // waste of a byte
		btui8 a = 0ui8; // waste of a byte

		//#define tPos ent::t[index].position
		//#define tHgt ent::t[index].height
		//#define tVel ent::t[index].velocity
		//#define iCellX ent::t[index].cellx
		//#define iCellY ent::t[index].celly
		//#define aYaw ent::yaw[index]
		#define eCSI ((Entity*)_entities[index])->csi
		#define ePos ((Entity*)_entities[index])->t.position
		#define eHgt ((Entity*)_entities[index])->t.height
		#define eVel ((Entity*)_entities[index])->t.velocity
		//#define eCellX ((Entity*)_entities[index])->t.cellx
		//#define eCellY ((Entity*)_entities[index])->t.celly
		#define eCellX ((Entity*)_entities[index])->csi.c[eCELL_I].x
		#define eCellY ((Entity*)_entities[index])->csi.c[eCELL_I].y
		#define eYaw2 ((Entity*)_entities[index])->yaw
		#define eYaw(a) ((Entity*)_entities[a])->yaw
		// Animation
		#define aniLower ((Chara*)_entities[index])->animPlayer
		#define iAniID ((Chara*)_entities[index])->animPlayer.aniID
		#define iAniStep ((Chara*)_entities[index])->animPlayer.aniStep
		#define fAniTime ((Chara*)_entities[index])->animPlayer.aniTime
		#define bAniStepR ((Chara*)_entities[index])->aniStepR
		//#define bAniStepR ((Chara*)_chara[index])->charastatebv.get(Chara::ani_right_foot)
		#define resAnimID res::animations[((Chara*)_entities[index])->animPlayer.aniID]
		#define resAniStep res::animations[((Chara*)_entities[index])->animPlayer.aniID].steps[((Chara*)_entities[index])->animPlayer.aniStep]
		// chara
		#define f2Input ((Chara*)_entities[index])->input // set 1 underscore to test
		#define aViewYaw ((Chara*)_entities[index])->viewYaw
		#define aViewPitch ((Chara*)_entities[index])->viewPitch
		#define bMoving ((Chara*)_entities[index])->moving
		#define bvInRun ((Chara*)_entities[index])->inputbv.get(Actor::in_run)
		#define bvInAim ((Chara*)_entities[index])->inputbv.get(Actor::in_aim)
		#define bvInAtk ((Chara*)_entities[index])->inputbv.get(Actor::in_atk)
		#define fSpeed ((Chara*)_entities[index])->speed
		// Pathfinding
		#define aiNode ((Chara*)_entities[index])->node
		#define aiHasPath ((Chara*)_entities[index])->haspath
		#define aiPath ((Chara*)_entities[index])->pathnodes
		#define aiTarget ((Chara*)_entities[index])->target_ent

		#define accel 0.025f // Accelleration speed (now not relevant due to animation based motion)
		#define rotdeg 4.f // Rotation in degrees per frame when we hit a wall
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- PREFABS --------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	namespace prefab
	{
		enum prefabtype : btui8
		{
			prefab_player,
			prefab_ai_player,
			prefab_npc,
			prefab_zombie,
		};
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- FUNCTION DECLARATIONS ------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(prefab::prefabtype TYPE, m::Vector2 pos, float dir);
	// Removes a given Entity from the index
	void DestroyEntity(btID ID);

	btID GetClosestPlayer(btID id);
	btID GetClosestEntity(btID id, btf32 dist);
	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance);

	void ProjectileTick(btf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(btID id);
}