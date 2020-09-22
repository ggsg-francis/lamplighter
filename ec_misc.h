#pragma once

#include "ec_common.h"

// Entity type representing placed items
struct ECSingleItem : public ECCommon
{
	btID item_instance;
	graphics::Matrix4x4 matrix;

	int TestFn() {
		return 10;
	}
};

char* RestingItemName(void* ent); // <- this seems really unneeded...
void RestingItemTick(btID id, void* ent, btf32 dt);
void RestingItemDraw(btID id, void* ent);

struct ECTarBubble : public ECCommon
{
	btf32 tb_state;
};

void TarBubbleTick(btID id, void* ent, btf32 dt);
void TarBubbleDraw(btID id, void* ent);

struct ECWallBug : public ECCommon
{
	btf32 test;
};

void WallBugTick(btID id, void* ent, btf32 dt);
void WallBugDraw(btID id, void* ent);
