#include "objects_items.h"
#include "audio.hpp"
// not desirable but necessary for draw functions
#include "objects_entities.h"
#include "core.h"
#include "index.h"

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
	return self->t_item.GetPosition() + self->t_item.GetRight() * -((acv::BaseItem*)acv::items[self->id_item_template])->f_radius;
}
m::Vector3 HeldItemGetRightHandPos(btID id)
{
	HeldItem* self = GETITEM_MISC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * 0.5f;
	return self->t_item.GetPosition() + self->t_item.GetRight() * ((acv::BaseItem*)acv::items[self->id_item_template])->f_radius;
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
	HeldItem* self = GETITEM_MELEE(id);

	if (self->swinging == HeldItem::SWINGSTATE_IDLE)
	{
		if (owner->inputBV.get(Actor::IN_ACTN_A))
			self->ePose = HeldItem::HOLD_POSE_SWING_OVERHEAD;
		else if (owner->inputBV.get(Actor::IN_ACTN_B))
			self->ePose = HeldItem::HOLD_POSE_SWING_SIDE;
		else if (owner->inputBV.get(Actor::IN_ACTN_C))
			self->ePose = HeldItem::HOLD_POSE_THRUST;

		//if (owner->inputBV.get(Actor::IN_USE_HIT))
		if (owner->inputBV.get(Actor::IN_USE))
		{
			self->swinging = HeldItem::SWINGSTATE_ATTACK;
			aud::PlaySnd(aud::FILE_SWING, self->t_item.pos_glm);
		}
	}
	else if (self->swinging == HeldItem::SWINGSTATE_ATTACK)
	{
		self->swingState += 0.075f;
		// if in the middle of the swing, try damage
		if (self->swingState > 0.4f && self->swingState < 0.75f)
		{
			// if has enemy target, damage it
			if (owner->atk_target != BUF_NULL)
			{
				Entity* ent = (Entity*)GetEntityPtr(owner->atk_target);
				ent->state.Damage(300u, self->yaw);
				aud::PlaySnd(aud::FILE_SWING_CONNECT, m::Vector3(ent->t.position.x, ent->t.height, ent->t.position.y));
				ent->slideVelocity += m::AngToVec2(owner->t.yaw.Rad()) * 0.05f;
				// exit swing early
				self->swinging = HeldItem::SWINGSTATE_RESET;
			}
		}
		// if completed swing
		if (self->swingState > 1.f)
		{
			self->swinging = HeldItem::SWINGSTATE_RESET;
		}
	}
	else // reset
	{
		self->swingState -= 0.2f;
		if (self->swingState < 0.f)
		{
			self->swingState = 0.f;
			self->swinging = HeldItem::SWINGSTATE_IDLE;
		}
	}

	switch ((self->ePose))
	{
	case HeldItem::HOLD_POSE_SWING_OVERHEAD:
		self->loc = m::Lerp(m::Vector3(0.1f, 0.f, 0.25f), m::Vector3(0.1f, 0.f, 0.25f), self->swingState);
		self->yaw = m::Lerp(35.f, -45.f, self->swingState);
		self->pitch = m::Lerp(-60.f, 60.f, self->swingState);
		break;
	case HeldItem::HOLD_POSE_SWING_SIDE:
		self->loc = m::Lerp(m::Vector3(-0.2f, 0.1f, 0.3f), m::Vector3(-0.2f, 0.1f, 0.1f), self->swingState);
		self->yaw = m::Lerp(85.f, -85.f, self->swingState);
		self->pitch = m::Lerp(-20.f, -20.f, self->swingState);
		break;
	case HeldItem::HOLD_POSE_THRUST:
		self->loc = m::Lerp(m::Vector3(0.13f, 0.0f, 0.1f), m::Vector3(0.13f, 0.0f, 0.6f), self->swingState);
		self->yaw = 0.f;
		self->pitch = owner->viewPitch.Deg();
		break;
	}
}
void HeldMelDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle _pitch)
{
	HeldItem* self = GETITEM_MELEE(id);
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
	HeldItem* self = GETITEM_MELEE(id);
	switch ((self->ePose))
	{
	case HeldItem::HOLD_POSE_SWING_OVERHEAD:
		return self->t_item.GetPosition();
	case HeldItem::HOLD_POSE_SWING_SIDE:
		return self->t_item.GetPosition();
	case HeldItem::HOLD_POSE_THRUST:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.25f;
	}
	return m::Vector3(0.f, 0.f, 0.f);
}
m::Vector3 HeldMelGetRightHandPos(btID id)
{
	HeldItem* self = GETITEM_MELEE(id);
	switch ((self->ePose))
	{
	case HeldItem::HOLD_POSE_SWING_OVERHEAD:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.2f;
	case HeldItem::HOLD_POSE_SWING_SIDE:
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.3f;
	case HeldItem::HOLD_POSE_THRUST:
		return self->t_item.GetPosition() + self->t_item.GetForward() * -0.15f;
	}
	return m::Vector3(0.f, 0.f, 0.f);
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

#define HOLDSTATE_AIM HeldItem::HOLDSTATE_AIM
#define HOLDSTATE_IDLE HeldItem::HOLDSTATE_IDLE
#define HOLDSTATE_INSPECT HeldItem::HOLDSTATE_INSPECT
#define HOLDSTATE_BARREL HeldItem::HOLDSTATE_BARREL

void HeldGunTick(btID id, btf32 dt, Actor* owner)
{
	HeldItem* self = GETITEM_GUN(id);

	if (owner->inputBV.get(Actor::IN_USE) && self->ePose == HOLDSTATE_AIM && self->fire_time < tickCount_temp)
	{
		// if we try to fire, see if we can load the weapon
		if (self->id_ammoInstance == ID_NULL)
			self->id_ammoInstance = owner->inventory.GetItemOfAmmunitionType(((acv::BaseItemGun*)acv::items[self->id_item_template])->ammunition_type);

		if (self->id_ammoInstance != ID_NULL)
		{
			if (!HeldConUse(self->id_ammoInstance, owner))
				self->id_ammoInstance = owner->inventory.GetItemOfAmmunitionType(((acv::BaseItemGun*)acv::items[self->id_item_template])->ammunition_type);

			self->fire_time = tickCount_temp + 3u;

			aud::PlaySnd(aud::FILE_SHOT_SMG, self->t_item.pos_glm); // Play gunshot sound
			//loc_velocity.z -= 0.12f;
			self->loc_velocity.z -= m::Random(0.03f, 0.06f);
			//pitch_velocity -= 19.f;
			self->pitch = owner->viewPitch.Deg();
			self->pitch_velocity -= m::Random(5.f, 7.5f);

			#ifdef DEF_AUTOAIM
			if (owner->atk_target != BUF_NULL)
			{
				Entity* target = (Entity*)GetEntityPtr(owner->atk_target);
				//index::SpawnProjectile(owner->faction, owner->t.position + (m::AngToVec2(owner->yaw.Rad()) * 0.55f), owner->t.height, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 1.f);
				m::Vector2 targetoffset = m::Normalize(target->t.position - (owner->t.position + (m::AngToVec2(owner->t.yaw.Rad()) * 0.55f)));
				m::Vector2 targetoffsetVertical = m::Vector2(m::Length(target->t.position - m::Vector2(self->t_item.GetPosition().x, self->t_item.GetPosition().z)), (target->t.height + target->height * 0.5f) - self->t_item.GetPosition().y);
				btf32 angle_yaw = m::Vec2ToAng(targetoffset);
				btf32 angle_pit = glm::radians(-90.f) + m::Vec2ToAng(m::Normalize(targetoffsetVertical));
				m::Vector3 spawnpos = self->t_item.GetPosition() + self->t_item.GetForward();

				if (GetItemType(self->id_ammoInstance) == ITEM_TYPE_CONS)
					index::SpawnProjectileSpread(owner->faction, // TODO: fucking hell please make this easier to access
					((acv::BaseItemCon*)acv::items[((HeldItem*)GetItemPtr(self->id_ammoInstance))->id_item_template])->id_projectile,
						m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, angle_yaw, angle_pit, 2.5f);
				else printf("Tried to fire a projectile from non-consumable type item!\n");
			}
			else
			#endif
			{
				m::Vector3 spawnpos = self->t_item.GetPosition() + self->t_item.GetForward();
				if (GetItemType(self->id_ammoInstance) == ITEM_TYPE_CONS)
					index::SpawnProjectileSpread(owner->faction, // TODO: fucking hell please make this easier to access
					((acv::BaseItemCon*)acv::items[((HeldItem*)GetItemPtr(self->id_ammoInstance))->id_item_template])->id_projectile,
						m::Vector2(spawnpos.x, spawnpos.z), spawnpos.y, owner->viewYaw.Rad(), owner->viewPitch.Rad(), 2.5f);
				else printf("Tried to fire a projectile from non-consumable type item!\n");
			}

			// The gunfire pushes us backwards
			if (owner->grounded) // if we're on the ground, 
				owner->slideVelocity += m::AngToVec2(owner->viewYaw.Rad()) * -0.01f; // set my velocity
			else // if we're in the air
				owner->t.velocity += m::AngToVec2(owner->viewYaw.Rad()) * -0.01f; // set my velocity
		}
	}

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

	if (owner->atk_target != BUF_NULL)
	{
		Entity* target = (Entity*)GetEntityPtr(owner->atk_target);
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
		self->ang_aim_pitch += owner->aniSlideResponse * 30.f;
		if (owner->aniRun) self->ang_aim_pitch += 15.f;
	}
}
void HeldGunDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{

	HeldItem* self = GETITEM_GUN(id);

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
		/*m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.13f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.9f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.2f, mass, spring_mov, damping);

		m::SpringDamper(self->yaw, self->yaw_velocity, -80.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -33.f, mass, spring_rot, damping);*/

		m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.2f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.1f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.2f, mass, spring_mov, damping);

		m::SpringDamper(self->yaw, self->yaw_velocity, -20.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -80.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_AIM:
		//loc = m::Lerp(loc, m::Vector3(0.08f, 1.3f, 0.4f), 0.1f);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.2f + self->ang_aim_pitch * -0.005f, mass, spring_mov, damping);
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
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.z, self->loc_velocity.z, 0.4f, mass, spring_mov, damping);
		/*yaw = m::Lerp(yaw, -15.f, 0.1f);
		pitch = m::Lerp(pitch, -30.f, 0.1f);*/
		m::SpringDamper(self->yaw, self->yaw_velocity, -15.f, mass, spring_rot, damping);
		m::SpringDamper(self->pitch, self->pitch_velocity, -30.f, mass, spring_rot, damping);
		break;
	case HOLDSTATE_BARREL:
		//loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.3f), 0.1f);
		m::SpringDamper(self->loc.x, self->loc_velocity.x, 0.f, mass, spring_mov, damping);
		m::SpringDamper(self->loc.y, self->loc_velocity.y, 0.f, mass, spring_mov, damping);
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

void HeldGunOnEquip(btID id, Actor* owner)
{
	HeldItem* self = GETITEM_GUN(id);
	self->loc = m::Vector3(0.3f, 1.1f, 0.f);
	self->pitch = 90.f;
	self->yaw = 0.f;
	self->ePose = HeldItem::HOLDSTATE_AIM;
	//self->ammoInstance = owner->inventory.GetItemOfTemplate(8);
}
m::Vector3 HeldGunGetLeftHandPos(btID id)
{
	HeldItem* self = GETITEM_GUN(id);
	if (self->ePose != HeldItem::HOLDSTATE_BARREL) // If holding normally
		//return self->t_item.GetPosition() + self->t_item.GetForward() * 0.4f; // rifle
		return self->t_item.GetPosition() + self->t_item.GetUp() * -0.1f;
	else
		return self->t_item.GetPosition() + self->t_item.GetForward() * 1.f;
}
m::Vector3 HeldGunGetRightHandPos(btID id)
{
	HeldItem* self = GETITEM_GUN(id);
	if (self->ePose != HeldItem::HOLDSTATE_BARREL) // If holding normally
		return self->t_item.GetPosition();
	else
		return self->t_item.GetPosition() + self->t_item.GetForward() * 0.75f;
}
bool HeldGunBlockTurn(btID id)
{
	HeldItem* self = GETITEM_GUN(id);
	if (self->ePose == HeldItem::HOLDSTATE_BARREL) // If holding barrel
		return true;
	else return false;
}
bool HeldGunBlockMove(btID id)
{
	HeldItem* self = GETITEM_GUN(id);
	if (self->ePose == HeldItem::HOLDSTATE_BARREL) // If holding barrel
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
	HeldItem* self = GETITEM_MAGIC(id);
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
	HeldItem* self = GETITEM_MAGIC(id);
	//return self->t_item.GetPosition() + self->t_item.GetRight() * -0.2f;
	return self->t_item.GetPosition() + m::RotateVector(m::Vector3(-0.25f, 0.f, -0.25f), self->t_item.rot_glm);
}
m::Vector3 HeldMgcGetRightHandPos(btID id)
{
	HeldItem* self = GETITEM_MAGIC(id);
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
	HeldItem* self = GETITEM_CONS(id);
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
	HeldItem* self = GETITEM_CONS(id);
	if (owner->inputBV.get(Actor::IN_USE_HIT) && self->uses > 0u)
	{
		owner->state.AddSpell(owner->id, ((acv::BaseItemCon*)acv::items[self->id_item_template])->id_effect);
		//if (self->uses > 1u) --self->uses;
		//else owner->inventory.DestroyID(id);
		HeldConUse(id, owner);
	}
}
void HeldConDraw(btID id, btID itemid, m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch)
{
	HeldItem* self = GETITEM_CONS(id);
	self->t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	self->t_item.SetRotation(0.f);
	self->t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));
	//self->t_item.TranslateLocal(m::Vector3(0.f, 1.f, 0.3f)); // set pose
	self->t_item.TranslateLocal(m::Vector3(0.f, 0.1f, 0.25f + acv::items[itemid]->f_radius)); // set pose
	self->t_item.Rotate(glm::radians(45.f), m::Vector3(1, 0, 0));
	DrawMesh(ID_NULL, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, self->t_item.getMatrix());
}
void HeldConOnEquip(btID id, Actor* owner)
{
	//
}
