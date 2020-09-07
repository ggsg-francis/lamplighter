#include "collision.h"

#include "graphics.hpp"

#include "core.h"
#include "index.h"
#include "entity.h"

bool RayEntity(btID ent, btf32 stand_height)
{
	ECCommon* entity = ENTITY(ent);

	btf32 th;
	env::GetNearestSurfaceHeight(th, entity->t.csi, entity->t.height);
	if (entity->t.height + entity->t.height_velocity <= th + stand_height)
		return true;
	return false;
}
