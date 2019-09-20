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

struct CellGroup;
struct CellSpace;

namespace index
{
	btf32 GetHP(btID id);

	extern btID players[2];
	extern btID activeplayer;

	void SetInput(btID INDEX, m::Vector2 INPUT, btf32 YAW, btf32 PITCH, bool WantAttack, bool RUN, bool AIM);

	void SetViewTargetID(btID ID);
	btID GetViewTargetID();

	void* GetEntity(btID ID);

	btID SpawnItem(btID ITEMID, m::Vector2 POSITION, btf32 DIRECTION);

	//btID SpawnEntity(prefab::prefabtype type, fw::Vector2 pos, float dir);
	void DestroyEntity(btID ID);

	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectile(int TYPE, m::Vector2 POSITION, btf32 HEIGHT, float YAW, float PITCH, float SPREAD);
	// Removes a given projectile from the index
	void DestroyProjectile(btID ID);

	// Adds this entity to the local vector of this cell
	void AddEntityCell(btui32 X, btui32 Y, btID ENTITY);
	// Removes this entity from the local vector of this cell
	void RemoveEntityCell(btui32 X, btui32 Y, btID ENTITY);

	void GetCellGroup(m::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& csi);

	bool ProjectileDoesIntersectEnv(btID id);
}