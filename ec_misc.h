#pragma once

#include "ec_common.h"

// Entity type representing placed items
struct ECSingleItem : public ECCommon
{
	btID item_instance;
	graphics::Matrix4x4 matrix;
};

char* RestingItemName(void* ent);
void RestingItemTick(btID id, void* ent, btf32 dt);
void RestingItemDraw(btID id, void* ent);

// todo remove this
struct EditorPawn : public ECCommon
{
	Transform3D t_body, t_head;
};

void TickEditorPawn(btID id, void* ent, btf32 dt);
void DrawEditorPawn(btID id, void* ent);
