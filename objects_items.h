#ifndef OBJECTS_ITEMS_H
#define OBJECTS_ITEMS_H

// Originally inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "input.h"
#include "memory.h"
#include "maths.hpp"
#include "graphics.hpp"

#include "entity.h"

struct ECActor;

// Initialize this item instance
void ItemInit(lid item);
// Tick this item instance
void ItemTick(lid item, lf32 DT, lid OWNER_ID, ECActor* OWNER);
// Render graphics of this item instance
void ItemDraw(lid item, lid ITEMID);
// Tell this item instance that it's just been equipped
void ItemOnEquip(lid item, ECActor* b);
// Get left hand position
m::Vector3 ItemLHPos(lid item);
// Get right hand position
m::Vector3 ItemRHPos(lid item);
bool ItemBlockTurn(lid item);
bool ItemBlockMove(lid item);

// Base item instance
struct HeldItem
{
	//-------------------------------- HELD ITEM

	lid id_item_template = ID_NULL;
	Transform3D t_item;

	//-------------------------------- SHARED THINGS

	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : lui8
	{
		HOLD_POSE_SWING_OVERHEAD,
		HOLD_POSE_SWING_SIDE,
		HOLD_POSE_THRUST,
		HOLDSTATE_IDLE,
		HOLDSTATE_AIM,
		HOLDSTATE_INSPECT,
		HOLDSTATE_BARREL,
	};
	HoldPose ePose = HOLD_POSE_SWING_OVERHEAD;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	m::Vector3 loc_velocity = m::Vector3(0.f, 0.f, 0.f);

	lf32 pitch = -90.f;
	lf32 pitch_velocity = 0.f;
	lf32 yaw = 0.f;
	lf32 yaw_velocity = 0.f;

	//-------------------------------- HELD ITEM MELEE

	enum SwingState : lui8
	{
		SWINGSTATE_IDLE,
		SWINGSTATE_ATTACK,
		SWINGSTATE_RESET,
	};
	SwingState swinging = SWINGSTATE_IDLE;
	lf32 swingState = 0.f;

	//-------------------------------- HELD ITEM GUN

	lf32 ang_aim_offset_temp = 0.f;
	lf32 ang_aim_pitch = 0.f;
	lui64 fire_time = 0u;
	lid id_ammoInstance = ID_NULL;

	//-------------------------------- HELD ITEM MAGIC

	lf32 charge = 1.f;

	//-------------------------------- HELD ITEM CONSUME

	lui32 uses = 32u;
};

union ItemInstance
{
	HeldItem item;
};

#endif
