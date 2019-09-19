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

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- FUCKING TEMPORARY AS, MOVE TO OBJECTS.H ASAP -------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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

		struct AnimPlayer
		{
			btui8 aniID = 0u; // Which animation to play
			btui8 aniStep = 0u; // which step in the animation
			btf32 aniTime = 0.f; // how far through this animation step are we

			enum advstate : btui8
			{
				eNOEVENT,
				eADVSTEP,
				eENDOFANIM,
			};

			advstate AdvanceAnim(btf32 amount)
			{
				aniTime += amount;
				if (aniTime > res::animations[aniID].steps[aniStep].time) // Check time overflow (end of step)
				{
					++aniStep; // Iterate step
					aniTime = 0.f; // Reset timer
					if (aniStep == res::animations[aniID].size) // Check step overflow (end of animation)
					{
						aniStep = 0ui8;
						return eENDOFANIM;
					}
					return eADVSTEP;
				}
				return eNOEVENT;
			}
			void setAnim(res::aniarr anim)
			{
				aniTime = 0.f; // Reset timer
				aniStep = 0ui8; // Reset step
				aniID = anim; // Set current animation
			}
			btf32 GetCurve()
			{
				/*
					btf32 normalTime = aniTime / res::animations[aniID].steps[aniStep].time;
					//std::cout << "normaltime: " << normalTime << std::endl;
					//std::cout << "anitime: " << aniTime << std::endl;

					btui32 lower = (btui32)floorf(normalTime * 16.f);
					btui32 upper = lower + 1u;
					btf32 remainder = normalTime - (lower / 16.f);
					//if (upper == 16) return res::anim_curve[lower];
					//else return m::Lerp(res::anim_curve[lower], res::anim_curve[upper], remainder);

					//btui32 lower = (btui32)floorf(aniTime * 16.f);
					//btui32 upper = (btui32)ceilf(aniTime * 16.f) + 1u;
					//btf32 remainder = (aniTime * 16.f) - lower;
					////return res::anim_curve[(int)roundf(time * 16.f)];
					//return res::anim_curve[lower];
					//return m::Lerp(res::anim_curve[lower], res::anim_curve[upper], remainder) / res::animations[aniID].steps[aniStep].time;
					*/

				return aniTime / res::animations[aniID].steps[aniStep].time; // working linear result
			}
		};
	};
	struct Chara : public Actor
	{
		virtual etype::etype Type() { return etype::chara; };
		//Chara(btID ID) : Actor(ID) {};

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
		index::actor::AnimPlayer animPlayer; // Leg animation
		btID lookTarget = BUF_NULL; // What it's looking at

		Musket musket;

		Transform3D t_body, t_head;
	};

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- GLOBAL VARIABLES -----------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	float networkTimerTemp;

	btID players[2];
	m::Vector2 viewpos;

	btID activeplayer;

	Transform3D line_transform;

	struct cell
	{
		mem::idbuf ents;
	};

	cell cells[WORLD_SIZE][WORLD_SIZE];

	// inventory stuff
	graphics::GUIBox guibox;
	graphics::GUIBox guibox_selection;
	graphics::GUIText text;
	graphics::GUIText invtext;
	btui16 inv_active_slot = 0u;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- ENTITY BUFFERS -------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
		graphics::Matrix4x4 smokePoints[PROJ_TRAIL_NUM];
		btf32 smokePointTime[PROJ_TRAIL_NUM] { 0.f };
		btui8 smokePointIndex = 0ui8;
		btf32 distance_travelled = 0.f;
	};
	Proj proj[BUF_SIZE];

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//------------- DEFINES FOR 'EASY ACCESS' ;3 ---------------------

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
		#define eCellX ((Entity*)_entities[index])->t.cellx
		#define eCellY ((Entity*)_entities[index])->t.celly
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

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- PREFABS --------------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- FUNCTION DECLARATIONS ------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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