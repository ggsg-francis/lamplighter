#include "ec_actor.h"

#include "archive.hpp"
#include "core.h"
#include "audio.hpp"
#include "index.h"
#include "objects_inventory.h"
#include "objects_items.h"
#include "ec_misc.h"
#include "nonpc.h"

//#define LEGLEN(a,x) 0.75f
//#define ARMLEN(a,x) 0.75f
#define ARMLEN(a,x) acv::actor_templates[a].leng_arm
#define LEGLEN(a,x) acv::actor_templates[a].leng_leg
// leg disconnect length? not sure
#define legDClen ARMLEN(0,0)

//#define bodylen 0.65
#define BODYLEN acv::actor_templates[0].leng_body

//#define hip_width 0.125f
#define hip_width acv::actor_templates[0].jpos_arm_rt

#define velocityStepMult (8.f * LEGLEN(chr->actorBase,0)) // How far to place our foot ahead when walking

#define STEP_LEN 0.45f

#define FOOT_H 0.08f
#define HAND_LEN 0.12f

AnimFramesVec3 framesBodyIdle(4,
	AnimKeyframeVec3(m::Vector3(0.9f, 0.982f, 0.f), 1.5f),
	AnimKeyframeVec3(m::Vector3(0.9f, 0.985f, 0.f), 2.5f),
	AnimKeyframeVec3(m::Vector3(0.88f, 0.985f, 0.f), 1.5f),
	AnimKeyframeVec3(m::Vector3(0.88f, 0.982f, 0.f), 2.5f));

AnimFramesVec3 framesBodyWalk(5,
	AnimKeyframeVec3(m::Vector3(0.84f, 0.985f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.92f, 0.985f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.92f, 0.985f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.84f, 0.985f, 0.f), 0.125f / 2.f),
	AnimKeyframeVec3(m::Vector3(0.78f, 0.985f, 0.f), 0.125f / 2.f));

AnimFramesVec3 framesBodyRun(5,
	AnimKeyframeVec3(m::Vector3(0.85f, 0.88f, 0.f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.98f, 0.93f, 0.f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.98f, 0.93f, 0.f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.85f, 0.93f, 0.f), 0.25f / 2.f),
	AnimKeyframeVec3(m::Vector3(0.75f, 0.9f, 0.f), 0.25f / 2.f));

AnimFramesVec3 framesBodyCrouch(4,
	AnimKeyframeVec3(m::Vector3(0.5f, 0.8f, 0.f), 0.8f),
	AnimKeyframeVec3(m::Vector3(0.5f, 0.85f, 0.f), 1.8f),
	AnimKeyframeVec3(m::Vector3(0.45f, 0.85f, 0.f), 0.8f),
	AnimKeyframeVec3(m::Vector3(0.45f, 0.8f, 0.f), 1.8f));

AnimFramesVec3 framesBodyCrouchWalk(4,
	AnimKeyframeVec3(m::Vector3(0.4f, 0.85f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.55f, 0.85f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.55f, 0.85f, 0.f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.4f, 0.85f, 0.f), 0.125f));

AnimFramesVec3 frames_handtest_l(2,
	AnimKeyframeVec3(m::Vector3(-0.25f, -0.15f, 0.1f), 1.5f),
	AnimKeyframeVec3(m::Vector3(-0.25f, -0.14f, 0.1f), 1.5f));

AnimFramesVec3 frames_handtest_r(2,
	AnimKeyframeVec3(m::Vector3(0.25f, -0.15f, 0.1f), 1.5f),
	AnimKeyframeVec3(m::Vector3(0.25f, -0.14f, 0.1f), 1.5f));

AnimFramesVec3 frames_handtest_l_crouch(2,
	AnimKeyframeVec3(m::Vector3(-0.32f, -0.05f, 0.1f), 1.5f),
	AnimKeyframeVec3(m::Vector3(-0.325f, -0.04f, 0.1f), 1.5f));

AnimFramesVec3 frames_handtest_r_crouch(2,
	AnimKeyframeVec3(m::Vector3(0.32f, -0.05f, 0.1f), 1.5f),
	AnimKeyframeVec3(m::Vector3(0.325f, -0.04f, 0.1f), 1.5f));

AnimFramesVec3 framesHandLWalk(4,
	AnimKeyframeVec3(m::Vector3(-0.23f, -0.16f, -0.07f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.2f, -0.15f, 0.2f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.23f, -0.05f, 0.18f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.29f, -0.08f, -0.12f), 0.25f));

AnimFramesVec3 framesHandRWalk(4,
	AnimKeyframeVec3(m::Vector3(0.23f, -0.05f, 0.18f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.29f, -0.08f, -0.12f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.23f, -0.16f, -0.07f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.2f, -0.15f, 0.2f), 0.25f));

AnimFramesVec3 framesHandLRun(4,
	AnimKeyframeVec3(m::Vector3(-0.f, 0.15f, 0.35f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.23f, 0.05f, 0.17f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.29f, 0.08f, -0.22f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.23f, 0.16f, -0.2f), 0.5f));

AnimFramesVec3 framesHandRRun(4,
	AnimKeyframeVec3(m::Vector3(0.29f, 0.08f, -0.22f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.23f, 0.16f, -0.20f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.f, 0.15f, 0.35f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.23f, 0.05f, 0.17f), 0.5f));

AnimFramesVec3 framesHandLWalkCrouch(4,
	AnimKeyframeVec3(m::Vector3(-0.2f, 0.3f + -0.15f, 0.25f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.23f, 0.3f + -0.05f, 0.2f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.29f, 0.3f + -0.05f, -0.25f), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.23f, 0.3f + -0.12f, -0.2f), 0.25f));

AnimFramesVec3 framesHandRWalkCrouch(4,
	AnimKeyframeVec3(m::Vector3(0.29f, 0.3f + -0.05f, -0.25f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.23f, 0.3f + -0.12f, -0.2f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.2f, 0.3f + -0.15f, 0.25f), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.23f, 0.3f + -0.05f, 0.2f), 0.25f));

AnimFramesVec3 framesFootLWalk(4,
	AnimKeyframeVec3(m::Vector3(-0.08f, -1.f, STEP_LEN), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.08f, -1.f, -STEP_LEN), 0.25f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.65f, -0.2f), 0.125f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.8f, 0.2f), 0.125f));

AnimFramesVec3 framesFootRWalk(4,
	AnimKeyframeVec3(m::Vector3(0.08f, -1.f, -STEP_LEN), 0.25f),
	AnimKeyframeVec3(m::Vector3(0.05f, -0.65f, -0.2f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.05f, -0.8f, 0.2f), 0.125f),
	AnimKeyframeVec3(m::Vector3(0.08f, -1.f, STEP_LEN), 0.5f));

AnimFramesVec3 framesFootLRun(4,
	AnimKeyframeVec3(m::Vector3(-0.05f, -1.f, -0.2f), 0.5f), // ground
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.5f, -0.7f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.5f, -0.2f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.5f, 0.3f), 0.5f));

AnimFramesVec3 framesFootRRun(4,
	AnimKeyframeVec3(m::Vector3(0.05f, -0.5f, -0.2f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.05f, -0.5f, 0.3f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.05f, -1.f, -0.2f), 0.5f), // ground
	AnimKeyframeVec3(m::Vector3(0.05f, -0.5f, -0.7f), 0.5f));

AnimFramesVec3 framesFootLIdle(2,
	AnimKeyframeVec3(m::Vector3(-0.15f, -1.f, -0.1f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.15f, -1.f, -0.1f), 0.5f));

AnimFramesVec3 framesFootRIdle(2,
	AnimKeyframeVec3(m::Vector3(0.15f, -1.f, 0.1f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.15f, -1.f, 0.1f), 0.5f));

AnimFramesVec3 framesFootLJump(2,
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.85f, -0.1f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.85f, -0.1f), 0.5f));

AnimFramesVec3 framesFootRJump(2,
	AnimKeyframeVec3(m::Vector3(0.05f, -0.6f, -0.3f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.05f, -0.6f, -0.3f), 0.5f));

AnimFramesVec3 framesFootLJumpCrouch(2,
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.65f, -0.1f), 0.5f),
	AnimKeyframeVec3(m::Vector3(-0.05f, -0.65f, -0.1f), 0.5f));

AnimFramesVec3 framesFootRJumpCrouch(2,
	AnimKeyframeVec3(m::Vector3(0.05f, -0.24f, -0.3f), 0.5f),
	AnimKeyframeVec3(m::Vector3(0.05f, -0.24f, -0.3f), 0.5f));

void ActorOnHitGround(ECActor* chr)
{
	if (chr->foot_state == ECActor::eL_DOWN) aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_A, chr->fpCurrentL);
	else aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_B, chr->fpCurrentR);
	// Swap feet
	if (chr->foot_state == ECActor::eL_DOWN) chr->foot_state = ECActor::eR_DOWN;
	else if (chr->foot_state == ECActor::eR_DOWN) chr->foot_state = ECActor::eL_DOWN;
}

void ActorTryHoldHand(lid id_self, lid id)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	ECActor* actr = (ECActor*)GetEntityPtr(id);

	lui32 id1 = self->inventory.GetFirstEmptySpace();
	lui32 id2 = actr->inventory.GetFirstEmptySpace();
	if (id1 != ID_NULL && id2 != ID_NULL) {
		self->inv_active_slot = id1;
		actr->inv_active_slot = id2;
		if (self->aniHandHoldTarget == id)
			self->aniHandHoldTarget = ID_NULL;
		else
			self->aniHandHoldTarget = id;
		if (actr->aniHandHoldTarget == id_self)
			actr->aniHandHoldTarget = ID_NULL;
		else
			actr->aniHandHoldTarget = id_self;
	}
}

void ActorTakeItem(lid id_self, lid id)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	ECSingleItem* item = (ECSingleItem*)GetEntityPtr(id);
	HeldItem* item_held = GETITEMINST(item->item_instance);
	lui32 slot_added = self->inventory.TransferItemRecv(item->item_instance);
	core::DestroyEntity(id);
	if (slot_added == self->inv_active_slot)
		ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	if (core::players[0] == id_self)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::ItemRecord*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(0, string);
	}
	else if (core::players[1] == id_self)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::ItemRecord*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(1, string);
	}
}

void ActorDropItem(lid id_self, lid slot)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (slot < self->inventory.items.Size() && self->inventory.items.Used(slot))
	{
		m::Vector2 throwDir = m::AngToVec2(self->viewYaw.Rad());
		lid item_entity = core::SpawnEntityItem(self->inventory.items[slot],
			self->t.position + (throwDir * (self->radius + acv::items[((HeldItem*)GetItemInstance(self->inventory.items[slot]))->id_item_template]->f_radius)),
			self->t.altitude, self->viewYaw.Deg());
		ENTITY(item_entity)->velocity = throwDir * 0.05f + self->velocity;
		self->inventory.TransferItemSendIndex(slot);
		ActorDecrEquipSlot(id_self);
	}
}

void ActorDropAllItems(lid id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	for (lui32 slot = 0u; slot < INV_SIZE; ++slot)
	{
		if (slot < self->inventory.items.Size() && self->inventory.items.Used(slot))
		{
			core::SpawnEntityItem(self->inventory.items[slot], m::Vector2(
				m::Random(self->t.position.x - 0.5f, self->t.position.x + 0.5f),
				m::Random(self->t.position.y - 0.5f, self->t.position.y + 0.5f)),
				self->t.altitude,
				m::Random(0.f, 360.f));
			self->inventory.TransferItemSendIndex(slot);
			ActorDecrEquipSlot(id_self);
		}
	}
}

void ActorSetEquipSlot(lid id_self, lui32 index)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (index < self->inventory.items.Size())
	{
		self->inv_active_slot = index;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorIncrEquipSlot(lid id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (self->inv_active_slot < self->inventory.items.Size() - 1u)
	{
		++self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorDecrEquipSlot(lid id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (self->inv_active_slot > 0u)
	{
		--self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

lf32 Actor_FindGroundHeight(m::Vector3 position)
{
	CellSpace cs;
	core::GetCellSpaceInfo(m::Vector2(position.x, position.z), cs);
	lf32 height;
	#if DEF_GRID
	env::GetHeight(height, cs);
	#else
	env::GetNearestSurfaceHeight(height, cs, position.y);
	#endif
	return height;
}

void Actor_ClampLegs(ECActor* chr)
{
	// Duplicated code
	m::Vector3 newpos = chr->t_body.GetPosition();
	m::Vector3 jointPosR = newpos + chr->t_body.GetRight() * hip_width;
	m::Vector3 jointPosL = newpos + chr->t_body.GetRight() * -hip_width;
	// Foot position distance check
	if (m::Length(chr->fpCurrentL - jointPosL) > LEGLEN(chr->actorBase, 0))
		chr->fpCurrentL = jointPosL + m::Normalize(chr->fpCurrentL - jointPosL) * LEGLEN(chr->actorBase, 0);
	if (m::Length(chr->fpCurrentR - jointPosR) > LEGLEN(chr->actorBase, 0))
		chr->fpCurrentR = jointPosR + m::Normalize(chr->fpCurrentR - jointPosR) * LEGLEN(chr->actorBase, 0);
}

void ActorTick(lid id, void* ent, lf32 dt)
{
	ECActor* chr = (ECActor*)ent;

	// Update view target (used to pick up items and stuff)
	chr->viewtarget = core::GetEntityViewTarget(id);

	chr->input.move = m::Normalize(chr->input.move);

	// modify input to keep handholding characters together
	if (chr->aniHandHoldTarget != ID_NULL) {
		ECActor* actor = ACTOR(chr->aniHandHoldTarget);
		m::Vector2 destpos = actor->t.position;
		if (m::Length(destpos - chr->t.position) > 0.85f) {
			chr->input.move += (destpos - chr->t.position);
		}
	}

	// apply crouch input
	if (chr->input.bits.get(IN_CROUCH))
		chr->aniCrouch = !chr->aniCrouch;
	chr->aniRun = chr->input.bits.get(IN_RUN);

	// Animate slide crouch
	lf32 slide = (m::Length(chr->slideVelocity) / dt) * 0.5f;
	if (slide > 1.f) chr->aniSlideResponse = m::Lerp(chr->aniSlideResponse, 1.f, 0.3f);
	else chr->aniSlideResponse = m::Lerp(chr->aniSlideResponse, slide, 0.3f);

	// hold hands check if our target is bullshit (gone, or too far away, or has started holding an item
	if (chr->aniHandHoldTarget != ID_NULL)
		if (!GetEntityExists(chr->aniHandHoldTarget) ||
			m::Length(ACTOR(chr->aniHandHoldTarget)->t.position - chr->t.position) > 1.4f ||
			ACTOR(chr->aniHandHoldTarget)->inventory.items.Used(ACTOR(chr->aniHandHoldTarget)->inv_active_slot) ||
			chr->inventory.items.Used(chr->inv_active_slot)) // or we started holding a thing
			chr->aniHandHoldTarget = ID_NULL;
	// or if our target is ourself somehow
	if (chr->aniHandHoldTarget == id)
		chr->aniHandHoldTarget = ID_NULL;

	bool canJump = true;
	#if DEF_GRID
	switch (acv::props[env::eCells.prop[chr->t.csi.c[eCELL_I].x][chr->t.csi.c[eCELL_I].y]].floorType) {
	case acv::PropRecord::FLOOR_QUICKSAND:
		canJump = false;
		chr->aniRun = false;
		break;
	}
	#else
	//env::get
	/*switch (acv::props[env::eCells.prop[chr->t.csi.c[eCELL_I].x][chr->t.csi.c[eCELL_I].y]].floorType) {
	case acv::PropRecord::FLOOR_QUICKSAND:
		canJump = false;
		chr->aniRun = false;
		break;
	}*/
	#endif

	lf32 speedMult = 1.f;
	if (chr->aniRun) speedMult = 2.15f;

	// Jump input
	if (canJump) {
		if (chr->input.bits.get(IN_JUMP)) {
			#if DEF_LONGJUMP // enable to turn on hover jumping
			if (chr->jump_state == ECActor::eJUMP_JUMP)
				chr->t.height_velocity += 0.005f; // hover
			#endif
			if (chr->Grounded()) {
				// Enter jump
				if (chr->aniCrouch) // jump higher out of a crouch position
					chr->altitude_velocity = 0.135f * chr->agility;
				else
					chr->altitude_velocity = 0.1f * chr->agility;
				chr->velocity = chr->input.move * dt * (chr->speed * speedMult * (1.f + chr->agility * 0.1f)) + chr->slideVelocity;
				chr->aniCrouch = false;
				chr->jump_state = ECActor::eJUMP_JUMP;
			}
		}
	}
	if (chr->aniRun) chr->aniCrouch = false;

	bool can_move = true;
	bool can_turn = true;
	if (chr->inventory.items.Used(chr->inv_active_slot)) {
		can_move = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
		can_turn = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
	}
	
	if (chr->activeFlags.get(ECCommon::eDIED_REPORT)) {
		ActorDropAllItems(id);
		chr->activeFlags.unset(ECCommon::eDIED_REPORT);
	}
	if (chr->activeFlags.get(ECCommon::eALIVE)) {
		if (chr->Grounded()) {
			if (can_move) {
				if (!chr->aniCrouch)
					chr->velocity = m::Lerp(chr->velocity, chr->input.move * dt * chr->speed * speedMult, 0.3f);
				else
					chr->velocity = m::Lerp(chr->velocity, chr->input.move * dt * chr->speed * 0.6f, 0.3f);
			}
			else {
				chr->input.move = m::Vector2(0.f, 0.f);
				chr->velocity = m::Lerp(chr->velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
		}
		if (can_turn) {
			lf32 angdif = m::AngDifAbs(chr->t.yaw.Deg(), chr->viewYaw.Deg());
			if (angdif > 85.f)
				if (angdif - 85.f < 8.f)
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), angdif - 85.f); // Rotate body towards the target direction
				else
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), 8.f); // Rotate body towards the target direction
			else if (m::Length(chr->input.move) > 0.2f) {
				chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), 8.f); // Rotate body towards the target direction
			}
		}

		//-------------------------------- APPLY MOVEMENT

		Entity_PhysicsTick(chr, id, dt);

		// set height for collision
		chr->height = chr->t_head.GetPosition().y - chr->t_body.GetPosition().y;

		//-------------------------------- RUN AI FUNCTION

		// if AI controlled run the AI function
		if (chr->aiControlled) NPCTick(id);
		// if player controlled, just aim at whatever's in front of us
		else chr->atk_target = core::GetViewTargetEntity(id, 100.f, fac::enemy);

		////-------------------------------- RUN ITEM TICK

		//if (chr->inventory.items.Used(chr->inv_active_slot))
		//	ItemTick(chr->inventory.items[chr->inv_active_slot], dt, id, chr);
	} // End if alive

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	lf32 velLen = m::Length(chr->velocity);

	//-------------------------------- HANDLE ANIMATION

	// will hopefully be used as a root for all matrix calculations
	// god knows how long until that can happen though
	graphics::Matrix4x4 rootMatrix_vel;
	graphics::Matrix4x4 rootMatrix_legs;
	graphics::Matrix4x4 rootMatrix_arms;
	{
		m::Angle angtest = chr->t.yaw;
		lf32 angdif = m::AngDifAbs(chr->t.yaw.Deg(), chr->viewYaw.Deg());
		angtest.RotateTowards(chr->viewYaw.Deg(), angdif * 0.5f);

		graphics::MatrixTransform(rootMatrix_vel,
			m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y),
			m::Vector3(chr->velocity.x, 0.f, chr->velocity.y), m::Vector3(0.f, 1.f, 0.f));
		graphics::MatrixTransform(rootMatrix_legs,
			m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y),
			chr->t.yaw.Rad());
		graphics::MatrixTransform(rootMatrix_arms,
			m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y),
			angtest.Rad());
	}

	//-------------------------------- FEET AND BODY
	// Animate stand height
	m::Vector3 animBody;
	// Run walking animation
	m::Vector3 animFootL;
	m::Vector3 animFootR;
	//--------------------------------
	// In the air
	if (!chr->Grounded()) {
		if (chr->aniCrouch) {
			chr->ap_fpCurrentL.Tick(dt, &framesFootLJumpCrouch, &animFootL);
			chr->ap_fpCurrentR.Tick(dt, &framesFootRJumpCrouch, &animFootR);
		}
		else{
			chr->ap_fpCurrentL.Tick(dt, &framesFootLJump, &animFootL);
			chr->ap_fpCurrentR.Tick(dt, &framesFootRJump, &animFootR);
		}
		chr->ap_StandHeight.Tick(dt, &framesBodyIdle, &animBody);
	}
	// Moving
	else if (velLen > 0.05f * dt) {
		// Running
		if (chr->aniRun) {
			chr->ap_fpCurrentL.Tick((0.25f / STEP_LEN) * velLen, &framesFootLRun, &animFootL);
			chr->ap_fpCurrentR.Tick((0.25f / STEP_LEN) * velLen, &framesFootRRun, &animFootR);
			chr->ap_StandHeight.Tick((0.25f / STEP_LEN) * velLen, &framesBodyRun, &animBody);
		}
		// Walking
		else {
			chr->ap_fpCurrentL.Tick((0.25f / STEP_LEN) * velLen, &framesFootLWalk, &animFootL);
			chr->ap_fpCurrentR.Tick((0.25f / STEP_LEN) * velLen, &framesFootRWalk, &animFootR);
			chr->aniCrouch ?
				chr->ap_StandHeight.Tick((0.25f / STEP_LEN) * velLen, &framesBodyCrouchWalk, &animBody) :
				chr->ap_StandHeight.Tick((0.25f / STEP_LEN) * velLen, &framesBodyWalk, &animBody);
		}
	}
	// Standing
	else {
		chr->ap_fpCurrentL.Tick(dt, &framesFootLIdle, &animFootL);
		chr->ap_fpCurrentR.Tick(dt, &framesFootRIdle, &animFootR);
		chr->aniCrouch ?
			chr->ap_StandHeight.Tick(dt, &framesBodyCrouch, &animBody) :
			chr->ap_StandHeight.Tick(dt, &framesBodyIdle, &animBody);
	}
	// Apply the animation
	animFootL.y *= chr->aniStandHeight - FOOT_H;
	animFootR.y *= chr->aniStandHeight - FOOT_H;
	// Apply relative positions
	chr->fpCurrentL = graphics::MatrixGetPosition(rootMatrix_legs) +
		graphics::MatrixGetRight(rootMatrix_legs) * animFootL.x +
		graphics::MatrixGetUp(rootMatrix_legs) * animFootL.y +
		graphics::MatrixGetForward(rootMatrix_vel) * animFootL.z;
	chr->fpCurrentR = graphics::MatrixGetPosition(rootMatrix_legs) +
		graphics::MatrixGetRight(rootMatrix_legs) * animFootR.x +
		graphics::MatrixGetUp(rootMatrix_legs) * animFootR.y +
		graphics::MatrixGetForward(rootMatrix_vel) * animFootR.z;
	// Raise feet above ground height
	lf32 hl = chr->t.altitude - Actor_FindGroundHeight(chr->fpCurrentL) + 0.075f;
	lf32 hr = chr->t.altitude - Actor_FindGroundHeight(chr->fpCurrentR) + 0.075f;
	if (chr->fpCurrentL.y < hl) chr->fpCurrentL.y = hl;
	if (chr->fpCurrentR.y < hr) chr->fpCurrentR.y = hr;
	// Clamp positions so they never exceed the length of a leg in distance
	Actor_ClampLegs(chr);

	// Apply the animation
	chr->aniStandHeight = LEGLEN(chr->actorBase, 0) * animBody.x;

	if (chr->activeFlags.get(ECCommon::eALIVE)) {
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		// Set head transform
		chr->t_head.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude
			- (chr->aniSlideResponse * 0.25f) + BODYLEN * animBody.y, chr->t.position.y));

		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_head.Rotate(chr->viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

		// Set body transform
		chr->t_body.SetPosition(m::Vector3(
			chr->t.position.x,
			chr->t.altitude - (chr->aniSlideResponse * 0.125f),
			chr->t.position.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));

		// Head position distance check
		if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > BODYLEN)
			chr->t_head.SetPosition(m::Lerp(chr->t_head.GetPosition(), chr->t_body.GetPosition()
				+ m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * BODYLEN, 0.001f));

		//-------------------------------- RUN ITEM TICK (SHOULDNT REALLY BE HERE BUT HAX I GUESS)

		if (chr->inventory.items.Used(chr->inv_active_slot))
			ItemTick(chr->inventory.items[chr->inv_active_slot], dt, id, chr);

		//-------------------------------- HANDS
		// Let the inventory item animate our hands
		if (chr->inventory.items.Used(chr->inv_active_slot)) {
			// Get held item
			HeldItem* heldItem = ((HeldItem*)GetItemInstance(chr->inventory.items[chr->inv_active_slot]));
			// Get hand positions
			chr->handPosR = ItemRHPos(chr->inventory.items[chr->inv_active_slot]);
			chr->handPosL = ItemLHPos(chr->inventory.items[chr->inv_active_slot]);
		}
		// Empty handed
		else {
			// TEMP, this should be in tick
			m::Vector3 v3l;
			m::Vector3 v3r;
			// if in the air
			if (!chr->Grounded()) {
				chr->ap_HandPosL.Tick(dt, &frames_handtest_l_crouch, &v3l);
				chr->ap_HandPosR.Tick(dt, &frames_handtest_r_crouch, &v3r);
			}
			// if running
			else if (velLen > 0.05f * dt) {
				if (chr->aniRun) {
					chr->ap_HandPosL.Tick((0.25f / STEP_LEN) * velLen, &framesHandLRun, &v3l);
					chr->ap_HandPosR.Tick((0.25f / STEP_LEN) * velLen, &framesHandRRun, &v3r);
				}
				else {
					chr->aniCrouch ?
						chr->ap_HandPosL.Tick((0.25f / STEP_LEN) * velLen, &framesHandLWalkCrouch, &v3l) :
						chr->ap_HandPosL.Tick((0.25f / STEP_LEN) * velLen, &framesHandLWalk, &v3l);
					chr->aniCrouch ?
						chr->ap_HandPosR.Tick((0.25f / STEP_LEN) * velLen, &framesHandRWalkCrouch, &v3r) :
						chr->ap_HandPosR.Tick((0.25f / STEP_LEN) * velLen, &framesHandRWalk, &v3r);
				}
			}
			// if standing
			else {
				chr->aniCrouch ?
					chr->ap_HandPosL.Tick(dt, &frames_handtest_l_crouch, &v3l) :
					chr->ap_HandPosL.Tick(dt, &frames_handtest_l, &v3l);
				chr->aniCrouch ?
					chr->ap_HandPosR.Tick(dt, &frames_handtest_r_crouch, &v3r) :
					chr->ap_HandPosR.Tick(dt, &frames_handtest_r, &v3r);
			}
			chr->handPosL = graphics::MatrixGetPosition(rootMatrix_arms) +
				graphics::MatrixOrientVector(rootMatrix_arms, v3l);
			chr->handPosR = graphics::MatrixGetPosition(rootMatrix_arms) +
				graphics::MatrixOrientVector(rootMatrix_arms, v3r);

			// hold hands override
			if (chr->aniHandHoldTarget != ID_NULL) {
				if (m::Dot(chr->t.position - ACTOR(chr->aniHandHoldTarget)->t.position, m::AngToVec2(chr->t.yaw.Rad() - glm::degrees(90.f))) > 0.f)
					chr->handPosL = (ACTOR(chr->aniHandHoldTarget)->t_body.GetPosition() + chr->t_body.GetPosition()) * 0.5f;
				else
					chr->handPosR = (ACTOR(chr->aniHandHoldTarget)->t_body.GetPosition() + chr->t_body.GetPosition()) * 0.5f;
			}
		}
	}
	else
	{
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		chr->t_body.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_body.Rotate(glm::radians(90.f), m::Vector3(1, 0, 0));

		// Set head transform
		chr->t_head.SetPosition(chr->t_body.GetPosition());
		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		//t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.Translate(chr->t_body.GetUp() * BODYLEN);
	}
}

void MatrixIK(lf32 iklen, m::Vector3 start, m::Vector3 target, m::Vector3 elbowdir,
	graphics::Matrix4x4* mat1, graphics::Matrix4x4* mat2, graphics::Matrix4x4* mat3,
	bool inverse_bend, bool XFlip, lf32 handlen) {
	lf32 len = m::Length(start - target) - handlen;
	if (len > iklen) len = iklen;
	lf32 lenUp = sqrtf(iklen * iklen - len * len); // Pythagorean theorem
	m::Vector3 vecfw = m::Normalize(target - start);
	m::Vector3 vecside = m::Normalize(m::Cross(vecfw, elbowdir));
	m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside)) * -1.f;
	if (XFlip) {
		// upper
		graphics::MatrixTransformXFlip(*mat1, start, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		// lower
		graphics::MatrixTransformXFlip(*mat2, start - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		// hand/foot
		graphics::MatrixTransformXFlip(*mat3, start - vecfw * (iklen - len), vecfw, vecup);
	}
	else {
		// upper
		graphics::MatrixTransform(*mat1, start, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		// lower
		graphics::MatrixTransform(*mat2, start - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		// hand/foot
		graphics::MatrixTransform(*mat3, start - vecfw * (iklen - len), vecfw, vecup);
	}
}

void ActorDraw(lid id, void* ent)
{
	ECActor* chr = (ECActor*)ent;

	// will hopefully be used as a root for all matrix calculations
	// god knows how long until that can happen though
	graphics::Matrix4x4 rootMatrix;
	{
		m::Angle angtest = chr->t.yaw;
		lf32 angdif = m::AngDifAbs(chr->t.yaw.Deg(), chr->viewYaw.Deg());
		angtest.RotateTowards(chr->viewYaw.Deg(), angdif * 0.5f);

		//graphics::MatrixTransform(rootMatrix, m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y), chr->viewYaw.Rad());
		graphics::MatrixTransform(rootMatrix, m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y), angtest.Rad());
	}

	graphics::Matrix4x4 matLegHipR, matLegUpR, matLegLoR, matLegFootR;
	graphics::Matrix4x4 matLegHipL, matLegUpL, matLegLoL, matLegFootL;
	graphics::Matrix4x4 matBodyUp, matBodyLo;

	Transform3D t_upperbody = chr->t_body;
	t_upperbody.Rotate(glm::radians(m::AngDif(chr->viewYaw.Deg(), chr->t.yaw.Deg())), m::Vector3(0, 1, 0));

	lf32 lerpAmt = 0.05f * chr->speed;

	//-------------------------------- SET ACTOR SHADER PARAMETERS

	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).Use();
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_A, chr->skin_col_a.x, chr->skin_col_a.y, chr->skin_col_a.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_B, chr->skin_col_b.x, chr->skin_col_b.y, chr->skin_col_b.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_C, chr->skin_col_c.x, chr->skin_col_c.y, chr->skin_col_c.z);

	//-------------------------------- DRAW BODY

	lf32 len = m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition());
	if (len > BODYLEN) len = BODYLEN;
	lf32 lenUp = sqrtf(BODYLEN * BODYLEN - len * len); // Pythagorean theorem
	m::Vector3 vecfw = m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition());
	m::Vector3 vecside = m::Normalize(m::Cross(vecfw, chr->t_body.GetForward()));
	m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
	m::Vector3 vecside_upper = m::Normalize(m::Cross(vecfw, chr->t_head.GetForward()));
	m::Vector3 vecup_upper = (m::Normalize(m::Cross(vecfw, vecside_upper)) + vecup);
	graphics::MatrixTransform(matBodyLo, chr->t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp), vecup * -1.f);
	graphics::MatrixTransform(matBodyUp,
		chr->t_body.GetPosition() + (vecup * lenUp),
		m::Normalize(vecfw * len + vecup * lenUp),
		vecup_upper * -1.f);
	DrawMeshDeform(id,
		acv::GetMD(acv::actor_templates[chr->actorBase].m_body),
		acv::GetT(acv::actor_templates[chr->actorBase].t_body), SS_NORMAL, 2u,
		matBodyLo, matBodyUp, graphics::Matrix4x4(), graphics::Matrix4x4());

	//-------------------------------- CALCULATE ARMS

	m::Vector3 chestOrigin = graphics::MatrixGetPosition(matBodyUp) - graphics::MatrixGetForward(matBodyUp) * (BODYLEN - 0.1f);

	lf32 shoulder_width = 0.12f;

	m::Vector3 jointPosR = chestOrigin + graphics::MatrixGetRight(matBodyUp) * shoulder_width;
	m::Vector3 jointPosL = chestOrigin + graphics::MatrixGetRight(matBodyUp) * -shoulder_width;

	// Arm orientation vectors
	m::Vector3 elbowDirR = m::Normalize(graphics::MatrixOrientVector(rootMatrix, m::Vector3(-0.3f, 0.5f, 0.5f)));
	m::Vector3 elbowDirL = m::Normalize(graphics::MatrixOrientVector(rootMatrix, m::Vector3(0.3f, 0.5f, 0.5f)));

	//-------------------------------- DRAW HELD ITEM

	if (chr->inventory.items.Used(chr->inv_active_slot)) {
		// Get held item
		HeldItem* heldItem = ((HeldItem*)GetItemInstance(chr->inventory.items[chr->inv_active_slot]));
		// Draw held item
		ItemDraw(chr->inventory.items[chr->inv_active_slot],
			GETITEMINST(chr->inventory.items[chr->inv_active_slot])->id_item_template);
	}

	//-------------------------------- DRAW ARMS

	// Arm right
	MatrixIK(ARMLEN(chr->actorBase, 0), jointPosR, chr->handPosR, elbowDirR,
		&matLegUpR, &matLegLoR, &matLegFootR, true, true, HAND_LEN);
	// Draw arm
	graphics::SetFrontFaceInverse();
	DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpR, matLegLoR, matLegFootR);
	graphics::SetFrontFace();

	// Arm left
	MatrixIK(ARMLEN(chr->actorBase, 0), jointPosL, chr->handPosL, elbowDirL,
		&matLegUpL, &matLegLoL, &matLegFootL, true, false, HAND_LEN);
	// Draw arm
	DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpL, matLegLoL, matLegFootL);

	//-------------------------------- DRAW LEGS

	if (chr->activeFlags.get(ECCommon::eALIVE))
	{
		lf32 velocityAmt = m::Length(chr->velocity - chr->slideVelocity);

		m::Vector3 newpos = chr->t_body.GetPosition();

		jointPosR = newpos + graphics::MatrixGetRight(matBodyLo) * hip_width;
		jointPosL = newpos + graphics::MatrixGetRight(matBodyLo) * -hip_width;

		// generate matrices

		// Leg right
		lf32 len = m::Length(jointPosR - chr->fpCurrentR);
		if (len > LEGLEN(chr->actorBase, 0)) len = LEGLEN(chr->actorBase, 0);
		lf32 lenUp = sqrtf(LEGLEN(chr->actorBase, 0) * LEGLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(chr->fpCurrentR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, (m::Normalize(chr->t_body.GetForward() + chr->t_body.GetRight() * 0.23f))));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		m::Vector3 vecup_inv = vecup * -1.f;
		graphics::MatrixTransformXFlip(matLegHipR, jointPosR, graphics::MatrixGetForward(matBodyLo), graphics::MatrixGetUp(matBodyLo));
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegFootR, chr->fpCurrentR + m::Vector3(0.f, LEGLEN(chr->actorBase, 0), 0.f), m::Vector3(0.f, -1.f, 0.f), vecup_inv);
		// Draw leg
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_leg), acv::GetT(acv::actor_templates[chr->actorBase].t_leg), SS_NORMAL, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();
		// transform legR for cloak
		//graphics::MatrixTransformForwardUp(matLegUpR, chr->t_body.GetPosition(), chr->fpCurrentR - chr->t_body.GetPosition(), chr->t_body.GetForward());

		// Leg left
		len = m::Length(jointPosL - chr->fpCurrentL);
		if (len > LEGLEN(chr->actorBase, 0)) len = LEGLEN(chr->actorBase, 0);
		lenUp = sqrtf(LEGLEN(chr->actorBase, 0) * LEGLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(chr->fpCurrentL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, (m::Normalize(chr->t_body.GetForward() + chr->t_body.GetRight() * -0.23f))));
		vecup = m::Normalize(m::Cross(vecfw, vecside));
		vecup_inv = vecup * -1.f;
		graphics::MatrixTransform(matLegHipL, jointPosL, graphics::MatrixGetForward(matBodyLo), graphics::MatrixGetUp(matBodyLo));
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegFootL, chr->fpCurrentL + m::Vector3(0.f, LEGLEN(chr->actorBase, 0), 0.f), m::Vector3(0.f, -1.f, 0.f), vecup_inv);
		// Draw leg
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_leg), acv::GetT(acv::actor_templates[chr->actorBase].t_leg), SS_NORMAL, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
		// transform legL for cloak
		//graphics::MatrixTransformForwardUp(matLegUpL, chr->t_body.GetPosition(), chr->fpCurrentL - chr->t_body.GetPosition(), chr->t_body.GetForward());

	}

	//-------------------------------- DRAW HEAD

	Transform3D t2;
	t2.SetPosition(chr->t_body.GetPosition());
	t2.SetRotation(t_upperbody.GetRotation());
	t2.TranslateLocal(m::Vector3(0.f, 0.7f, 0.f));
	//DrawMeshDeform(chr->id, acv::GetMD(acv::md_char_head), acv::GetT(chr->t_skin), SS_CHARA, 4u,
	//	t2.getMatrix(), t_head.getMatrix(), t_head.getMatrix(), t_head.getMatrix());
	DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_head), acv::GetT(acv::actor_templates[chr->actorBase].t_head), SS_CHARA, 4u,
		t2.getMatrix(), chr->t_head.getMatrix(), chr->t_head.getMatrix(), chr->t_head.getMatrix());

	//-------------------------------- DRAW DEBUG PATHING

	#if !DEF_GRID
	// debug draw path
	if (chr->ai_pathing) {
		for (int i = 0; i < chr->ai_path.len; ++i) {
			//CellSpace cs;
			//core::GetCellSpaceInfo();
			//env::GetHeight();
			graphics::Matrix4x4 mattt;
			graphics::MatrixTransform(mattt, m::Vector3(chr->ai_path.pos_x[chr->ai_path_current_index], chr->t.altitude, chr->ai_path.pos_y[chr->ai_path_current_index]));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_debug_bb), SS_NORMAL, mattt);
		}
	}
	#endif
}
