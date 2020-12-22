#pragma once

#include "ec_common.h"

// Entity type representing placed items
struct ECSingleItem : public ECCommon
{
	LtrID item_instance;
	graphics::Matrix4x4 matrix;
};

char* RestingItemName(void* ent); // <- this seems really unneeded...
void RestingItemTick(LtrID id, void* ent, lf32 dt);
void RestingItemDraw(LtrID id, void* ent);

struct ECTarBubble : public ECCommon
{
	lf32 tb_state;
};

void TarBubbleTick(LtrID id, void* ent, lf32 dt);
void TarBubbleDraw(LtrID id, void* ent);

struct ECWallBug : public ECCommon
{
	lf32 test;
};

void WallBugTick(LtrID id, void* ent, lf32 dt);
void WallBugDraw(LtrID id, void* ent);
