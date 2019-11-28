#pragma once

// Inherited from index.cpp
#include "archive.hpp"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "time.hpp"
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

// Base item instance
struct HeldItem
{
	Transform3D t_item;
	virtual void Tick(Actor* owner);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	virtual void OnEquip();
	virtual m::Vector3 GetLeftHandPos();
	virtual m::Vector3 GetRightHandPos();
	virtual bool BlockTurn();
	virtual bool BlockMove();
};

// Magic caster instance
struct HeldMel : public HeldItem
{
	// GUN STUFF (will eventually end up elsewhere)
	enum HoldPose : btui8
	{
		HOLDSTATE_SWING_OVERHEAD,
		HOLDSTATE_SWING_SIDE,
		HOLDSTATE_THRUST,
	};
	HoldPose ePose = HOLDSTATE_SWING_OVERHEAD;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	btf32 pitch = -90.f;
	btf32 yaw = 0.f;

	virtual void Tick(Actor* owner);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	virtual void OnEquip();
	virtual m::Vector3 GetLeftHandPos();
	virtual m::Vector3 GetRightHandPos();
	virtual bool BlockTurn();
	virtual bool BlockMove();
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

	btf32 fire_time = 0.f;

	virtual void Tick(Actor* owner);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	virtual void OnEquip();
	virtual m::Vector3 GetLeftHandPos();
	virtual m::Vector3 GetRightHandPos();
	virtual bool BlockTurn();
	virtual bool BlockMove();
};

// Magic caster instance
struct HeldMgc : public HeldItem
{
	btf32 charge = 1.f;

	virtual void Tick(Actor* owner);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	virtual void OnEquip();
	virtual m::Vector3 GetLeftHandPos();
	virtual m::Vector3 GetRightHandPos();
	virtual bool BlockTurn();
	virtual bool BlockMove();
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

	virtual void Tick(Actor* owner);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
	virtual void OnEquip();
	virtual m::Vector3 GetLeftHandPos();
	virtual m::Vector3 GetRightHandPos();
	virtual bool BlockTurn();
	virtual bool BlockMove();
};