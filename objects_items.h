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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//------------- ITEM STUFF ---------------------------------------
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

struct HeldItem
{
	Transform3D t_item;

	virtual void Tick(bool actionA, bool actionB, bool actionC, bool use);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
};
struct HeldGun : public HeldItem
{
	// GUN STUFF (will eventually end up elsewhere)
	enum musket_hold_state : btui8
	{
		aim,
		inspect_pan,
		inspect_barrel,
		eHOLD_STATE_COUNT,
	};
	musket_hold_state eMusketHoldState = aim;

	enum musket_state : btui16
	{
		latch_pulled = (0x1 << 0x0),
		fpan_hatch_open = (0x1 << 0x1),
		fpan_powder_in = (0x1 << 0x2),
		barrel_armed = (0x1 << 0x3),
		barrel_rod_in = (0x1 << 0x4),
		match_lit = (0x1 << 0x7),
		match_held = (0x1 << 0x8),

		get_can_fire = fpan_hatch_open & fpan_powder_in & barrel_armed,
		unset_fire = fpan_powder_in | barrel_armed,
	};
	mem::bv<btui16, musket_state> bvMusketState;

	btf32 fpan = 0.f;
	btf32 lever = 0.f;
	btf32 rod = 0.f;

	m::Vector3 loc = m::Vector3(0.f, 0.9f, 0.4f);
	btf32 pitch = -90.f;
	btf32 yaw = 0.f;

	virtual void Tick(bool actionA, bool actionB, bool actionC, bool use);
	virtual void Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch);
};
