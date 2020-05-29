#include "collision.h"

#include "graphics.hpp"

#include "core.h"
#include "index.h"
#include "objects_entities.h"

bool RayEntity(btID ent, btf32 stand_height)
{
	Entity* entity = ENTITY(ent);

	btf32 th;
	env::GetHeight(th, entity->t.csi);
	if (entity->t.height <= th + stand_height)
		return true;
	return false;
}
