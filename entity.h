#ifndef ENTITY_H
#define ENTITY_H

#include "global.h"

#include "ec_common.h"
#include "ec_actor.h"
#include "ec_misc.h"

enum EntityType : lui8 {
	// Types
	ENTITY_TYPE_TAR_BUBBLE,
	ENTITY_TYPE_RESTING_ITEM,
	ENTITY_TYPE_ACTOR,
	ENTITY_TYPE_WALL_BUG,
	// Utilities
	ENTITY_TYPE_COUNT,
	ENTITY_TYPE_NULL = 255u,
};

void InitEntityMeta();

#endif
