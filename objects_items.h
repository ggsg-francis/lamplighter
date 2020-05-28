#ifndef OBJECTS_ITEMS_H
#define OBJECTS_ITEMS_H

// Inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "input.h"
#include "memory.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

class Actor;

namespace m
{
	class Vector3;
};

//-------------------------------- HELD ITEM

void HeldItemTick(btID id, btf32 dt, Actor* owner);
void HeldItemDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
void HeldItemOnEquip(btID id, Actor* owner);
m::Vector3 HeldItemGetLeftHandPos(btID id);
m::Vector3 HeldItemGetRightHandPos(btID id);
bool HeldItemBlockTurn(btID id);
bool HeldItemBlockMove(btID id);

//-------------------------------- HELD ITEM MELEE

void HeldMelTick(btID id, btf32 dt, Actor * owner);
void HeldMelDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle _pitch);
void HeldMelOnEquip(btID id, Actor* owner);
m::Vector3 HeldMelGetLeftHandPos(btID id);
m::Vector3 HeldMelGetRightHandPos(btID id);
bool HeldMelBlockTurn(btID id);
bool HeldMelBlockMove(btID id);

//-------------------------------- HELD ITEM GUN

void HeldGunTick(btID id, btf32 dt, Actor* owner);
void HeldGunDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2);
void HeldGunOnEquip(btID id, Actor* owner);
m::Vector3 HeldGunGetLeftHandPos(btID id);
m::Vector3 HeldGunGetRightHandPos(btID id);
bool HeldGunBlockTurn(btID id);
bool HeldGunBlockMove(btID id);

//-------------------------------- HELD ITEM MAGIC

void HeldMgcTick(btID id, btf32 dt, Actor* owner);
void HeldMgcDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
void HeldMgcOnEquip(btID id, Actor* owner);
m::Vector3 HeldMgcGetLeftHandPos(btID id);
m::Vector3 HeldMgcGetRightHandPos(btID id);
bool HeldMgcBlockTurn(btID id);
bool HeldMgcBlockMove(btID id);

//-------------------------------- HELD ITEM CONSUME

bool HeldConUse(btID id, Actor* owner);
void HeldConTick(btID id, btf32 dt, Actor* owner);
void HeldConDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
void HeldConOnEquip(btID id, Actor* owner);

//-------------------------------- HELD ITEM MATCHLOCK

void HeldGunMatchLockTick(btID id, btf32 dt, Actor* owner);
void HeldGunMatchLockDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2);
void HeldGunMatchLockOnEquip(btID id, Actor* owner);
m::Vector3 HeldGunMatchLockGetLeftHandPos(btID id);
m::Vector3 HeldGunMatchLockGetRightHandPos(btID id);
bool HeldGunMatchLockBlockTurn(btID id);
bool HeldGunMatchLockBlockMove(btID id);

// Base item instance
struct HeldItem
{
	btID id_item_template = ID_NULL;
	Transform3D t_item;

	// Tick this item
	void(*fpTick)(btID self, btf32 dt, Actor* owner);
	// Render graphics of this item
	void(*fpDraw)(btID self, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	//
	void(*fpOnEquip)(btID self, Actor* owner);
	//
	m::Vector3(*fpGetLeftHandPos)(btID self);
	//
	m::Vector3(*fpGetRightHandPos)(btID self);
	//
	bool(*fpBlockTurn)(btID self);
	//
	bool(*fpBlockMove)(btID self);
};

// Melee weapon instance
struct HeldMel : public HeldItem
{
	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : btui8
	{
		HOLD_POSE_SWING_OVERHEAD,
		HOLD_POSE_SWING_SIDE,
		HOLD_POSE_THRUST,
	};
	HoldPose ePose = HOLD_POSE_SWING_OVERHEAD;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	btf32 pitch = -90.f;
	btf32 yaw = 0.f;

	enum SwingState : btui8
	{
		SWINGSTATE_IDLE,
		SWINGSTATE_ATTACK,
		SWINGSTATE_RESET,
	};
	SwingState swinging = SWINGSTATE_IDLE;
	btf32 swingState = 0.f;
};

// Regular gun instance
struct HeldGun : public HeldItem
{
	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : btui8
	{
		HOLDSTATE_IDLE,
		HOLDSTATE_AIM,
		HOLDSTATE_INSPECT,
		HOLDSTATE_BARREL,
	};
	HoldPose ePose = HOLDSTATE_AIM;

	enum HGState : btui16
	{
		LATCH_PULLED = (0X1 << 0X0),
		FPAN_HATCH_OPEN = (0X1 << 0X1),
		FPAN_POWDER_IN = (0X1 << 0X2),
		BARREL_ARMED = (0X1 << 0X3),
		BARREL_ROD_IN = (0X1 << 0X4),
		MATCH_LIT = (0X1 << 0X7),
		MATCH_HELD = (0X1 << 0X8),
		LOST_ROD = (0X1 << 0X9),

		GET_CAN_FIRE = FPAN_HATCH_OPEN | FPAN_POWDER_IN | BARREL_ARMED,
		UNSET_FIRE = FPAN_POWDER_IN | BARREL_ARMED,
	};
	mem::bv<btui16, HGState> stateBV;

	btf32 lever = 0.f;
	btf32 rod = 0.f;
	btf32 rod_velocity = 0.f;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	m::Vector3 loc_velocity = m::Vector3(0.f, 0.f, 0.f);
	btf32 pitch = -90.f;
	btf32 pitch_velocity = 0.f;
	btf32 yaw = 0.f;
	btf32 yaw_velocity = 0.f;

	btf32 ang_aim_offset_temp = 0.f;
	btf32 ang_aim_pitch = 0.f;

	btui64 fire_time = 0.f;

	btID id_ammoInstance = ID_NULL;
};

// Magic caster instance
struct HeldMgc : public HeldItem
{
	btf32 charge = 1.f;
};

// Magic caster instance
struct HeldCons : public HeldItem
{
	btui32 uses = 32u;
};

// Matchlock gun instance
struct HeldGunMatchLock : public HeldItem
{
	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : btui8
	{
		HOLDSTATE_IDLE,
		HOLDSTATE_AIM,
		HOLDSTATE_INSPECT,
		HOLDSTATE_BARREL,
	};
	HoldPose ePose = HOLDSTATE_IDLE;

	enum HGState : btui16
	{
		LATCH_PULLED = (0X1 << 0X0),
		FPAN_HATCH_OPEN = (0X1 << 0X1),
		FPAN_POWDER_IN = (0X1 << 0X2),
		BARREL_ARMED = (0X1 << 0X3),
		BARREL_ROD_IN = (0X1 << 0X4),
		MATCH_LIT = (0X1 << 0X7),
		MATCH_HELD = (0X1 << 0X8),
		LOST_ROD = (0X1 << 0X9),

		GET_CAN_FIRE = FPAN_HATCH_OPEN | FPAN_POWDER_IN | BARREL_ARMED,
		UNSET_FIRE = FPAN_POWDER_IN | BARREL_ARMED,
	};
	mem::bv<btui16, HGState> stateBV;

	btf32 lever = 0.f;
	btf32 rod = 0.f;
	btf32 rod_velocity = 0.f;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	m::Vector3 loc_velocity = m::Vector3(0.f, 0.f, 0.f);
	btf32 pitch = -90.f;
	btf32 pitch_velocity = 0.f;
	btf32 yaw = 0.f;
	btf32 yaw_velocity = 0.f;

	btf32 ang_aim_offset_temp = 0.f;
	btf32 ang_aim_pitch = 0.f;
};

#endif
