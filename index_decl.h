// For use in index_fn.h only

#define CUTE_C2_IMPLEMENTATION
#include "3rdparty/cute_c2.h"

#include "objects.h"

namespace index
{
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- FUCKING TEMPORARY AS, MOVE TO OBJECTS.H ASAP -------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	namespace ent
	{
		namespace type
		{
			enum prefabtype : btui8
			{
				prefab_player,
				prefab_npc,
				prefab_inanimate_bb,
			};
		}

		struct ent_state
		{
			bool alive = true;
			btf32 hp = 1.f;
		};

		// Variables
		//type::fntype type22[BUF_SIZE]; // what type of entity is this?
		fac::faction faction[BUF_SIZE];
		ent_state state[BUF_SIZE];
		float radius[BUF_SIZE]{ 0.5f }; // Radius of the entity (no larger than .5)
		Transform2D t[BUF_SIZE];
		m::Angle yaw[BUF_SIZE];
	};
	// Actor variables
	namespace actor
	{
		namespace equipmode
		{
			enum equipmode : btui8
			{
				spell,
				weapon,
			};
		}

		struct anim_player
		{
			btID			aniID = 0u; // Which animation to play
			btui8			aniStep = 0u; // which step in the animation
			btf32			aniTime = 0.f; // how far through this animation are we

			enum advstate : btui8
			{
				eNOEVENT,
				eADVSTEP,
				eENDOFANIM,
			};

			advstate AdvanceAnim(btf32 amount)
			{
				aniTime += amount;
				if (aniTime > pRes->anims[aniID].steps[aniStep].time) // Check time overflow (end of step)
				{
					++aniStep; // Iterate step
					aniTime = 0.f; // Reset timer
					if (aniStep == pRes->anims[aniID].size) // Check step overflow (end of animation)
					{
						aniStep = 0ui8;
						return eENDOFANIM;
					}
					return eADVSTEP;
				}
				return eNOEVENT;
			}
			void setAnim(res::anims anim)
			{
				aniTime = 0.f; // Reset timer
				aniStep = 0ui8; // Reset step
				aniID = anim; // Set current animation
			}
		};
	};
	struct Chara : public Actor
	{
		virtual etype Type() { return chara; };

		enum equipmode : btui8
		{
			spell,
			weapon,
		};
		enum chara_state : btui8
		{
			ani_right_foot = (0x1ui8 << 0x0ui8),
			reloading = (0x1ui8 << 0x1ui8),
		};
		// Inventory stuff
		btID equipped_item = BUF_NULL; // Everything that moves can hold an item
		equipmode equip_mode;
		// Animation stuff
		bool aniStepR = false; // Which foot is forwards right now
		mem::bv<btui8, chara_state> charastatebv;
		index::actor::anim_player ani_Lower; // Leg animation
		index::actor::anim_player ani_Upper; // Body animation
											 // What it's looking at
		btID lookTarget = BUF_NULL;

		Musket musket;
	};

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- GLOBAL VARIABLES -----------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	float networkTimerTemp;

	btID players[2];
	fw::Vector2 viewpos;

	btID activeplayer;

	Transform3D line_transform;

	struct cell
	{
		mem::idbuf ents;
	};

	cell cells[WORLD_SIZE][WORLD_SIZE];

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- ENTITY FUNCTION POINTERS ---------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	void TickEntity(btf32 dt, btID id);
	void TickChara(btf32 dt, btID id);
	void TickMusket(btf32 dt, btID id);
	// Function pointers for different entity types
	void(*const tick[])(btf32, btID) = {
		TickEntity,
		TickEntity,
		TickChara,
		TickMusket,
	};
	// Function pointers for different entity types
	//void(*const draw[])(btID) = {
	//	EntityDraw,
	//};

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- ENTITY BUFFERS -------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//block of IDs in memory, tracks the numbers and IDs of any type of object
	mem::objbuf block_entity; // Entity buffer
	Entity* _entities[BUF_SIZE]; // Character buffer

	mem::objbuf block_ai; // AI buffer

	mem::objbuf_caterpillar block_proj; // Projectile buffer
	// Projectile variables
	struct Proj
	{
		Transform2D t;
		btf32 ttd;
		bool grounded;
	};
	Proj proj[BUF_SIZE];

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//------------- DEFINES FOR 'EASY ACCESS' ;3 ---------------------

	namespace def
	{
		btui8 index = 0ui8; // waste of a byte

		#define tPos ent::t[index].position
		#define tHgt ent::t[index].height
		#define tVel ent::t[index].velocity
		#define iCellX ent::t[index].cellx
		#define iCellY ent::t[index].celly
		#define aYaw ent::yaw[index]
		// Animation
		#define aniLower ((Chara*)_entities[index])->ani_Lower
		#define iAniID ((Chara*)_entities[index])->ani_Lower.aniID
		#define iAniStep ((Chara*)_entities[index])->ani_Lower.aniStep
		#define fAniTime ((Chara*)_entities[index])->ani_Lower.aniTime
		#define bAniStepR ((Chara*)_entities[index])->aniStepR
		//#define bAniStepR ((Chara*)_chara[index])->charastatebv.get(Chara::ani_right_foot)
		#define resAnimID pRes->anims[((Chara*)_entities[index])->ani_Lower.aniID]
		#define resAniStep pRes->anims[((Chara*)_entities[index])->ani_Lower.aniID].steps[((Chara*)_entities[index])->ani_Lower.aniStep]
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

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- FUNCTION DECLARATIONS ------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(ent::type::prefabtype TYPE, fw::Vector2 pos, float dir);
	// Removes a given Entity from the index
	void DestroyEntity(btID ID);

	btID GetClosestEntity(btID id, btf32 dist);

	void EntityDraw();
	void ProjectileTick(btf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(btID id);
}