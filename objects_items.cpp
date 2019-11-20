#include "objects_items.h"
#include "audio.hpp"
// not desirable but necessary for draw functions
#include "objects.h"
#include "index.h"

//________________________________________________________________________________________________________________________________
//--------------------------- BASE ITEM ------------------------------------------------------------------------------------------

void HeldItem::Tick(Actor* owner)
{
	//
}

void HeldItem::Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.1f + archive::items[itemid]->f_radius)); // set pose

	DrawMeshAtTransform(ID_NULL, res::GetM(archive::items[itemid]->id_mesh), res::GetT(archive::items[itemid]->id_tex), graphics::shader_solid, t_item);
}

void HeldItem::OnEquip()
{
	//
}

m::Vector3 HeldItem::GetLeftHandPos()
{
	return t_item.GetPosition() + t_item.GetRight() * 0.5f;
}

m::Vector3 HeldItem::GetRightHandPos()
{
	return t_item.GetPosition() + t_item.GetRight() * -0.5f;
}

bool HeldItem::BlockTurn()
{
	return false;
}

bool HeldItem::BlockMove()
{
	return false;
}

//________________________________________________________________________________________________________________________________
//--------------------------- MELEE WEAPON ---------------------------------------------------------------------------------------

void HeldMel::Tick(Actor * owner)
{
	if (owner->inputBV.get(Actor::IN_ACTN_A))
		ePose = HOLDSTATE_SWING_OVERHEAD;
	else if (owner->inputBV.get(Actor::IN_ACTN_B))
		ePose = HOLDSTATE_SWING_SIDE;
	else if (owner->inputBV.get(Actor::IN_ACTN_C))
		ePose = HOLDSTATE_THRUST;

	switch ((ePose))
	{
	case HOLDSTATE_SWING_OVERHEAD:
		if (owner->inputBV.get(Actor::IN_USE))
		{
			loc = m::Lerp(loc, m::Vector3(0.1f, 1.f, 0.25f), 0.3f);
			yaw = m::Lerp(yaw, -35.f, 0.3f);
			pitch = m::Lerp(pitch, 20.f, 0.3f);
		}
		else
		{
			loc = m::Lerp(loc, m::Vector3(0.1f, 1.f, 0.25f), 0.2f);
			yaw = m::Lerp(yaw, 35.f, 0.2f);
			pitch = m::Lerp(pitch, -60.f, 0.2f);
		}
		break;
	case HOLDSTATE_SWING_SIDE:
		if (owner->inputBV.get(Actor::IN_USE))
		{
			loc = m::Lerp(loc, m::Vector3(-0.2f, 1.1f, 0.1f), 0.2f);
			yaw = m::Lerp(yaw, -85.f, 0.2f);
			pitch = m::Lerp(pitch, -20.f, 0.2f);
		}
		else
		{
			loc = m::Lerp(loc, m::Vector3(-0.2f, 1.1f, 0.3f), 0.1f);
			yaw = m::Lerp(yaw, 85.f, 0.1f);
			pitch = m::Lerp(pitch, -20.f, 0.1f);
		}
		break;
	case HOLDSTATE_THRUST:
		owner->inputBV.get(Actor::IN_USE) ?
			loc = m::Lerp(loc, m::Vector3(0.13f, 1.0f, 0.6f), 0.6f) :
			loc = m::Lerp(loc, m::Vector3(0.13f, 1.0f, 0.1f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.3f);
		pitch = m::Lerp(pitch, owner->viewPitch.Deg(), 0.1f);
		break;
	}
}

void HeldMel::Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle _pitch)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(ID_NULL, res::GetM(archive::items[itemid]->id_mesh), res::GetT(archive::items[itemid]->id_tex), graphics::shader_solid, t_item);
}

void HeldMel::OnEquip()
{
}

m::Vector3 HeldMel::GetLeftHandPos()
{
	switch ((ePose))
	{
	case HOLDSTATE_SWING_OVERHEAD:
		return t_item.GetPosition() + t_item.GetForward() * 0.2f;
		break;
	case HOLDSTATE_SWING_SIDE:
		return t_item.GetPosition() + t_item.GetForward() * 0.3f;
		break;
	case HOLDSTATE_THRUST:
		return t_item.GetPosition() + t_item.GetForward() * -0.15f;
		break;
	}
}

m::Vector3 HeldMel::GetRightHandPos()
{
	switch ((ePose))
	{
	case HOLDSTATE_SWING_OVERHEAD:
		return t_item.GetPosition();
		break;
	case HOLDSTATE_SWING_SIDE:
		return t_item.GetPosition();
		break;
	case HOLDSTATE_THRUST:
		return t_item.GetPosition() + t_item.GetForward() * 0.25f;
		break;
	}
}

bool HeldMel::BlockTurn()
{
	return false;
}

bool HeldMel::BlockMove()
{
	return false;
}

//________________________________________________________________________________________________________________________________
//--------------------------- MATCHLOCK GUN --------------------------------------------------------------------------------------

void HeldGun::Tick(Actor* owner)
{
	if (owner->inputBV.get(Actor::IN_ACTN_A))
		if (ePose == HOLDSTATE_IDLE)
			ePose = HOLDSTATE_AIM;
		else if (ePose == HOLDSTATE_AIM)
			ePose = HOLDSTATE_IDLE;
		else ePose = HOLDSTATE_AIM;
	else if (owner->inputBV.get(Actor::IN_ACTN_B))
		ePose = HOLDSTATE_INSPECT;
	else if (owner->inputBV.get(Actor::IN_ACTN_C))
		ePose = HOLDSTATE_BARREL;

	if (ePose == HOLDSTATE_IDLE) // IF AIMING
	{
		if (owner->inputBV.get(Actor::IN_COM_ALERT))
			ePose = HOLDSTATE_AIM;
	}
	if (ePose == HOLDSTATE_AIM) // IF AIMING
	{
		stateBV.setto(HGState::LATCH_PULLED, owner->inputBV.get(Actor::IN_USE));
	}
	else if (ePose == HOLDSTATE_INSPECT) // IF PAN
	{
		if (owner->inputBV.get(Actor::IN_USE_ALT))
			stateBV.toggle(HGState::FPAN_HATCH_OPEN);
		if (stateBV.get(HGState::FPAN_HATCH_OPEN)) // if hatch open
			if (owner->inputBV.get(Actor::IN_USE_HIT)) // input
				stateBV.set(HGState::FPAN_POWDER_IN); // set powder
	}
	else if (ePose == HOLDSTATE_BARREL) // IF BARREL
	{
		// if the hatch is open powder falls out lol
		if (stateBV.get(FPAN_HATCH_OPEN))
			stateBV.unset(FPAN_POWDER_IN);

		// If has ramrod & powder is in
		if (stateBV.get(FPAN_POWDER_IN) && !stateBV.get(HGState::LOST_ROD))
		{
			if (owner->inputBV.get(Actor::IN_USE)) // set rod float
				stateBV.set(HGState::BARREL_ROD_IN);
			else if (owner->inputBV.get(Actor::IN_USE_ALT)) // set rod float
				stateBV.unset(HGState::BARREL_ROD_IN);
			rod = m::Lerp(rod, (btf32)stateBV.get(HGState::BARREL_ROD_IN), 0.1f);
			//m::SpringDamper(rod, rod_velocity,(btf32)stateBV.get(HGState::BARREL_ROD_IN), 2.f);
			if (rod > 0.95f)
				stateBV.set(HGState::BARREL_ARMED); // unset powder
		}
	}

	lever = m::Lerp(lever, (btf32)stateBV.get(HGState::LATCH_PULLED), 0.2f);

	if (lever > 0.95f
		//&& stateBV.get(GET_CAN_FIRE))
		&& stateBV.get(BARREL_ARMED)
		&& stateBV.get(FPAN_HATCH_OPEN)
		&& stateBV.get(FPAN_POWDER_IN))
	{
		stateBV.unset(UNSET_FIRE); // unset
		if (stateBV.get(HGState::BARREL_ROD_IN))
		{
			aud::PlayGunshotTemp(false); // Play gunshot sound
			stateBV.set(LOST_ROD);
		}
		else
		{
			aud::PlayGunshotTemp(true); // Play gunshot sound
			loc_velocity.z -= 0.12f;
			pitch_velocity -= 19.f;
			index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
		}
	}
	else if (lever > 0.95f
		&& stateBV.get(FPAN_HATCH_OPEN)
		&& stateBV.get(FPAN_POWDER_IN))
	{
		stateBV.unset(UNSET_FIRE); // unset
		aud::PlayGunshotTemp(false); // Play gunshot sound
	}
}

void HeldGun::Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	const btf32 mass = 15.f;
	const btf32 spring_rot = 2.6f;
	const btf32 spring_mov = 1.8f;
	const btf32 damping = 8.f;

	switch ((ePose))
	{
	case HOLDSTATE_IDLE:
		//loc = m::Lerp(loc, m::Vector3(0.13f, 1.0f, 0.1f), 0.1f);
		/*m::SpringDamper(loc.x, loc_velocity.x, 0.13f, k_mov);
		m::SpringDamper(loc.y, loc_velocity.y, 1.0f, k_mov);
		m::SpringDamper(loc.z,  loc_velocity.z, 0.1f, k_mov);*/
		m::SpringDamper(loc.x, loc_velocity.x, 0.23f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 1.0f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.1f, mass, spring_mov, damping);

		/*yaw = m::Lerp(yaw, -45.f, 0.1f);
		pitch = m::Lerp(pitch, 20.f, 0.1f);*/
		/*m::SpringDamper(yaw, yaw_velocity, -45.f, k_rot);
		m::SpringDamper(pitch, pitch_velocity, 20.f, k_rot);*/
		m::SpringDamper(yaw, yaw_velocity, -60.f, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, -33.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_AIM:
		//loc = m::Lerp(loc, m::Vector3(0.08f, 1.3f, 0.4f), 0.1f);
		m::SpringDamper(loc.x, loc_velocity.x, 0.08f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 1.3f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.4f, mass, spring_mov, damping);
		//yaw = m::Lerp(yaw, 0.f, 0.1f);
		m::SpringDamper(yaw, yaw_velocity, 0.f, mass, spring_rot, damping);
		std::cout << "yaw " << yaw << std::endl;
		m::SpringDamper(pitch, pitch_velocity, pitch2.Deg(), mass, spring_rot, damping);
		break;
	case HOLDSTATE_INSPECT:
		//loc = m::Lerp(loc, m::Vector3(0.f, 1.f, 0.4f), 0.1f);
		m::SpringDamper(loc.x, loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 1.f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.4f, mass, spring_mov, damping);
		/*yaw = m::Lerp(yaw, -15.f, 0.1f);
		pitch = m::Lerp(pitch, -30.f, 0.1f);*/
		m::SpringDamper(yaw, yaw_velocity, -15.f, mass, spring_rot, damping);
		std::cout << "yaw " << yaw << std::endl;
		m::SpringDamper(pitch, pitch_velocity, -30.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_BARREL:
		//loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.3f), 0.1f);
		m::SpringDamper(loc.x, loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 0.2f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.3f, mass, spring_mov, damping);
		/*yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, -70.f, 0.1f);*/
		m::SpringDamper(yaw, yaw_velocity, 0.f, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, -70.f, mass, spring_rot, damping);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(ID_NULL, res::GetM(archive::items[itemid]->id_mesh), res::GetT(archive::items[itemid]->id_tex), graphics::shader_solid, t_item);

	if (stateBV.get(FPAN_HATCH_OPEN))
		if (stateBV.get(FPAN_POWDER_IN))
			DrawMeshAtTransform(ID_NULL, res::GetM(res::m_item_matchlock_01_pan_open_full), res::GetT(res::t_item_matchlock_01), graphics::shader_solid, t_item);
		else
			DrawMeshAtTransform(ID_NULL, res::GetM(res::m_item_matchlock_01_pan_open), res::GetT(res::t_item_matchlock_01), graphics::shader_solid, t_item);
	else DrawMeshAtTransform(ID_NULL, res::GetM(res::m_item_matchlock_01_pan), res::GetT(res::t_item_matchlock_01), graphics::shader_solid, t_item);

	DrawBlendMeshAtTransform(ID_NULL, res::GetMB(res::mb_item_matchlock_01_lever), lever, res::GetT(res::t_item_matchlock_01), graphics::shader_solidBlend, t_item);

	if (!stateBV.get(LOST_ROD)) // IF HAS RAMROD
	{
		if ((ePose == HOLDSTATE_BARREL && stateBV.get(FPAN_POWDER_IN)) || stateBV.get(BARREL_ROD_IN)) // IF BARREL
			DrawBlendMeshAtTransform(ID_NULL, res::GetMB(res::mb_item_matchlock_01_rod_anim), rod, res::GetT(res::t_item_matchlock_01), graphics::shader_solidBlend, t_item);
		else
			DrawMeshAtTransform(ID_NULL, res::GetM(res::m_item_matchlock_01_rod), res::GetT(res::t_item_matchlock_01), graphics::shader_solid, t_item);
	}
}

void HeldGun::OnEquip()
{
	loc = m::Vector3(0.3f, 1.1f, 0.f);
	pitch = 90.f;
	yaw = 0.f;
	ePose = HeldGun::HOLDSTATE_IDLE;
}

m::Vector3 HeldGun::GetLeftHandPos()
{
	if (ePose != HeldGun::HOLDSTATE_BARREL) // If holding normally
		return t_item.GetPosition() + t_item.GetUp() * -0.08f;
	else
		return t_item.GetPosition() + t_item.GetForward() * 0.75f;
}

m::Vector3 HeldGun::GetRightHandPos()
{
	if (ePose != HeldGun::HOLDSTATE_BARREL) // If holding normally
		return t_item.GetPosition() + t_item.GetForward() * 0.5f;
	else
		return t_item.GetPosition() + t_item.GetForward() * 1.f;
}

bool HeldGun::BlockTurn()
{
	if (ePose == HeldGun::HOLDSTATE_BARREL) // If holding barrel
		return true;
	else return false;
}

bool HeldGun::BlockMove()
{
	if (ePose == HeldGun::HOLDSTATE_BARREL) // If holding barrel
		return true;
	else return false;
}

//________________________________________________________________________________________________________________________________
//--------------------------- MAGIC CASTER ---------------------------------------------------------------------------------------

void HeldMgc::Tick(Actor * owner)
{
	//
}

void HeldMgc::Draw(btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.3f)); // set pose

	DrawMeshAtTransform(ID_NULL, res::GetM(archive::items[itemid]->id_mesh), res::GetT(archive::items[itemid]->id_tex), graphics::shader_solid, t_item);
}

void HeldMgc::OnEquip()
{
	//
}

m::Vector3 HeldMgc::GetLeftHandPos()
{
	return t_item.GetPosition() + t_item.GetRight() * 0.1f;
}

m::Vector3 HeldMgc::GetRightHandPos()
{
	return t_item.GetPosition() + t_item.GetRight() * -0.1f;
}

bool HeldMgc::BlockTurn()
{
	return false;
}

bool HeldMgc::BlockMove()
{
	return false;
}
