// For inclusion in index_decl.h only

// Inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "time.hpp"
#include "input.h"
#include "memory.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

struct HeldItem;

struct ItemSlot
{
	btID item = ID_NULL;
	btui32 count = 1.f;
	HeldItem* heldInstance = nullptr;

	ItemSlot() {}
	ItemSlot(btID _item);
	~ItemSlot();
};

class Inventory
{
public:
	mem::CkBuffer<ItemSlot> items;
	Inventory() {};
	~Inventory() {};
private:
	inline void IncrStack(btui32 index);
	inline void DecrStack(btui32 index);
public:
	void AddItem(btID itemid);
	void RemvItem(btID itemid);
	void RemvItemAt(btui32 index);
	void Draw(btui16 active_slot);
};

enum ShaderStyle
{
	SS_NORMAL,
	SS_CHARA,
	SS_TERRAIN,
	SS_SKY,
};

void DrawMesh(btID ID, graphics::Mesh MODEL, graphics::TextureBase TEXTURE, ShaderStyle SHADER, glm::mat4 MATRIX);

void DrawMesh(btID ID, graphics::Mesh MODEL, graphics::TextureBase TEXTURE, ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawMesh(btID ID, graphics::Mesh MODEL, ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawBlendMesh(btID ID, graphics::MeshBlend MODEL, btf32 BLENDSTATE,
	graphics::TextureBase TEXTURE, ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawMeshDeform(btID ID, graphics::MeshDeform MODEL,
	graphics::TextureBase TEXTURE, ShaderStyle SHADER, btui32 MATRIX_COUNT,
	graphics::Matrix4x4 MATRIX_A, graphics::Matrix4x4 MATRIX_B,
	graphics::Matrix4x4 MATRIX_C, graphics::Matrix4x4 MATRIX_D);

//transform
class Transform2D
{
public:
	// world space position
	m::Vector2 position;
	m::Vector2 velocity;
	btf32 height = 0.f;
	btf32 height_velocity = 0.f;
	btui8 cellx = 0ui8;
	btui8 celly = 0ui8;
};

class TransformEntity
{
	Transform2D t;
	CellSpace cs;
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//------------- ENTITY STRUCTS -----------------------------------
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

struct ActiveState
{
	// Global properties, ultimately to be used by every object in the game, incl. environment tiles
	enum globalProperty : btui64 // WIP
	{
		eALIVE = 1ui64,
		eFLAMMABLE = 1ui64 << 1ui64,
		eON_FIRE = 1ui64 << 2ui64,
	};
	mem::bv<btui64, globalProperty> properties;
	btf32 hp = 1.f;

	void Damage(btf32 AMOUNT, btf32 ANGLE);
};

// Base entity class
struct Entity
{
	virtual bool IsActor() { return false; };
	virtual bool IsActivator() { return false; };
	virtual bool IsRestingItem() { return false; };
	virtual char* GetDisplayName() { return "Entity";};

	enum EntityFlags : btui8
	{
		// Basic properties
		eCOLLIDE_ENV = 1ui8, // Handle collision between this entity and the environment
		eCOLLIDE_ENT = 1ui8 << 1ui8, // Handle collision between this entity and other entities
		eCOLLIDE_PRJ = 1ui8 << 2ui8, // Handle collision between this entity and physical projectiles
		eCOLLIDE_MAG = 1ui8 << 3ui8, // Handle collision between this entity and magic effects
		eREPORT_TOUCH = 1ui8 << 4ui8, // Use callback function when another entity touches this one
		eNO_TICK = 1ui8 << 5ui8, // Do not tick every frame
		//eALIGN_MESH_TO_GROUND = 1ui8 << 6ui8, // Align this object's mesh to the ground normal (useless because each class has its own draw fn anyway)
		// Can go up to 1 << 7

		ePREFAB_FULLSOLID = eCOLLIDE_ENV | eCOLLIDE_ENT | eCOLLIDE_PRJ | eCOLLIDE_MAG,
		//ePREFAB_ITEM = eCOLLIDE_ENV | eNO_TICK | eALIGN_MESH_TO_GROUND,
		ePREFAB_ITEM = eCOLLIDE_ENV | eNO_TICK,
	};
	// Entity base properties
	mem::bv<btui8, EntityFlags> properties;

	fac::faction faction;
	ActiveState state;
	btui8 statebuffer[32 - sizeof(ActiveState)]; // reserved space for save / load data

	bool freeTurn;
	btf32 radius = 0.5f; // Radius of the entity (no larger than .5)
	btf32 height = 1.9f; // Height of the entity cylinder
	Transform2D t;
	m::Angle yaw;

	CellSpace csi; // Where we are in cell space

	virtual void Tick(btID INDEX, btf32 DELTA_TIME);
	virtual void Draw(btID INDEX);
};
// Entity type representing placed items
struct EItem : public Entity
{
	virtual bool IsActivator() { return true; };
	virtual bool IsRestingItem() { return true; };
	virtual char* GetDisplayName() { return (char*)acv::items[itemid]->name; };

	btID itemid;
	Transform3D t_item;

	virtual void Tick(btID INDEX, btf32 DELTA_TIME);
	virtual void Draw(btID INDEX);
};
struct Actor : public Entity
{
	virtual bool IsActor() { return true; };
	virtual char* GetDisplayName() { return "Actor"; };

	enum ActorInput : btui8
	{
		IN_RUN = 0x1ui8 << 0x0ui8,
		IN_AIM = 0x1ui8 << 0x1ui8,
		IN_USE = 0x1ui8 << 0x2ui8,
		IN_USE_HIT = 0x1ui8 << 0x3ui8,
		IN_USE_ALT = 0x1ui8 << 0x4ui8,
		IN_ACTN_A = 0x1ui8 << 0x5ui8,
		IN_ACTN_B = 0x1ui8 << 0x6ui8,
		IN_ACTN_C = 0x1ui8 << 0x7ui8,

		IN_COM_ALERT = IN_USE | IN_USE_HIT | IN_USE_ALT,
	};

	//-------------------------------- Actor stuff

	m::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;
	// Movement stuff
	bool moving = false;
	mem::bv<btui8, ActorInput> inputBV;

	//res::AssetConstantID t_skin; // The texture we use for drawing the character
	assetID t_skin;

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

	void PickUpItem(btID ID);
	void DropItem(btID SLOT);
	void SetEquipSlot();
	void IncrEquipSlot();
	void DecrEquipSlot();

	virtual void Tick(btID INDEX, btf32 DELTA_TIME);
	virtual void Draw(btID INDEX);
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

	enum CharaActiveState : btui8
	{
		ani_right_foot = (0x1ui8 << 0x0ui8),
	}; 
	mem::bv<btui8, CharaActiveState> charastatebv;


	Transform3D t_body, t_head;
	m::Vector3 footPosTargR, footPosTargL, footPosR, footPosL;
	m::Vector2 ani_body_lean;
	graphics::Matrix4x4 matLegHipR, matLegUpR, matLegLoR, matLegFootR;
	graphics::Matrix4x4 matLegHipL, matLegUpL, matLegLoL, matLegFootL;
	enum FootState : btui8
	{
		eL_DOWN,
		eR_DOWN,
		eBOTH_DOWN,
	};
	FootState foot_state = eBOTH_DOWN;

	virtual void Tick(btID INDEX, btf32 DELTA_TIME);
	virtual void Draw(btID INDEX);

protected:
	m::Vector3 SetFootPos(m::Vector2 POSITION);
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

	virtual void Tick(btID INDEX, btf32 DELTA_TIME);
	virtual void Draw(btID INDEX);
};