#pragma once

//Forward declarations
namespace m
{
	class Vector3;
	class Vector2;
}
class Transform2D;
namespace graphics
{
	struct Texture;
}
class Transform3D;

namespace fac
{
	enum faction : btui8;
	enum facalleg : btui8;
}

struct CellGroup;
struct CellSpace;

struct Entity;
struct Chara;
struct Actor;
struct RestingItem;

//extern enum EntityType;

struct HeldItem;

namespace mem
{
	struct objbuf;
}

struct ObjBuf;

namespace index
{
	void Init();
	void End();

	void ClearBuffers();

	void Tick(btf32 DELTA);
	void TickGUI();

	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw();

	void SetViewFocus(btID i);
	m::Vector2 GetViewOffset();

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(btui8 TYPE_PREFAB_TEMP, m::Vector2 pos, float dir);
	// Removes a given Entity from the index
	void DestroyEntity(btID ID);

	btID SpawnNewEntityItem(btID ITEM_TEMPLATE, m::Vector2 POSITION, btf32 DIRECTION);
	btID SpawnEntityItem(btID ITEMID, m::Vector2 POSITION, btf32 DIRECTION);

	// Create a new item instance of type TYPE
	btID SpawnItem(btID ITEM_TEMPLATE);
	// Destroy the item at ID
	void DestroyItem(btID ID);

	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectile(fac::faction FACTION, m::Vector2 POSITION, btf32 HEIGHT, float YAW, float PITCH, float SPREAD);
	// Removes a given projectile from the index
	void DestroyProjectile(btID ID);

	// TODO: move to index
	void IndexInitEntity(btID id, EntityType type);
	void IndexFreeEntity(btID id);
	void IndexInitItem(btID id, ItemType type);
	void IndexFreeItem(btID id);

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	// Get HP (Health Points) of entity at X ID
	btf32 GetHP(btID id);

	// Set shadow texture ID (to do: hacky, get rid of this)
	void SetShadowTexture(btui32 ID);

	// TODO: MOVE THIS SECTION TO INDEX

	extern btID players[2];

	//block of IDs in memory, tracks the numbers and IDs of any type of object
	extern mem::objbuf block_entity; // Entity buffer
	typedef struct EntAddr
	{
		EntityType type;
		btID type_buffer_index;
	} EntAddr;
	extern EntAddr block_entity_data[BUF_SIZE];
	extern mem::objbuf buf_resting_item;
	extern RestingItem buf_resting_item_data[BUF_SIZE];
	extern mem::objbuf buf_chara;
	extern Chara       buf_chara_data[BUF_SIZE];

	// Get the pointer address of the entity at X ID
	void* GetEntityPtr(btID ID);

	extern ObjBuf block_item; // Item buffer
	extern HeldItem* items[BUF_SIZE];

	// Get the pointer of the item at ID
	HeldItem* GetItemPtr(btID ID);

	#define ENT_VOID(a) (index::GetEntityPtr(a))
	#define CHARA(a) ((Chara*)index::GetEntityPtr(a))
	#define ACTOR(a) ((Actor*)index::GetEntityPtr(a))
	#define ENTITY(a) ((Entity*)index::GetEntityPtr(a))
	#define ITEM(a) ((RestingItem*)index::GetEntityPtr(a))

	void SetInput(btID INDEX, m::Vector2 INPUT, btf32 YAW, btf32 PITCH, bool WantAttack, bool use_hit, bool WantAttack2,
		bool RUN, bool AIM, bool ACTION_A, bool ACTION_B, bool ACTION_C);

	void SetViewTargetID(btID ID, btui32 player);
	btID GetViewTargetID(btui32 player);

	void EntDeintersect(Entity* ENT, CellSpace& CSI, btf32 ROT, bool KNOCKBACK);
	void ActorRunAI(btID ID);
	void ActorCastProj(btID ID);

	// Adds this entity to the local vector of this cell
	void AddEntityCell(btui32 X, btui32 Y, btID ENTITY);
	// Removes this entity from the local vector of this cell
	void RemoveEntityCell(btui32 X, btui32 Y, btID ENTITY);

	bool LOSCheck(btID ENT_THIS, btID ENT_TARG);
	btID GetClosestPlayer(btID INDEX);
	btID GetClosestEntity(btID INDEX, btf32 DISTANCE);
	btID GetViewTargetEntity(btID INDEX, btf32 DISTANCE, fac::facalleg ALLEGIANCE);
	btID GetClosestEntityAlleg(btID INDEX, btf32 DISTANCE, fac::facalleg ALLEGIANCE);
	btID GetClosestEntityAllegLOS(btID INDEX, btf32 DISTANCE, fac::facalleg ALLEGIANCE);
	btID GetClosestActivator(btID INDEX);
}