#include "objects_items.h"
// not desirable but necessary for draw functions
#include "objects.h"

void HeldItem::Tick(bool actionA, bool actionB, bool actionC, bool actionD, bool use)
{
	
}

void HeldItem::Draw(m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{
	
}

void HeldGun::Tick(bool actionA, bool actionB, bool actionC, bool actionD, bool use)
{
	// State switch
	///*
	if (actionA)
	{
		btui8 i = eMusketHoldState;
		++i;
		if (i == eHOLD_STATE_COUNT)
			i = 0ui8;
		eMusketHoldState = (musket_hold_state)i;
	}
	//*/

	if (eMusketHoldState == HeldGun::aim) // IF AIMING
	{
		bvMusketState.setto(HeldGun::musket_state::latch_pulled, use);
	}
	else if (eMusketHoldState == HeldGun::inspect_pan) // IF PAN
	{
		if (actionB)
			bvMusketState.toggle(HeldGun::musket_state::fpan_hatch_open);
		if (bvMusketState.get(HeldGun::musket_state::fpan_hatch_open)) // if hatch open
			if (actionC) // input
				bvMusketState.set(HeldGun::musket_state::fpan_powder_in); // set powder
		bvMusketState.setto(HeldGun::musket_state::latch_pulled, use);
	}
	else if (eMusketHoldState == HeldGun::inspect_barrel) // IF BARREL
	{
		// if the hatch is open powder falls out lol
		if (bvMusketState.get(HeldGun::fpan_hatch_open))
			bvMusketState.unset(HeldGun::fpan_powder_in);

		if (actionB) // set latch float
			bvMusketState.toggle(HeldGun::musket_state::barrel_rod_in);

		rod = m::Lerp(rod, (btf32)bvMusketState.get(HeldGun::musket_state::barrel_rod_in), 0.1f);

		if (rod > 0.95f)
			bvMusketState.set(HeldGun::musket_state::barrel_armed); // unset powder
	}

	fpan = m::Lerp(fpan, (btf32)bvMusketState.get(HeldGun::musket_state::fpan_hatch_open), 0.3f);
	lever = m::Lerp(lever, (btf32)bvMusketState.get(HeldGun::musket_state::latch_pulled), 0.2f);

	if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(HeldGun::barrel_armed)
		&& bvMusketState.get(HeldGun::fpan_hatch_open)
		&& bvMusketState.get(HeldGun::fpan_powder_in))
	{
		bvMusketState.unset(HeldGun::unset_fire); // unset
												  //EntityCastProj(index);
	}
	else if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(HeldGun::fpan_hatch_open)
		&& bvMusketState.get(HeldGun::fpan_powder_in))
	{
		bvMusketState.unset(HeldGun::unset_fire); // unset
	}
}

void HeldGun::Draw(m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	switch ((eMusketHoldState))
	{
	case HeldGun::aim:
		loc = m::Lerp(loc, m::Vector3(0.13f, 1.3f, 0.3f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, pitch2.Deg(), 0.1f);
		break;
	case HeldGun::inspect_pan:
		/*loc = m::Lerp(loc, m::Vector3(-0.13f, 1.2f, 0.6f), 0.1f);
		yaw = m::Lerp(yaw, 65.f, 0.1f);
		pitch = m::Lerp(pitch, -15.f, 0.1f);*/
		loc = m::Lerp(loc, m::Vector3(0.f, 1.f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, -15.f, 0.1f);
		pitch = m::Lerp(pitch, -30.f, 0.1f);
		break;
	case HeldGun::inspect_barrel:
		loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, -80.f, 0.1f);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(ID_NULL, res::m_item_matchlock_01, res::t_item_matchlock_01, graphics::shader_solid, t_item);

	if (bvMusketState.get(HeldGun::musket_state::fpan_powder_in))
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan_full, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);
	else
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_lever, lever, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	if (eMusketHoldState == HeldGun::inspect_barrel || bvMusketState.get(HeldGun::musket_state::barrel_rod_in)) // IF BARREL
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_rod, rod, res::t_item_matchlock_01, graphics::shader_blend, t_item);
}