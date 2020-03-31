// For inclusion in index_decl.h only

// Inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "input.h"
#include "memory.h"
#include "memoryC.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

// these entire 2 includes only for one line...
#include "objects_items.h"
#include "core.h"

struct HeldItem;

class Inventory
{
public:
	mem::Buffer64<btID> items;
public:
	void AddNew(btID ITEM_TEMPLATE);
	void DestroyIndex(btui32 INDEX);
	void DestroyID(btID ITEM_ID);
	void Destroy(btID ITEM_TEMPLATE);
	void TransferItemRecv(btID ITEM_ID);
	void TransferItemSendIndex(btui32 INDEX);
	void TransferItemSend(btID ITEM_ID);
	//btID GetItemOfType(ItemType TYPE);
	btID GetItemOfTemplate(btID ITEM_TEMPLATE);
	btID GetItemOfAmmunitionType(btui8 AMMO_TYPE);
	void Draw(btui16 ACTIVE_SLOT);
};

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

#define STATE_DAMAGE_MAX 1000ui16
// TODO: merge with entity?? should be able to produce sfx
struct ActiveState
{
	// Global properties, ultimately to be used by every object in the game, incl. environment tiles
	enum ActiveFlags : btui64 // WIP
	{
		eALIVE = 1ui64,
		eDIED_REPORT = 1ui64 << 1ui64,
	};
	mem::bv<btui64, ActiveFlags> stateFlags;
	enum StaticFlags : btui64 // WIP
	{
		eNOTHING = 1ui64,
		eFLAMMABLE = 1ui64 << 1ui64,
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
void EntityTransformTick(Entity* ENTITY, btID ID, btf32 X, btf32 Y, btf32 Z);

char* DisplayNameActor(void* ent);
char* DisplayNameRestingItem(void* ent);

void TickRestingItem(void* ent, btf32 dt);
void TickChara(void* ent, btf32 dt);
void TickEditorPawn(void* ent, btf32 dt);

void DrawRestingItem(void* ent);
m::Vector3 SetFootPos(m::Vector2 position);
void DrawChara(void* ent);
void DrawEditorPawn(void* ent);

// Base entity class
struct Entity
{
	btID id;
	EntityType type;
	bti8 name[32];

	enum EntityFlags : btui8
	{
		// Basic properties
		eCOLLIDE_ENV = 1ui8, // Handle collision between this entity and the environment
		eCOLLIDE_ENT = 1ui8 << 1ui8, // Handle collision between this entity and other entities
		eCOLLIDE_PRJ = 1ui8 << 2ui8, // Handle collision between this entity and physical projectiles
		eCOLLIDE_MAG = 1ui8 << 3ui8, // Handle collision between this entity and magic effects
		eREPORT_TOUCH = 1ui8 << 4ui8, // Use callback function when another entity touches this one
		eNO_TICK = 1ui8 << 5ui8, // Do not tick every frame
		ePHYS_DRAG = 1ui8 << 6ui8, // Do not tick every frame
		//eALIGN_MESH_TO_GROUND = 1ui8 << 6ui8, // Align this object's mesh to the ground normal (useless because each class has its own draw fn anyway)
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
		IN_RUN = 0x1ui8 << 0x0ui8,
		IN_AIM = 0x1ui8 << 0x1ui8,
		IN_USE = 0x1ui8 << 0x2ui8,
		IN_USE_HIT = 0x1ui8 << 0x3ui8,
		IN_USE_ALT = 0x1ui8 << 0x4ui8,
		IN_ACTN_A = 0x1ui8 << 0x5ui8,
		IN_ACTN_B = 0x1ui8 << 0x6ui8,
		IN_ACTN_C = 0x1ui8 << 0x7ui8,
		IN_CROUCH = 0x1ui8 << 0x8ui8,

		IN_COM_ALERT = IN_USE | IN_USE_HIT | IN_USE_ALT,
	};

	//-------------------------------- Actor stuff

	m::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;
	// Movement stuff
	//bool moving = false;
	mem::bv<btui16, ActorInput> inputBV;

	//res::AssetConstantID t_skin; // The texture we use for drawing the character
	assetID t_skin;
	m::Vector3 skin_col_a;
	m::Vector3 skin_col_b;
	m::Vector3 skin_col_c;

	btf32 speed = 2.3f;
	btf32 agility = 0.f; // 0?? use agility to determine turning speed?

	Inventory inventory;
	btui32 inv_active_slot = 0u;

	btID atk_target = BUF_NULL;
	m::Angle atkYaw;

	//-------------------------------- AI stuff

	btf32 ai_vy_target = 0.f;
	btf32 ai_vp_target = 0.f;
	btID ai_target_ent = BUF_NULL;
	btID ai_ally_ent = BUF_NULL;
	bool aiControlled = false;

	void TakeItem(btID ID);
	void DropItem(btID SLOT);
	void DropAllItems();
	void SetEquipSlot(btui32 slot);
	void IncrEquipSlot();
	void DecrEquipSlot();
};
enum FootState : btui8
{
	eL_DOWN,
	eR_DOWN,
};
struct Chara : public Actor
{
	enum equipmode : btui8
	{
		spell,
		weapon,
	};

	enum CharaStaticProperties : btui8
	{
		eLEFT_HANDED = (0x1ui8 << 0x0ui8),
	};
	mem::bv<btui8, CharaStaticProperties> staticPropertiesBV;

	//enum CharaActiveState : btui8
	//{
	//	ani_right_foot = (0x1ui8 << 0x0ui8),
	//}; 
	//mem::bv<btui8, CharaActiveState> charastatebv;

	Transform3D t_body, t_head;
	m::Vector3 footPosTargR, footPosTargL, footPosR, footPosL;
	m::Vector2 ani_body_lean;
	bool aniSteppingL;
	bool aniSteppingR;
	btf32 aniStepAmountL;
	btf32 aniStepAmountR;
	graphics::Matrix4x4 matLegHipR, matLegUpR, matLegLoR, matLegFootR;
	graphics::Matrix4x4 matLegHipL, matLegUpL, matLegLoL, matLegFootL;
	bool aniCrouch = false;

	//FootState foot_state = eBOTH_DOWN;
	FootState foot_state = eL_DOWN;
};
struct EditorPawn : public Actor
{
	enum equipmode : btui8
	{
		spell,
		weapon,
	};

	enum CharaStaticProperties : btui8
	{
		eLEFT_HANDED = (0x1ui8 << 0x0ui8),
	};
	mem::bv<btui8, CharaStaticProperties> staticPropertiesBV;

	enum CharaActiveState : btui8
	{
		ani_right_foot = (0x1ui8 << 0x0ui8),
	};
	mem::bv<btui8, CharaActiveState> charastatebv;

	Transform3D t_body, t_head;
};
