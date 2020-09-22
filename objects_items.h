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
void ItemInit(btID item);
// Tick this item instance
void ItemTick(btID item, btf32 DT, btID OWNER_ID, ECActor* OWNER);
// Render graphics of this item instance
void ItemDraw(btID item, btID ITEMID, m::Vector2 OWNER_POSITION, btf32 OWNER_HEIGHT, m::Angle OWNER_YAW, m::Angle OWNER_PITCH);
// Tell this item instance that it's just been equipped
void ItemOnEquip(btID item, ECActor* b);
// Get left hand position
m::Vector3 ItemLHPos(btID item);
// Get right hand position
m::Vector3 ItemRHPos(btID item);
bool ItemBlockTurn(btID item);
bool ItemBlockMove(btID item);

// Base item instance
struct HeldItem
{
	//-------------------------------- HELD ITEM

	btID id_item_template = ID_NULL;
	Transform3D t_item;

	//-------------------------------- SHARED THINGS

	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : btui8
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

	btf32 pitch = -90.f;
	btf32 pitch_velocity = 0.f;
	btf32 yaw = 0.f;
	btf32 yaw_velocity = 0.f;

	//-------------------------------- HELD ITEM MELEE

	enum SwingState : btui8
	{
		SWINGSTATE_IDLE,
		SWINGSTATE_ATTACK,
		SWINGSTATE_RESET,
	};
	SwingState swinging = SWINGSTATE_IDLE;
	btf32 swingState = 0.f;

	//-------------------------------- HELD ITEM GUN

	btf32 ang_aim_offset_temp = 0.f;
	btf32 ang_aim_pitch = 0.f;
	btui64 fire_time = 0u;
	btID id_ammoInstance = ID_NULL;

	//-------------------------------- HELD ITEM MAGIC

	btf32 charge = 1.f;

	//-------------------------------- HELD ITEM CONSUME

	btui32 uses = 32u;
};

union ItemInstance
{
	HeldItem item;
};

#endif
