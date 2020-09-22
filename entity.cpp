#include "entity.h"

#include "index.h"

void InitEntityMeta()
{
	IndexRegisterEntityMeta(ENTITY_TYPE_RESTING_ITEM, sizeof(ECSingleItem),
		RestingItemName, RestingItemTick, RestingItemDraw);
	IndexRegisterEntityMeta(ENTITY_TYPE_ACTOR, sizeof(ECActor),
		EntityName, ActorTick, ActorDraw);
	IndexRegisterEntityMeta(ENTITY_TYPE_TAR_BUBBLE, sizeof(ECTarBubble),
		EntityName, TarBubbleTick, TarBubbleDraw);
	IndexRegisterEntityMeta(ENTITY_TYPE_WALL_BUG, sizeof(ECWallBug),
		EntityName, WallBugTick, WallBugDraw);
}
