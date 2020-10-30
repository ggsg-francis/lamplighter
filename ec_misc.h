#pragma once

#include "ec_common.h"

// Entity type representing placed items
struct ECSingleItem : public ECCommon
{
	lid item_instance;
	graphics::Matrix4x4 matrix;
};

char* RestingItemName(void* ent); // <- this seems really unneeded...
void RestingItemTick(lid id, void* ent, lf32 dt);
void RestingItemDraw(lid id, void* ent);

struct ECTarBubble : public ECCommon
{
	lf32 tb_state;
};

void TarBubbleTick(lid id, void* ent, lf32 dt);
void TarBubbleDraw(lid id, void* ent);

struct ECWallBug : public ECCommon
{
	lf32 test;
};

void WallBugTick(lid id, void* ent, lf32 dt);
void WallBugDraw(lid id, void* ent);
