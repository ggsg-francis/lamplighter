#pragma once

//Forward declarations
namespace fw
{
	class Vector3;
	class Vector2;
}
class Transform2D;
class Texture;
class Transform3D;

struct CellGroup;
struct CellSpaceInfo;

namespace index
{
	#ifdef DEF_EDITOR
	btui32 GetCellX();
	btui32 GetCellY();
	#endif // DEF_EDITOR

	extern btID players[2];
	extern btID activeplayer;

	void SetInput(btID INDEX, fw::Vector2 INPUT, btf32 YAW, btf32 PITCH, bool WantAttack, bool RUN, bool AIM);

	void SetPose(void* MSG);

	//	Creates a projectile instance, allocates an ID and sends a network message
	void SpawnProjectile(int TYPE, fw::Vector2 POSITION, float DIRECTION, float SPREAD);
	// Removes a given projectile from the index
	void DestroyProjectile(btID ID);

	// Adds this entity to the local vector of this cell
	void AddEntityCell(int X, int Y, btID ENTITY);
	// Removes this entity from the local vector of this cell
	void RemoveEntityCell(int X, int Y, btID ENTITY);

	void GetCollisionCells(fw::Vector2 vec, CellGroup& cg);
	void GetCellSpaceInfo(fw::Vector2 vec, CellSpaceInfo& csi);

	bool ProjectileDoesIntersectEnv(btID id);
}