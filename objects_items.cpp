#include "objects_items.h"
#include "audio.hpp"
// not desirable but necessary for draw functions
#include "objects.h"
#include "core.h"

//________________________________________________________________________________________________________________________________
//--------------------------- ON EQUIP -------------------------------------------------------------------------------------------

//-------------------------------- HELD ITEM MISC

void HeldItemTick(btID id, btf32 dt, Actor* owner)
{
	//
}
void HeldItemDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	HeldItem* self = GETITEM_MISC(id);
	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);
	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	self->t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.1f + acv::items[itemid]->f_radius)); // set pose
	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}
void HeldItemOnEquip(btID id, Actor* owner)
{
	//
}
m::Vector3 HeldItemGetLeftHandPos(btID id)
{
	HeldItem* self = GETITEM_MISC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * -0.5f;
	return self->t_item.GetPosition() + self->t_item.GetRight() * -((acv::BaseItem*)acv::items[self->item_template])->f_radius;
}
m::Vector3 HeldItemGetRightHandPos(btID id)
{
	HeldItem* self = GETITEM_MISC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * 0.5f;
	return self->t_item.GetPosition() + self->t_item.GetRight() * ((acv::BaseItem*)acv::items[self->item_template])->f_radius;
}
bool HeldItemBlockTurn(btID id)
{
	return false;
}
bool HeldItemBlockMove(btID id)
{
	return false;
}

//-------------------------------- HELD ITEM MELEE

void HeldMelTick(btID id, btf32 dt, Actor* owner)
{
	HeldMel* self = GETITEM_MELEE(id);
	
	if (self->swinging == HeldMel::SWINGSTATE_IDLE)
	{
		if (owner->inputBV.get(Actor::IN_ACTN_A))
			self->ePose = HeldMel::HOLD_POSE_SWING_OVERHEAD;
		else if (owner->inputBV.get(Actor::IN_ACTN_B))
			self->ePose = HeldMel::HOLD_POSE_SWING_SIDE;
		else if (owner->inputBV.get(Actor::IN_ACTN_C))
			self->ePose = HeldMel::HOLD_POSE_THRUST;

		//if (owner->inputBV.get(Actor::IN_USE_HIT))
		if (owner->inputBV.get(Actor::IN_USE))
		{
			self->swinging = HeldMel::SWINGSTATE_ATTACK;
			aud::PlaySnd(aud::FILE_SWING, self->t_item.pos_glm);
		}
	}
	else if (self->swinging == HeldMel::SWINGSTATE_ATTACK)
	{
		self->swingState += 0.075f;
		// if in the middle of the swing, try damage
		if (self->swingState > 0.4f && self->swingState < 0.75f)
		{
			// if has enemy target, damage it
			if (owner->atk_target != BUF_NULL)
			{
				Entity* ent = (Entity*)index::GetEntityPtr(owner->atk_target);
				ent->state.Damage(0.3f, self->yaw);
				aud::PlaySnd(aud::FILE_SWING_CONNECT, m::Vector3(ent->t.position.x, ent->t.height, ent->t.position.y));
				ent->slideVelocity += m::AngToVec2(owner->t.yaw.Rad()) * 0.2f;
				// exit swing early
				self->swinging = HeldMel::SWINGSTATE_RESET;
			}
		}
		// if completed swing
		if (self->swingState > 1.f)
		{
			self->swinging = HeldMel::SWINGSTATE_RESET;
		}
	}
	else // reset
	{
		self->swingState -= 0.2f;
		if (self->swingState < 0.f)
		{
			self->swingState = 0.f;
			self->swinging = HeldMel::SWINGSTATE_IDLE;
		}
	}

	switch ((self->ePose))
	{
	case HeldMel::HOLD_POSE_SWING_OVERHEAD:
		self->loc = m::Lerp(m::Vector3(0.1f, 1.f, 0.25f), m::Vector3(0.1f, 1.f, 0.25f), self->swingState);
		self->yaw = m::Lerp(35.f, -45.f, self->swingState);
		self->pitch = m::Lerp(-60.f, 60.f, self->swingState);
		break;
	case HeldMel::HOLD_POSE_SWING_SIDE:
		self->loc = m::Lerp(m::Vector3(-0.2f, 1.1f, 0.3f), m::Vector3(-0.2f, 1.1f, 0.1f), self->swingState);
		self->yaw = m::Lerp(85.f, -85.f, self->swingState);
		self->pitch = m::Lerp(-20.f, -20.f, self->swingState);
		break;
	case HeldMel::HOLD_POSE_THRUST:
		self->loc = m::Lerp(m::Vector3(0.13f, 1.0f, 0.1f), m::Vector3(0.13f, 1.0f, 0.6f), self->swingState);
		self->yaw = 0.f;
		self->pitch = owner->viewPitch.Deg();
		break;
	}
}
void HeldMelDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle _pitch)
{
	HeldMel* self = GETITEM_MELEE(id);
	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);
	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	self->t_item.TranslateLocal(self->loc); // set pose
	self->t_item.Rotate(glm::radians(self->yaw), m::Vector3(0.f, 1.f, 0.f));
	self->t_item.Rotate(glm::radians(self->pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}
void HeldMelOnEquip(btID id, Actor* owner)
{
}
m::Vector3 HeldMelGetLeftHandPos(btID id)
{
	HeldMel* self = GETITEM_MELEE(id);
	switch ((self->ePose))
	{
	case HeldMel::HOLD_POSE_SWING_OVERHEAD:
		return self->t_item.GetPosition();
		break;
	case HeldMel::HOLD_POSE_SWING_SIDE:
		return self->t_item.GetPosition();
		break;
	case HeldMel::HOLD_POSE_THRUST:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.25f;
		break;
	}
}
m::Vector3 HeldMelGetRightHandPos(btID id)
{
	HeldMel* self = GETITEM_MELEE(id);
	switch ((self->ePose))
	{
	case HeldMel::HOLD_POSE_SWING_OVERHEAD:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.2f;
		break;
	case HeldMel::HOLD_POSE_SWING_SIDE:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.3f;
		break;
	case HeldMel::HOLD_POSE_THRUST:
		return self->t_item.GetPosition() + self->t_item.GetForward() * -0.15f;
		break;
	}
}
bool HeldMelBlockTurn(btID id)
{
	return false;
}
bool HeldMelBlockMove(btID id)
{
	return false;
}

//-------------------------------- HELD ITEM GUN

#define HOLDSTATE_AIM HeldGun::HOLDSTATE_AIM
#define HOLDSTATE_IDLE HeldGun::HOLDSTATE_IDLE
#define HOLDSTATE_INSPECT HeldGun::HOLDSTATE_INSPECT
#define HOLDSTATE_BARREL HeldGun::HOLDSTATE_BARREL

#define LATCH_PULLED HeldGun::LATCH_PULLED
#define FPAN_HATCH_OPEN HeldGun::FPAN_HATCH_OPEN
#define FPAN_POWDER_IN HeldGun::FPAN_POWDER_IN
#define BARREL_ARMED HeldGun::BARREL_ARMED
#define BARREL_ROD_IN HeldGun::BARREL_ROD_IN
#define MATCH_LIT HeldGun::MATCH_LIT
#define MATCH_HELD HeldGun::MATCH_HELD
#define LOST_ROD HeldGun::LOST_ROD
#define GET_CAN_FIRE HeldGun::GET_CAN_FIRE
#define UNSET_FIRE HeldGun::UNSET_FIRE

void HeldGunTick(btID id, btf32 dt, Actor* owner)
{
	HeldGun* self = GETITEM_GUN(id);

	if (owner->inputBV.get(Actor::IN_USE) && self->ePose == HOLDSTATE_AIM && self->fire_time < Time::time)
	{
		// if we try to fire, see if we can load the weapon
		if (self->ammoInstance = ID_NULL)
			self->ammoInstance = owner->inventory.GetItemOfTemplate(8);
		if (self->ammoInstance != ID_NULL)
		{
			if (!HeldConUse(self->ammoInstance, owner))
				self->ammoInstance = owner->inventory.GetItemOfTemplate(8);

			self->fire_time = Time::time + 0.1f;

			aud::PlaySnd(aud::FILE_SHOT_SMG, self->t_item.pos_glm); // Play gunshot sound
			//loc_velocity.z -= 0.12f;
			self->loc_velocity.z -= m::Random(0.03f, 0.06f);
			//pitch_velocity -= 19.f;
			self->pitch = owner->viewPitch.Deg();
			self->pitch_velocity -= m::Random(5.f, 7.5f);

			if (owner->atk_target != BUF_NULL)
			{
				//aud::PlayGunshotTemp(true); // Play gunshot sound

				Entity* target = (Entity*)index::GetEntityPtr(owner->atk_target);
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector2 targetoffset = m::Normalize(target->t.position - (owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f)));
				m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(self->t_item.GetPosition().x, self->t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - self->t_item.GetPosition().y);
				btf32 angle_yaw = m::Vec2ToAng(targetoffset);
				btf32 angle_pit = glm::radians(-90.f) + m::Vec2ToAng(m::Normalize(targetoffsetVertical));
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->viewYaw.Rad()) * 0.55f), owner->t.height, angle_yaw, angle_pit, 1.f);
				m::Vector3 spawnpos = self->t_item.GetPosition() + self->t_item.GetForward();
				index::SpawnProjectile(owner->faction, m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, angle_yaw, angle_pit, 1.f);
			}
			else
			{
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->viewYaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector3 spawnpos = self->t_item.GetPosition() + self->t_item.GetForward();
				index::SpawnProjectile(owner->faction, m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
			}

			//owner->t.velocity = m::AngToVec2(owner->viewYaw.Rad()) * -0.07f; // set my velocity
			owner->slideVelocity += m::AngToVec2(owner->viewYaw.Rad()) * -0.07f; // set my velocity
		}
	}

	/*if (owner->inputBV.get(Actor::IN_ACTN_A))
		if (self->ePose == HOLDSTATE_IDLE)
			self->ePose = HOLDSTATE_AIM;
		else if (self->ePose == HOLDSTATE_AIM)
			self->ePose = HOLDSTATE_IDLE;
		else self->ePose = HOLDSTATE_AIM;
	else if (owner->inputBV.get(Actor::IN_ACTN_B))
		self->ePose = HOLDSTATE_INSPECT;
	else if (owner->inputBV.get(Actor::IN_ACTN_C))
		self->ePose = HOLDSTATE_BARREL;*/

	if (owner->inputBV.get(Actor::IN_ACTN_A))
			self->ePose = HOLDSTATE_AIM;
	else if (owner->inputBV.get(Actor::IN_ACTN_B))
		self->ePose = HOLDSTATE_IDLE;
	else if (owner->inputBV.get(Actor::IN_ACTN_C))
		self->ePose = HOLDSTATE_INSPECT;

	if (self->ePose == HOLDSTATE_IDLE) // IF AIMING
	{
		if (owner->inputBV.get(Actor::IN_COM_ALERT))
			self->ePose = HOLDSTATE_AIM;
	}
	if (self->ePose == HOLDSTATE_AIM) // IF AIMING
	{
		self->stateBV.setto(LATCH_PULLED, owner->inputBV.get(Actor::IN_USE));
	}
	else if (self->ePose == HOLDSTATE_INSPECT) // IF PAN
	{
		if (owner->inputBV.get(Actor::IN_USE_ALT))
			self->stateBV.toggle(FPAN_HATCH_OPEN);
		if (self->stateBV.get(FPAN_HATCH_OPEN)) // if hatch open
			if (owner->inputBV.get(Actor::IN_USE_HIT)) // input
				self->stateBV.set(FPAN_POWDER_IN); // set powder
	}
	else if (self->ePose == HOLDSTATE_BARREL) // IF BARREL
	{
		// if the hatch is open powder falls out lol
		if (self->stateBV.get(FPAN_HATCH_OPEN))
			self->stateBV.unset(FPAN_POWDER_IN);

		// If has ramrod & powder is in
		if (self->stateBV.get(FPAN_POWDER_IN) && !self->stateBV.get(LOST_ROD))
		{
			if (owner->inputBV.get(Actor::IN_USE)) // set rod float
				self->stateBV.set(BARREL_ROD_IN);
			else if (owner->inputBV.get(Actor::IN_USE_ALT)) // set rod float
				self->stateBV.unset(BARREL_ROD_IN);
			self->rod = m::Lerp(self->rod, (btf32)self->stateBV.get(BARREL_ROD_IN), 0.1f);
			//m::SpringDamper(rod, rod_velocity,(btf32)stateBV.get(HGState::BARREL_ROD_IN), 2.f);
			if (self->rod > 0.95f)
				self->stateBV.set(BARREL_ARMED); // unset powder
		}
	}

	//lever = m::Lerp(lever, (btf32)stateBV.get(HGState::LATCH_PULLED), 0.2f);

	if (self->lever > 0.95f
		//&& stateBV.get(GET_CAN_FIRE))
		&& self->stateBV.get(BARREL_ARMED)
		&& self->stateBV.get(FPAN_HATCH_OPEN)
		&& self->stateBV.get(FPAN_POWDER_IN))
	{
		self->stateBV.unset(UNSET_FIRE); // unset
		if (self->stateBV.get(BARREL_ROD_IN))
		{
			//aud::PlayGunshotTemp(false, self->t_item.pos_glm); // Play gunshot sound
			self->stateBV.set(LOST_ROD);
		}
		else
		{
			aud::PlaySnd(aud::FILE_SHOT_SMG, self->t_item.pos_glm); // Play gunshot sound
			self->loc_velocity.z -= 0.12f;
			self->pitch_velocity -= 19.f;

			if (owner->atk_target != BUF_NULL)
			{
				//aud::PlayGunshotTemp(true); // Play gunshot sound

				Entity* target = (Entity*)index::GetEntityPtr(owner->atk_target);
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector2 targetoffset = m::Normalize(target->t.position - (owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f)));
				m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(self->t_item.GetPosition().x, self->t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - self->t_item.GetPosition().y);
				btf32 angle_yaw = m::Vec2ToAng(targetoffset);
				btf32 angle_pit = glm::radians(-90.f) + m::Vec2ToAng(m::Normalize(targetoffsetVertical));
				index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f), owner->t.height, angle_yaw, angle_pit, 1.f);
			}
			else
			{
				index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
			}
		}
	}
	else if (self->lever > 0.95f
		&& self->stateBV.get(FPAN_HATCH_OPEN)
		&& self->stateBV.get(FPAN_POWDER_IN))
	{
		self->stateBV.unset(UNSET_FIRE); // unset
		aud::PlaySnd(aud::FILE_SHOT_SMG, self->t_item.pos_glm); // Play gunshot sound
	}

	if (owner->atk_target != BUF_NULL)
	{
		Entity* target = (Entity*)index::GetEntityPtr(owner->atk_target);
		m::Vector2 targetoffset = m::Normalize(target->t.position - m::Vector2(self->t_item.GetPosition().x, self->t_item.GetPosition().z));
		btf32 angle_yaw = m::Vec2ToAng(targetoffset);
		self->ang_aim_offset_temp = -m::AngDif(owner->viewYaw.Deg(), glm::degrees(angle_yaw));

		m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(self->t_item.GetPosition().x, self->t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - self->t_item.GetPosition().y);
		self->ang_aim_pitch = -90.f + glm::degrees(m::Vec2ToAng(m::Normalize(targetoffsetVertical)));
	}
	else
	{
		self->ang_aim_offset_temp = 0.f;
		self->ang_aim_pitch = owner->viewPitch.Deg();
	}
}
void HeldGunDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{

	HeldGun* self = GETITEM_GUN(id);

	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);

	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	const btf32 mass = 15.f;
	const btf32 spring_rot = 2.6f;
	const btf32 spring_mov = 1.8f;
	const btf32 damping = 8.f;

	switch ((self->ePose))
	{
	case HOLDSTATE_IDLE:
		//loc = m::Lerp(loc, m::Vector3(0.13f, 1.0f, 0.1f), 0.1f);
		/*m::SpringDamper(loc.x, loc_velocity.x, 0.13f, k_mov);
		m::SpringDamper(loc.y, loc_velocity.y, 1.0f, k_mov);
		m::SpringDamper(loc.z,  loc_velocity.z, 0.1f, k_mov);*/
		m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.13f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.9f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.2f, mass, spring_mov, damping);

		/*yaw = m::Lerp(yaw, -45.f, 0.1f);
		pitch = m::Lerp(pitch, 20.f, 0.1f);*/
		/*m::SpringDamper(yaw, yaw_velocity, -45.f, k_rot);
		m::SpringDamper(pitch, pitch_velocity, 20.f, k_rot);*/
		m::SpringDamper(self->yaw, self->yaw_velocity, -80.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -33.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_AIM:
		//loc = m::Lerp(loc, m::Vector3(0.08f, 1.3f, 0.4f), 0.1f);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 1.2f + self->ang_aim_pitch * -0.005f, mass, spring_mov, damping);
		if (self->ang_aim_pitch > 0.f) // if looking down
		{
			m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.08f + self->ang_aim_pitch * 0.001f, mass, spring_mov, damping);
			m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.45f + self->ang_aim_pitch * -0.006f, mass, spring_mov, damping);
		}
		else
		{
			m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.08f, mass, spring_mov, damping);
			m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.45f + self->ang_aim_pitch * 0.002f, mass, spring_mov, damping);
		}
		//yaw = m::Lerp(yaw, 0.f, 0.1f);
		m::SpringDamper(self->yaw, self->yaw_velocity, self->ang_aim_offset_temp, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, self->ang_aim_pitch, mass, spring_rot, damping);
		break;
	case HOLDSTATE_INSPECT:
		//loc = m::Lerp(loc, m::Vector3(0.f, 1.f, 0.4f), 0.1f);
		m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.9f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.4f, mass, spring_mov, damping);
		/*yaw = m::Lerp(yaw, -15.f, 0.1f);
		pitch = m::Lerp(pitch, -30.f, 0.1f);*/
		m::SpringDamper(self->yaw, self->yaw_velocity, -15.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -30.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_BARREL:
		//loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.3f), 0.1f);
		m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.2f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.3f, mass, spring_mov, damping);
		/*yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, -70.f, 0.1f);*/
		m::SpringDamper(self->yaw, self->yaw_velocity, 0.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -70.f, mass, spring_rot, damping);
		break;
	}

	self->t_item.TranslateLocal(self->loc); // set pose
	self->t_item.Rotate(glm::radians(self->yaw), m::Vector3(0.f, 1.f, 0.f));
	self->t_item.Rotate(glm::radians(self->pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}

#undef HOLDSTATE_AIM
#undef HOLDSTATE_IDLE
#undef HOLDSTATE_INSPECT
#undef HOLDSTATE_BARREL 

#undef LATCH_PULLED
#undef FPAN_HATCH_OPEN
#undef FPAN_POWDER_IN
#undef BARREL_ARMED
#undef BARREL_ROD_IN
#undef MATCH_LIT
#undef MATCH_HELD
#undef LOST_ROD
#undef GET_CAN_FIRE
#undef UNSET_FIRE

void HeldGunOnEquip(btID id, Actor* owner)
{
	HeldGun* self = GETITEM_GUN(id);
	self->loc = m::Vector3(0.3f, 1.1f, 0.f);
	self->pitch = 90.f;
	self->yaw = 0.f;
	self->ePose = HeldGun::HOLDSTATE_AIM;
	//self->ammoInstance = owner->inventory.GetItemOfTemplate(8);
}
m::Vector3 HeldGunGetLeftHandPos(btID id)
{
	HeldGun* self = GETITEM_GUN(id);
	if (self->ePose != HeldGun::HOLDSTATE_BARREL) // If holding normally
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.4f;
	else
		return self->t_item.GetPosition() + self->t_item.GetForward() * 1.f;
}
m::Vector3 HeldGunGetRightHandPos(btID id)
{
	HeldGun* self = GETITEM_GUN(id);
	if (self->ePose != HeldGun::HOLDSTATE_BARREL) // If holding normally
		return self->t_item.GetPosition();
	else
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.75f;
}
bool HeldGunBlockTurn(btID id)
{
	HeldGun* self = GETITEM_GUN(id);
	if (self->ePose == HeldGun::HOLDSTATE_BARREL) // If holding barrel
		return true;
	else return false;
}
bool HeldGunBlockMove(btID id)
{
	HeldGun* self = GETITEM_GUN(id);
	if (self->ePose == HeldGun::HOLDSTATE_BARREL) // If holding barrel
		return true;
	else return false;
}

//-------------------------------- HELD ITEM MAGIC

void HeldMgcTick(btID id, btf32 dt, Actor * owner)
{
	//
}
void HeldMgcDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	HeldMgc* self = GETITEM_MAGIC(id);
	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);
	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	self->t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.25f + acv::items[itemid]->f_radius)); // set pose
	self->t_item.Rotate(glm::radians(-35.f), m::Vector3(1, 0, 0));
	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}
void HeldMgcOnEquip(btID id, Actor* owner)
{
	//
}
m::Vector3 HeldMgcGetLeftHandPos(btID id)
{
	HeldMgc* self = GETITEM_MAGIC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * -0.2f;
	return self->t_item.GetPosition() + m::RotateVector(m::Vector3(-0.25f, 0.f, -0.25f), self->t_item.rot_glm);
}
m::Vector3 HeldMgcGetRightHandPos(btID id)
{
	HeldMgc* self = GETITEM_MAGIC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * 0.2f;
	return self->t_item.GetPosition() + m::RotateVector(m::Vector3(0.25f, 0.f, -0.25f), self->t_item.rot_glm);
}
bool HeldMgcBlockTurn(btID id)
{
	return false;
}
bool HeldMgcBlockMove(btID id)
{
	return false;
}

//-------------------------------- HELD ITEM CONSUME

bool HeldConUse(btID id, Actor* owner)
{
	HeldCons* self = GETITEM_CONS(id);
	//owner->state.AddSpell(owner->id, ((acv::BaseItemCon*)acv::items[self->item_template])->effect);
	if (self->uses > 1u)
	{
		--self->uses;
		return true;
	}
	else
	{
		owner->inventory.DestroyID(id);
		return false;
	}
}
void HeldConTick(btID id, btf32 dt, Actor* owner)
{
	HeldCons* self = GETITEM_CONS(id);
	if (owner->inputBV.get(Actor::IN_USE_HIT) && self->uses > 0u)
	{
		owner->state.AddSpell(owner->id, ((acv::BaseItemCon*)acv::items[self->item_template])->effect);
		//if (self->uses > 1u) --self->uses;
		//else owner->inventory.DestroyID(id);
		HeldConUse(id, owner);
	}
}
void HeldConDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	HeldCons* self = GETITEM_CONS(id);
	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);
	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	//self->t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.3f)); // set pose
	self->t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.25f + acv::items[itemid]->f_radius)); // set pose
	self->t_item.Rotate(glm::radians(45.f), m::Vector3(1, 0, 0));
	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}
void HeldConOnEquip(btID id, Actor* owner)
{
	//
}

//-------------------------------- HELD ITEM MATCHLOCK

void HeldGunMatchLockTick(btID id, btf32 dt, Actor* owner)
{
	/*
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
										//loc_velocity.z -= 0.12f;
			loc_velocity.z -= m::Random(0.03f, 0.06f);
			//pitch_velocity -= 19.f;
			pitch = owner->viewPitch.Deg();
			pitch_velocity -= m::Random(30.f, 60.f);

			if (owner->atk_target != BUF_NULL)
			{
				//aud::PlayGunshotTemp(true); // Play gunshot sound

				Entity* target = (Entity*)index::GetEntityPtr(owner->atk_target);
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector2 targetoffset = m::Normalize(target->t.position - (owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f)));
				m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(t_item.GetPosition().x, t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - t_item.GetPosition().y);
				btf32 angle_yaw = m::Vec2ToAng(targetoffset);
				btf32 angle_pit = glm::radians(-90.f) + m::Vec2ToAng(m::Normalize(targetoffsetVertical));
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->viewYaw.Rad()) * 0.55f), owner->t.height, angle_yaw, angle_pit, 1.f);
				m::Vector3 spawnpos = t_item.GetPosition() + t_item.GetForward();
				index::SpawnProjectile(owner->faction, m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, angle_yaw, angle_pit, 1.f);
			}
			else
			{
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->viewYaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector3 spawnpos = t_item.GetPosition() + t_item.GetForward();
				index::SpawnProjectile(owner->faction, m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
			}
		}
	}
	else if (lever > 0.95f
		&& stateBV.get(FPAN_HATCH_OPEN)
		&& stateBV.get(FPAN_POWDER_IN))
	{
		stateBV.unset(UNSET_FIRE); // unset
		aud::PlayGunshotTemp(false); // Play gunshot sound
	}

	if (owner->atk_target != BUF_NULL)
	{
		Entity* target = (Entity*)index::GetEntityPtr(owner->atk_target);
		m::Vector2 targetoffset = m::Normalize(target->t.position - m::Vector2(t_item.GetPosition().x, t_item.GetPosition().z));
		btf32 angle_yaw = m::Vec2ToAng(targetoffset);
		ang_aim_offset_temp = -m::AngDif(owner->viewYaw.Deg(), glm::degrees(angle_yaw));

		m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(t_item.GetPosition().x, t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - t_item.GetPosition().y);
		ang_aim_pitch = -90.f + glm::degrees(m::Vec2ToAng(m::Normalize(targetoffsetVertical)));
	}
	else
	{
		ang_aim_offset_temp = 0.f;
		ang_aim_pitch = owner->viewPitch.Deg();
	}
	*/
}
void HeldGunMatchLockDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{
	/*
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
		m::SpringDamper(loc.x, loc_velocity.x, 0.23f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 0.9f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.1f, mass, spring_mov, damping);
		m::SpringDamper(yaw, yaw_velocity, -60.f, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, -33.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_AIM:
		m::SpringDamper(loc.x, loc_velocity.x, 0.08f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 1.2f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.25f, mass, spring_mov, damping);
		m::SpringDamper(yaw, yaw_velocity, ang_aim_offset_temp, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, ang_aim_pitch, mass, spring_rot, damping);
		break;
	case HOLDSTATE_INSPECT:
		m::SpringDamper(loc.x, loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 1.f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.4f, mass, spring_mov, damping);
		m::SpringDamper(yaw, yaw_velocity, -15.f, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, -30.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_BARREL:
		m::SpringDamper(loc.x, loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(loc.y, loc_velocity.y, 0.2f, mass, spring_mov, damping);
		m::SpringDamper(loc.z, loc_velocity.z, 0.3f, mass, spring_mov, damping);
		m::SpringDamper(yaw, yaw_velocity, 0.f, mass, spring_rot, damping);
		m::SpringDamper(pitch, pitch_velocity, -70.f, mass, spring_rot, damping);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, t_item.getMatrix());

	if (stateBV.get(FPAN_HATCH_OPEN))
		if (stateBV.get(FPAN_POWDER_IN))
			DrawMesh(ID_NULL, res::GetM(res::m_item_matchlock_01_pan_open_full), res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());
		else
			DrawMesh(ID_NULL, res::GetM(res::m_item_matchlock_01_pan_open), res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());
	else DrawMesh(ID_NULL, res::GetM(res::m_item_matchlock_01_pan), res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());

	DrawBlendMesh(ID_NULL, res::GetMB(res::mb_item_matchlock_01_lever), lever, res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());

	if (!stateBV.get(LOST_ROD)) // IF HAS RAMROD
	{
		if ((ePose == HOLDSTATE_BARREL && stateBV.get(FPAN_POWDER_IN)) || stateBV.get(BARREL_ROD_IN)) // IF BARREL
			DrawBlendMesh(ID_NULL, res::GetMB(res::mb_item_matchlock_01_rod_anim), rod, res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());
		else
			DrawMesh(ID_NULL, res::GetM(res::m_item_matchlock_01_rod), res::GetT(res::t_item_matchlock_01), SS_NORMAL, t_item.getMatrix());
	}
	*/
}
void HeldGunMatchLockOnEquip(btID id, Actor* owner)
{
	HeldGunMatchLock* self = (HeldGunMatchLock*)GETITEM_GUN(id);
	self->loc = m::Vector3(0.3f, 1.1f, 0.f);
	self->pitch = 90.f;
	self->yaw = 0.f;
	self->ePose = HeldGunMatchLock::HOLDSTATE_IDLE;
}
m::Vector3 HeldGunMatchLockGetLeftHandPos(btID id)
{
	//if (ePose != HOLDSTATE_BARREL) // If holding normally
	//	return t_item.GetPosition() + t_item.GetForward() * 0.35f;
	//else
	//	return t_item.GetPosition() + t_item.GetForward() * 1.f;
	return m::Vector3();
}
m::Vector3 HeldGunMatchLockGetRightHandPos(btID id)
{
	//if (ePose != HOLDSTATE_BARREL) // If holding normally
	//	return t_item.GetPosition() + t_item.GetUp() * -0.08f;
	//else
	//	return t_item.GetPosition() + t_item.GetForward() * 0.75f;
	return m::Vector3();
}
bool HeldGunMatchLockBlockTurn(btID id)
{
	//if (ePose == HOLDSTATE_BARREL) // If holding barrel
	//	return true;
	//else return false;
	return false;
}
bool HeldGunMatchLockBlockMove(btID id)
{
	//if (ePose == HOLDSTATE_BARREL) // If holding barrel
	//	return true;
	//else return false;
	return false;
}
