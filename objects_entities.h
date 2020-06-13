#ifndef ENTITY_H
#define ENTITY_H

// Inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "input.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

#include "objects_inventory.h"

// these entire 2 includes only for one line...
#include "objects_items.h"
#include "core.h"

struct HeldItem;

//transform
class Transform2D
{
public:
	// world space position
	m::Vector2 position;
	m::Vector2 velocity;
	btf32 height = 0.f;
	btf32 height_velocity = 0.f;
	m::Angle yaw;
	CellSpace csi; // Where we are in cell space
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//------------- ENTITY STRUCTS -----------------------------------
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

enum StatusEffectType : btui16 {
	EFFECT_DAMAGE_HP,
	EFFECT_RESTORE_HP,
	EFFECT_SLOW_TIME,
	EFFECT_BURDEN_ITEM,
	EFFECT_BURN,
	EFFECT_FREEZE,
	EFFECT_TRANSFER_HP,
	EFFECT_LEVITATE,
	EFFECT_WATER_WALK,
	EFFECT_WATER_BREATHE,
	EFFECT_PARALYZE, // lunaris' spells
	EFFECT_SPEED_INCREASE,
	EFFECT_SPEED_DECREASE,
	EFFECT_STUN,
	EFFECT_SLEEP,
	EFFECT_MINDCONTROL,
	EFFECT_CHARM,
	EFFECT_FEAR,
	EFFECT_FRENZY,
	EFFECT_SILENCE,
	EFFECT_BLIND,
	EFFECT_SHIELD,
	// non-spell efect
	EFFECT_LEAD_POISON, // effect gained from eating bullets
};

typedef struct StatusEffect {
	btID effect_caster_id;
	btui16 effect_type;
	btf32 effect_duration;
	btui32 effect_magnitude;
	btui32 reserved;
} StatusEffect;

#define STATE_DAMAGE_MAX 1000u
// TODO: merge with entity?? should be able to produce sfx
struct ActiveState
{
	// Global properties, ultimately to be used by every object in the game, incl. environment tiles
	enum ActiveFlags : btui64 // WIP
	{
		eALIVE = 1u,
		eDIED_REPORT = 1u << 1u,
	};
	mem::bv<btui64, ActiveFlags> stateFlags;
	enum StaticFlags : btui64 // WIP
	{
		eNOTHING = 1u,
		eFLAMMABLE = 1u << 1u,
	};
	mem::bv<btui64, StaticFlags> properties2;
	btui16 damagestate = STATE_DAMAGE_MAX;

	mem::Buffer32<StatusEffect> effects;

	void Damage(btui32 AMOUNT, btf32 ANGLE);
	void AddEffect(btID CASTER, StatusEffectType TYPE, btf32 DURATION, btui32 MAGNITUDE);
	void AddSpell(btID CASTER, btID SPELL);
	void TickEffects(btf32 DELTA_TIME);
};

// Handle this entitiy's position, receives desired motion
void Entity_PhysicsTick(Entity* ENTITY, btID ID, btf32 DELTA_TIME);

char* DisplayNameActor(void* ent);
char* DisplayNameRestingItem(void* ent);

void TickRestingItem(void* ent, btf32 dt);
void DrawRestingItem(void* ent);

void TickChara(void* ent, btf32 dt);
void DrawChara(void* ent);

void TickEditorPawn(void* ent, btf32 dt);
void DrawEditorPawn(void* ent);

// does this need to be here?
m::Vector3 Actor_SetFootPos(m::Vector2 position);

// Base entity class
struct Entity
{
	btID id;
	EntityType type;
	bti8 name[32];

	enum EntityFlags : btui8
	{
		// Basic properties
		eCOLLIDE_ENV = 1u, // Handle collision between this entity and the environment
		eCOLLIDE_ENT = 1u << 1u, // Handle collision between this entity and other entities
		eCOLLIDE_MAG = 1u << 3u, // Handle collision between this entity and magic effects
		eCOLLIDE_PRJ = 1u << 2u, // Handle collision between this entity and physical projectiles
		eREPORT_TOUCH = 1u << 4u, // Use callback function when another entity touches this one
		eNO_TICK = 1u << 5u, // Do not tick every frame
		ePHYS_DRAG = 1u << 6u, // Do not tick every frame
		//eALIGN_MESH_TO_GROUND = 1u << 6ui8, // Align this object's mesh to the ground normal (useless because each class has its own draw fn anyway)
		// Can go up to 1 << 7

		ePREFAB_FULLSOLID = eCOLLIDE_ENV | eCOLLIDE_ENT | eCOLLIDE_PRJ | eCOLLIDE_MAG,
		ePREFAB_ITEM = eCOLLIDE_ENV | eCOLLIDE_ENT | eNO_TICK | ePHYS_DRAG,
	};
	// Entity base properties
	mem::bv<btui8, EntityFlags> properties;

	fac::faction faction;
	ActiveState state;
	//btui8 statebuffer[32 - sizeof(ActiveState)]; // reserved space for save / load data

	btf32 radius = 0.5f; // Radius of the entity (no larger than .5)
	btf32 height = 1.9f; // Height of the entity cylinder
	Transform2D t;
	// foot slide for slippery surfaces / knockback etc.
	m::Vector2 slideVelocity;
	bool grounded = true;
};
// Entity type representing placed items
struct RestingItem : public Entity
{
	btID item_instance;
	graphics::Matrix4x4 matrix;
};
struct Actor : public Entity
{
	enum ActorInput : btui16
	{
		IN_RUN = 0x1u << 0x0u,
		IN_AIM = 0x1u << 0x1u,
		IN_USE = 0x1u << 0x2u,
		IN_USE_HIT = 0x1u << 0x3u,
		IN_USE_ALT = 0x1u << 0x4u,
		IN_ACTN_A = 0x1u << 0x5u,
		IN_ACTN_B = 0x1u << 0x6u,
		IN_ACTN_C = 0x1u << 0x7u,
		IN_CROUCH = 0x1u << 0x8u,
		IN_JUMP = 0x1u << 0x9u,

		IN_COM_ALERT = IN_USE | IN_USE_HIT | IN_USE_ALT,
	};
	mem::bv<btui16, ActorInput> inputBV;
	m::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;

	//-------------------------------- Actor stuff

	btui8 actorBase = 0u;

	m::Vector3 skin_col_a;
	m::Vector3 skin_col_b;
	m::Vector3 skin_col_c;

	btf32 speed = 2.3f;
	btf32 agility = 0.f; // 0?? use agility to determine turning speed?

	Inventory inventory;
	btui32 inv_active_slot = 0u;

	btID atk_target = BUF_NULL;
	m::Angle atkYaw;

	//-------------------------------- CHARA stuff

	enum CharaStaticProperties : btui8
	{
		eLEFT_HANDED = (0x1u << 0x0u),
	};
	mem::bv<btui8, CharaStaticProperties> staticPropertiesBV;

	enum FootState : btui8
	{
		eL_DOWN,
		eR_DOWN,
	};
	FootState foot_state = eL_DOWN;

	enum JumpState : btui8
	{
		eJUMP_NEITHER,
		eJUMP_JUMP,
		eJUMP_SPRINT,
	};

	JumpState jump_state = eJUMP_NEITHER;

	enum CharaAniFlags : btui8
	{
		eANI_TEMP_1 = 1u,
		eANI_LANDED = 1u << 1u,
		eANI_TEMP_3 = 1u << 3u,
		eANI_TEMP_4 = 1u << 2u,
		eANI_TEMP_5 = 1u << 4u,
		eANI_TEMP_6 = 1u << 5u,
		eANI_TEMP_7 = 1u << 6u,
		eANI_TEMP_8 = 1u << 7u,
		
		eANI_CLEAR = 0b11111111u,
	};
	mem::bv<btui8, CharaAniFlags> animationBV;

	Transform3D t_body, t_head;
	m::Vector3 footPosTargR, footPosTargL, footPosR, footPosL, fpCurrentR, fpCurrentL;
	m::Vector2 ani_body_lean;
	btf32 aniStepAmountL;
	btf32 aniStepAmountR;
	btf32 aniStandHeight;
	bool aniCrouch = false;
	btf32 aniSlideResponse = 0.f;
	bool aniRun = true;

	//-------------------------------- AI stuff

	btf32 ai_vy_target = 0.f;
	btf32 ai_vp_target = 0.f;
	btID ai_target_ent = BUF_NULL;
	btID ai_ally_ent = BUF_NULL;
	bool aiControlled = false;
	path::Path ai_path;
	btui8 ai_path_current_index = 0u;
	bool ai_pathing = false;

	void TakeItem(btID ID);
	void DropItem(btID SLOT);
	void DropAllItems();
	void SetEquipSlot(btui32 slot);
	void IncrEquipSlot();
	void DecrEquipSlot();
};
struct EditorPawn : public Actor
{
	Transform3D t_body, t_head;
};

#endif
