#pragma once

namespace graphics
{
	class TerrainMesh;
}

void CollisionInit();

void CollisionEnd();

void CollisionMakeEnvMesh(graphics::TerrainMesh* mesh);

void CollideEntity(btID ent);
bool RayEntity(btID ent, btf32 stand_height);
bool RayProjectile(btf32 px, btf32 py, btf32 pz, btf32 dx, btf32 dy, btf32 dz);
