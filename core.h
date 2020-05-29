#ifndef CORE_H
#define CORE_H

#include "global.h"

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
	struct idbuf;
}

extern btui64 tickCount_temp;

namespace index
{
	void Init();
	void End();

	void ClearBuffers();

	void Tick(btf32 DELTA);
	void TickGUI();

	void GUISetMessag(int player, char* string);
	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw(btf64 delta);

	void SetViewFocus(btID i);
	m::Vector2 GetViewOffset();

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(btui8 TYPE_PREFAB_TEMP, m::Vector2 pos, float dir);
	// Removes a given Entity from the index
	void DestroyEntity(btID ID);

	btID SpawnNewEntityItem(btID ITEM_TEMPLATE, m::Vector2 POSITION, btf32 DIRECTION);
	btID SpawnEntityItem(btID ITEMID, m::Vector2 POSITION, btf32 DIRECTION);

	inline void spawn_setup_t(btID index, m::Vector2 pos, btf32 dir);

	// Create a new item instance of type TYPE
	btID SpawnItem(btID ITEM_TEMPLATE);
	// Destroy the item at ID
	void DestroyItem(btID ID);

	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectile(fac::faction FACTION, btID TEMPLATE_TYPE, m::Vector2 POSITION, btf32 HEIGHT,
		float YAW, float PITCH);
	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectileSpread(fac::faction FACTION, btID TEMPLATE_TYPE, m::Vector2 POSITION, btf32 HEIGHT,
		float YAW, float PITCH, float SPREAD);
	// Removes a given projectile from the index
	void DestroyProjectile(btID ID);

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	// Get HP (Health Points) of entity at X ID
	btui16 GetHP(btID id);

	// Set shadow texture ID (TODO: hacky, get rid of this)
	void SetShadowTexture(btui32 ID);

	extern btui64 spawnz_time_temp;

	// TODO: MOVE THIS SECTION TO INDEX

	extern unsigned int activePlayer;
	extern btID players[2];
	extern btID viewtarget[2];

	void SetInput(btID PLAYER_INDEX, m::Vector2 INPUT, btf32 YAW, btf32 PITCH,
		bool WantAttack, bool use_hit, bool WantAttack2,
		bool RUN, bool AIM, bool ACTION_A, bool ACTION_B, bool ACTION_C,
		bool crouch, bool jump);

	void SetViewTargetID(btID ID, btui32 player);
	btID GetViewTargetID(btui32 player);

	void EntDeintersect(Entity* ENT, CellSpace& CSI);
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

#endif
