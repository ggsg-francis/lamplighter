#ifndef CORE_H
#define CORE_H

#include "global.h"

//Forward declarations
namespace m
{
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
	enum faction : lui8;
	enum facalleg : lui8;
}

struct CellGroup;
struct CellSpace;

extern lui64 tickCount;

namespace core
{
	void Init();
	void InitEditMode();
	void End();

	// Clear all entities from memory
	void ClearBuffers();
	// Regenerate cell references
	void RegenCellRefs();

	bool Tick(lf32 delta);
	bool TickEditor(lf32 delta);
	void TickGUI();

	void GUISetMessag(int player, char* string);
	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw(lf64 delta);

	void SetViewFocus(lid i);

	//-------------------------------- SPAWN FUNCTIONS

	// Creates an Entity instance, adds it to the index and allocates it an ID
	lid SpawnEntity(lui8 type_prefab_temp, m::Vector2 pos, float dir);
	// Spawn an item on the ground, initializing a new item instance
	lid SpawnNewEntityItem(lid item_template, m::Vector2 position, lf32 direction);
	// Spawn an item on the ground, referencing an existing item instance
	lid SpawnEntityItem(lid itemid, m::Vector2 position, lf32 height, lf32 direction);
	// Removes a given Entity from the index
	void DestroyEntity(lid id);

	// Create a new item instance of type TYPE
	lid SpawnItem(lid item_template);
	// Destroy the item at ID
	void DestroyItem(lid id);

	// Creates a projectile instance, allocates an ID
	void SpawnProjectile(fac::faction faction, lid template_type, m::Vector2 position, lf32 height,
		float yaw, float pitch);
	// Creates a projectile instance, allocates an ID, includes a random spread amount
	void SpawnProjectileSpread(fac::faction faction, lid template_type, m::Vector2 position, lf32 height,
		float yaw, float pitch, float spread);
	// Removes a given projectile from the index
	void DestroyProjectile(lid id);

	//-------------------------------- idk

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	// Set shadow texture ID (TODO: hacky, get rid of this)
	void SetShadowTexture(lui32 id);

	extern unsigned int activePlayer;
	extern lid players[NUM_PLAYERS];

	int CellEntityCount(int x, int y);
	lid CellEntity(int x, int y, int e);

	void SetPlayerInput(lid player_index, m::Vector2 input, lf32 yaw, lf32 pitch,
		bool wantattack, bool use_hit, bool wantattack2,
		bool run, bool aim, bool action_a, bool action_b, bool action_c,
		bool crouch, bool jump);

	void CheckPlayerAI();

	//void EntDeintersect(Entity* ENT, CellSpace& CSI);
	void ActorCastProj(lid id);

	// Adds this entity to the local vector of this cell
	void AddEntityCell(lui32 x, lui32 y, lid entity);
	// Removes this entity from the local vector of this cell
	void RemoveEntityCell(lui32 x, lui32 y, lid entity);

	bool LOSCheck(lid ent_this, lid ent_targ);
	lid GetClosestPlayer(lid index);
	lid GetClosestEntity(lid index, lf32 distance);
	lid GetViewTargetEntity(lid index, lf32 distance, fac::facalleg allegiance);
	lid GetClosestEntityAlleg(lid index, lf32 distance, fac::facalleg allegiance);
	lid GetClosestEntityAllegLOS(lid index, lf32 distance, fac::facalleg allegiance);
	lid GetEntityViewTarget(lid index);
}

#endif
