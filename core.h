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

extern btui64 tickCount;

namespace core
{
	void Init();
	void End();

	void ClearBuffers();
	void RegenCellRefs();

	void Tick(btf32 delta);
	void TickGUI();

	void GUISetMessag(int player, char* string);
	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw(btf64 delta);

	void SetViewFocus(btID i);

	//-------------------------------- SPAWN FUNCTIONS

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(btui8 type_prefab_temp, m::Vector2 pos, float dir);
	// Spawn an item on the ground, initializing a new item instance
	btID SpawnNewEntityItem(btID item_template, m::Vector2 position, btf32 direction);
	// Spawn an item on the ground, referencing an existing item instance
	btID SpawnEntityItem(btID itemid, m::Vector2 position, btf32 height, btf32 direction);
	// Removes a given Entity from the index
	void DestroyEntity(btID id);

	// Create a new item instance of type TYPE
	btID SpawnItem(btID item_template);
	// Destroy the item at ID
	void DestroyItem(btID id);

	// Create a new activator
	btID SpawnActivator(btui32 x, btui32 y);
	// Destroy an activator
	void DestroyActivator(btID id);

	// Creates a projectile instance, allocates an ID
	void SpawnProjectile(fac::faction faction, btID template_type, m::Vector2 position, btf32 height,
		float yaw, float pitch);
	// Creates a projectile instance, allocates an ID, includes a random spread amount
	void SpawnProjectileSpread(fac::faction faction, btID template_type, m::Vector2 position, btf32 height,
		float yaw, float pitch, float spread);
	// Removes a given projectile from the index
	void DestroyProjectile(btID id);

	//-------------------------------- idk

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	// Get HP (Health Points) of entity at X ID
	btui16 GetHP(btID id);

	// Set shadow texture ID (TODO: hacky, get rid of this)
	void SetShadowTexture(btui32 id);

	extern btui64 spawnz_time_temp;

	extern unsigned int activePlayer;
	extern btID players[2];

	int CellEntityCount(int x, int y);
	btID CellEntity(int x, int y, int e);

	void SetPlayerInput(btID player_index, m::Vector2 input, btf32 yaw, btf32 pitch,
		bool wantattack, bool use_hit, bool wantattack2,
		bool run, bool aim, bool action_a, bool action_b, bool action_c,
		bool crouch, bool jump);

	//void EntDeintersect(Entity* ENT, CellSpace& CSI);
	void ActorCastProj(btID id);

	// Adds this entity to the local vector of this cell
	void AddEntityCell(btui32 x, btui32 y, btID entity);
	// Removes this entity from the local vector of this cell
	void RemoveEntityCell(btui32 x, btui32 y, btID entity);

	bool LOSCheck(btID ent_this, btID ent_targ);
	btID GetClosestPlayer(btID index);
	btID GetClosestEntity(btID index, btf32 distance);
	btID GetViewTargetEntity(btID index, btf32 distance, fac::facalleg allegiance);
	btID GetClosestEntityAlleg(btID index, btf32 distance, fac::facalleg allegiance);
	btID GetClosestEntityAllegLOS(btID index, btf32 distance, fac::facalleg allegiance);
	btID GetClosestEntityButDifferent(btID index);
}

#endif
