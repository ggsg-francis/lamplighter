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

namespace index
{
	void Init();
	void End();

	void Tick(btf32 DELTA);
	void TickGUI();

	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw();

	void SetViewFocus(btID i);
	m::Vector2 GetViewOffset();

	// Get the pointer address of the entity at X ID
	void* GetEntity(btID ID);
	// Destroy the entity at X ID
	void DestroyEntity(btID ID);

	btID SpawnItem(btID ITEMID, m::Vector2 POSITION, btf32 DIRECTION);

	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectile(fac::faction FACTION, m::Vector2 POSITION, btf32 HEIGHT, float YAW, float PITCH, float SPREAD);
	// Removes a given projectile from the index
	void DestroyProjectile(btID ID);

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	// Get HP (Health Points) of entity at X ID
	btf32 GetHP(btID id);

	// Set shadow texture ID (to do: hacky, get rid of this)
	void SetShadowTexture(btui32 ID);

	extern btID players[2];
	extern btID activeplayer;

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

	btID GetClosestPlayer(btID index);
	btID GetClosestEntity(btID index, btf32 dist);
	btID GetViewTargetEntity(btID index, btf32 dist, fac::facalleg allegiance);
	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance);
	btID GetClosestEntityWeighted(btID index, btf32 dist, fac::facalleg weight);
	btID GetClosestActivator(btID index);
}