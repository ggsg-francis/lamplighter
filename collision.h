#ifndef COLLISION_H
#define COLLISION_H

#include "global.h"

namespace graphics
{
	class TerrainMesh;
}

bool RayEntity(btID ent, btf32 stand_height);

#endif // COLLISION_H
