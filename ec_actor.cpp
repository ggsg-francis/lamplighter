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


void ActorOnHitGround(ECActor* chr)
{
	if (chr->foot_state == ECActor::eL_DOWN) aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_A, chr->fpCurrentL);
	else aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_B, chr->fpCurrentR);
	// Swap feet
	if (chr->foot_state == ECActor::eL_DOWN) chr->foot_state = ECActor::eR_DOWN;
	else if (chr->foot_state == ECActor::eR_DOWN) chr->foot_state = ECActor::eL_DOWN;
}

void ActorTryHoldHand(btID id_self, btID id)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	ECActor* actr = (ECActor*)GetEntityPtr(id);

	btui32 id1 = self->inventory.GetFirstEmptySpace();
	btui32 id2 = actr->inventory.GetFirstEmptySpace();
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

void ActorTakeItem(btID id_self, btID id)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	ECSingleItem* item = (ECSingleItem*)GetEntityPtr(id);
	HeldItem* item_held = GETITEMINST(item->item_instance);
	btui32 slot_added = self->inventory.TransferItemRecv(item->item_instance);
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

void ActorDropItem(btID id_self, btID slot)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (slot < self->inventory.items.Size() && self->inventory.items.Used(slot))
	{
		m::Vector2 throwDir = m::AngToVec2(self->viewYaw.Rad());
		btID item_entity = core::SpawnEntityItem(self->inventory.items[slot],
			self->t.position + (throwDir * (self->radius + acv::items[((HeldItem*)GetItemInstance(self->inventory.items[slot]))->id_item_template]->f_radius)),
			self->t.altitude, self->viewYaw.Deg());
		ENTITY(item_entity)->velocity = throwDir * 0.05f + self->velocity;
		self->inventory.TransferItemSendIndex(slot);
		ActorDecrEquipSlot(id_self);
	}
}

void ActorDropAllItems(btID id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	for (btui32 slot = 0u; slot < INV_SIZE; ++slot)
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

void ActorSetEquipSlot(btID id_self, btui32 index)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (index < self->inventory.items.Size())
	{
		self->inv_active_slot = index;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorIncrEquipSlot(btID id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (self->inv_active_slot < self->inventory.items.Size() - 1u)
	{
		++self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorDecrEquipSlot(btID id_self)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (self->inv_active_slot > 0u)
	{
		--self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

m::Vector3 Actor_SetFootPos(m::Vector2 position, btf32 in_height)
{
	CellSpace cs;
	core::GetCellSpaceInfo(position, cs);
	btf32 height;
	#if DEF_GRID
	env::GetHeight(height, cs);
	#else
	env::GetNearestSurfaceHeight(height, cs, in_height);
	#endif
	return m::Vector3(position.x, height + 0.075f, position.y);
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

void Actor_AnimateLegs(ECActor* chr)
{
	m::Vector2 vecfw = m::AngToVec2(chr->t.yaw.Rad());
	m::Vector2 vecrt = m::Vector2(-vecfw.y, vecfw.x);

	m::Vector3 tempFPL(0.f);
	m::Vector3 tempFPR(0.f);

	// If on the ground
	if (chr->grounded) {
		chr->lastGroundFootPos = Actor_SetFootPos(chr->t.position, chr->t.altitude);
		// If we're standing still, play idle anim
		if (m::Length(chr->velocity) < 0.005f) {
			if (chr->foot_state == ECActor::eL_DOWN) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.15f) + (vecfw * -0.15f), chr->t.altitude);
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.15f) + (vecfw * 0.15f), chr->t.altitude);
				chr->aniTimer = 0.5f;
			}
			if (chr->foot_state == ECActor::eR_DOWN) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.15f) + (vecfw * 0.15f), chr->t.altitude);
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.15f) + (vecfw * -0.15f), chr->t.altitude);
				chr->aniTimer = 1.5f;
			}
		}
		// If walking, play walking anim (crazy I know)
		else {
			#define STEP_LEN 0.35f
			// Generate offset timer
			btf32 aniTimerNew = chr->aniTimer + (0.5f / STEP_LEN) * m::Length(chr->velocity);
			// Check for footsound time (only if not sneaking)
			if (!chr->aniCrouch) {
				if (chr->aniTimer < 1.f && aniTimerNew > 1.f)
					aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_A, chr->fpCurrentL);
				else if (chr->aniTimer < 2.f && aniTimerNew > 2.f)
					aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_B, chr->fpCurrentR);
			}
			// Apply change to timer
			chr->aniTimer = aniTimerNew;
			// Timer rollover
			if (chr->aniTimer > 2.f) chr->aniTimer -= 2.f;
			// Create foot target positions
			m::Vector2 voNorm = m::Normalize(chr->velocity);
			if (chr->aniTimer < 1.f) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.05f) + (voNorm * m::Lerp(-STEP_LEN, STEP_LEN, chr->aniTimer)), chr->t.altitude);
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.05f) + (voNorm * m::Lerp(STEP_LEN, -STEP_LEN, chr->aniTimer)), chr->t.altitude);
				tempFPL.y += m::QuadraticFootstep(0.3f, (chr->aniTimer - 0.5f) * 2.f);
				chr->foot_state = ECActor::eR_DOWN;
			}
			else {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.05f) + (voNorm * m::Lerp(STEP_LEN, -STEP_LEN, chr->aniTimer - 1.f)), chr->t.altitude);
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.05f) + (voNorm * m::Lerp(-STEP_LEN, STEP_LEN, chr->aniTimer - 1.f)), chr->t.altitude);
				tempFPR.y += m::QuadraticFootstep(0.3f, (chr->aniTimer - 1.5f) * 2.f);
				chr->foot_state = ECActor::eL_DOWN;
			}
			#undef STEP_LEN
		}
	}
	// If in the air (can be due to jumping, falling, or running)
	else {
		// Set left foot
		if (chr->foot_state == ECActor::eL_DOWN) {
			if (chr->altitude_velocity > 0.f)
				tempFPR = chr->lastGroundFootPos;
			else
				tempFPR = chr->t_body.GetPosition() + m::Vector3(
					chr->velocity.x * -velocityStepMult * 0.5f,
					LEGLEN(chr->actorBase, 0) * -0.4f,
					chr->velocity.y * -velocityStepMult * 0.5f);
			tempFPL = chr->t_body.GetPosition() + m::Vector3(
				chr->velocity.x * -velocityStepMult * -0.5f,
				LEGLEN(chr->actorBase, 0) * -0.65f,
				chr->velocity.y * -velocityStepMult * -0.5f);
			chr->aniTimer = 0.f;
		}
		// Set right foot
		else {
			if (chr->altitude_velocity > 0.f)
				tempFPL = chr->lastGroundFootPos;
			else
				tempFPL = chr->t_body.GetPosition() + m::Vector3(
					chr->velocity.x * -velocityStepMult * 0.5f,
					LEGLEN(chr->actorBase, 0) * -0.4f,
					chr->velocity.y * -velocityStepMult * 0.5f);
			tempFPR = chr->t_body.GetPosition() + m::Vector3(
				chr->velocity.x * -velocityStepMult * -0.5f,
				LEGLEN(chr->actorBase, 0) * -0.65f,
				chr->velocity.y * -velocityStepMult * -0.5f);
			chr->aniTimer = 1.f;
		}
	}

	// Apply new positions (interpolated)
	chr->fpCurrentL = m::Lerp(chr->fpCurrentL, tempFPL, 0.5f);
	chr->fpCurrentR = m::Lerp(chr->fpCurrentR, tempFPR, 0.5f);

	// Clamp positions so they never exceed the length of a leg in distance
	Actor_ClampLegs(chr);
}

void ActorTick(btID id, void* ent, btf32 dt)
{
	ECActor* chr = (ECActor*)ent;

	// Update view target (used to pick up items and stuff)
	chr->viewtarget = core::GetEntityViewTarget(id);

	chr->input = m::Normalize(chr->input);

	// modify input to keep holding characters together
	if (chr->aniHandHoldTarget != ID_NULL) {
		ECActor* actor = ACTOR(chr->aniHandHoldTarget);
		m::Vector2 destpos = actor->t.position;
		//if (chr->aniHandHoldTarget > id)
		//	destpos = actor->t.position + (m::AngToVec2(chr->t.yaw.Rad() + glm::radians(90.f) * 0.75f));
		//else
		//	destpos = actor->t.position + (m::AngToVec2(chr->t.yaw.Rad() - glm::radians(90.f) * 0.75f));
		if (m::Length(destpos - chr->t.position) > 0.85f) {
			chr->input += (destpos - chr->t.position);
		}
	}

	// apply crouch input
	if (chr->inputBV.get(ECActor::IN_CROUCH))
		chr->aniCrouch = !chr->aniCrouch;
	chr->aniRun = chr->inputBV.get(ECActor::IN_RUN);

	if (chr->aniCrouch)
		chr->aniStandHeight = m::Lerp(chr->aniStandHeight, LEGLEN(chr->actorBase, 0) * 0.4f, 0.4f);
	else
		chr->aniStandHeight = m::Lerp(chr->aniStandHeight, LEGLEN(chr->actorBase, 0) * 0.9f, 0.4f);

	btf32 slide = (m::Length(chr->slideVelocity) / dt) * 0.5f;
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

	switch (acv::props[env::eCells.prop[chr->t.csi.c[eCELL_I].x][chr->t.csi.c[eCELL_I].y]].floorType) {
	case acv::PropRecord::FLOOR_QUICKSAND:
		canJump = false;
		break;
	}

	// Jump
	if (canJump) {
		if (chr->inputBV.get(ECActor::IN_JUMP)) {
			if (chr->grounded) {
				// Enter jump
				if (chr->aniCrouch) // jump higher out of a crouch position
					chr->altitude_velocity = 0.135f;
				else
					chr->altitude_velocity = 0.1f;
				chr->velocity = chr->input * dt * chr->speed + chr->slideVelocity;
				chr->aniCrouch = false;
				chr->jump_state = ECActor::eJUMP_JUMP;
			}
			#if DEF_LONGJUMP // enable to turn on hover jumping
			else if (chr->jump_state == ECActor::eJUMP_JUMP)
				chr->t.height_velocity += 0.005f; // hover
			#endif
			else if (chr->jump_state == ECActor::eJUMP_SPRINT)
				chr->altitude_velocity -= 0.01f; // anti-hover
		}
		// Sprint
		else if (chr->grounded && chr->aniRun && m::Length(chr->velocity) > 0.5f * dt) {
			// Enter sprint jump
			chr->altitude_velocity = 0.038f; // enter jump
			chr->velocity = chr->input * dt * (chr->speed * 1.1f) + (chr->slideVelocity * 0.9f);
			chr->aniCrouch = false;
			chr->jump_state = ECActor::eJUMP_SPRINT;
		}
	}

	bool can_move = true;
	bool can_turn = true;
	if (chr->inventory.items.Used(chr->inv_active_slot))
	{
		can_move = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
		can_turn = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
	}

	if (chr->state.stateFlags.get(ActiveState::eDIED_REPORT))
	{
		ActorDropAllItems(id);
		chr->state.stateFlags.unset(ActiveState::eDIED_REPORT);
	}
	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		if (chr->grounded)
		{
			if (can_move)
			{
				if (!chr->aniCrouch)
					chr->velocity = m::Lerp(chr->velocity, chr->input * dt * chr->speed, 0.3f);
				else
					chr->velocity = m::Lerp(chr->velocity, chr->input * dt * chr->speed * 0.5f, 0.3f);
			}
			else
			{
				chr->input = m::Vector2(0.f, 0.f);
				chr->velocity = m::Lerp(chr->velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
		}
		if (can_turn)
		{
			btf32 angdif = m::AngDifAbs(chr->t.yaw.Deg(), chr->viewYaw.Deg());
			if (angdif > 85.f)
				if (angdif - 85.f < 8.f)
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), angdif - 85.f); // Rotate body towards the target direction
				else
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), 8.f); // Rotate body towards the target direction
			else if (m::Length(chr->input) > 0.2f)
			{
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

		//-------------------------------- RUN ITEM TICK

		if (chr->inventory.items.Used(chr->inv_active_slot))
			ItemTick(chr->inventory.items[chr->inv_active_slot], dt, id, chr);
	} // End if alive

	  //________________________________________________________________
	  //------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		#define WALK_HEIGHT_OFFSET_MULT 0.2f

		btf32 walk_height_offset;
		if (chr->aniTimer <= 1.f)
			walk_height_offset = m::QuadraticFootstep(WALK_HEIGHT_OFFSET_MULT, (chr->aniTimer - 0.5f) * 2.f);
		else
			walk_height_offset = m::QuadraticFootstep(WALK_HEIGHT_OFFSET_MULT, (chr->aniTimer - 1.5f) * 2.f);

		// Set head transform
		if (!chr->aniCrouch)
			chr->t_head.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude - (chr->aniSlideResponse * 0.25f) + BODYLEN * 0.985f + walk_height_offset, chr->t.position.y));
		else
			chr->t_head.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude - (chr->aniSlideResponse * 0.25f) + BODYLEN * 0.85f + walk_height_offset, chr->t.position.y));

		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_head.Rotate(chr->viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

		if (m::Length(chr->ani_body_lean - chr->t.position) < 1.f)
			chr->ani_body_lean = m::Lerp(chr->ani_body_lean, chr->t.position, 0.3f);
		else
			chr->ani_body_lean = chr->t.position;

		// Set body transform
		//t_body.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height + 0.6f - (m::Length(chr->slideVelocity) * 0.5f), chr->t.position.y));
		chr->t_body.SetPosition(m::Vector3(chr->ani_body_lean.x, chr->t.altitude - (chr->aniSlideResponse * 0.125f) + walk_height_offset, chr->ani_body_lean.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));


		// Head position distance check
		//if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > bodylen)
		//	chr->t_head.SetPosition(chr->t_body.GetPosition() + m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * bodylen);
		if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > BODYLEN)
			chr->t_head.SetPosition(m::Lerp(chr->t_head.GetPosition(), chr->t_body.GetPosition() + m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * BODYLEN, 0.001f));

		//-------------------------------- HANDLE ANIMATION

		Actor_AnimateLegs(chr);
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

void ActorDraw(btID id, void* ent)
{
	ECActor* chr = (ECActor*)ent;

	graphics::Matrix4x4 matLegHipR, matLegUpR, matLegLoR, matLegFootR;
	graphics::Matrix4x4 matLegHipL, matLegUpL, matLegLoL, matLegFootL;
	graphics::Matrix4x4 matBodyUp, matBodyLo;

	Transform3D t_upperbody = chr->t_body;
	t_upperbody.Rotate(glm::radians(m::AngDif(chr->viewYaw.Deg(), chr->t.yaw.Deg())), m::Vector3(0, 1, 0));

	btf32 lerpAmt = 0.05f * chr->speed;

	//-------------------------------- SET ACTOR SHADER PARAMETERS

	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).Use();
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_A, chr->skin_col_a.x, chr->skin_col_a.y, chr->skin_col_a.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_B, chr->skin_col_b.x, chr->skin_col_b.y, chr->skin_col_b.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(
		graphics::Shader::Colour_C, chr->skin_col_c.x, chr->skin_col_c.y, chr->skin_col_c.z);

	//-------------------------------- DRAW BODY

	btf32 len = m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition());
	if (len > BODYLEN) len = BODYLEN;
	btf32 lenUp = sqrtf(BODYLEN * BODYLEN - len * len); // Pythagorean theorem
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

	//if (chr->state.stateFlags.get(ActiveState::eALIVE))
	//{
	//	DrawMeshDeform(chr->id, acv::GetMD(acv::md_equip_body_robe_01), acv::GetT(acv::t_equip_body_robe_01), SS_NORMAL, 4u,
	//		matBodyLo, matBodyUp, matLegUpL, matLegUpR);
	//}

	//-------------------------------- DRAW ARMS

	graphics::Matrix4x4 matrix;

	//m::Vector3 newpos2 = chr->t_body.GetPosition() + chr->t_body.GetUp() * len;
	m::Vector3 newpos2 = graphics::MatrixGetPosition(matBodyUp) - graphics::MatrixGetForward(matBodyUp) * (BODYLEN - 0.1f);

	btf32 shoulder_width = 0.12f;

	m::Vector3 jointPosR = newpos2 + graphics::MatrixGetRight(matBodyUp) * shoulder_width;
	m::Vector3 jointPosL = newpos2 + graphics::MatrixGetRight(matBodyUp) * -shoulder_width;

	//-------------------------------- DRAW ARMS

	if (chr->inventory.items.Used(chr->inv_active_slot))
	{
		HeldItem* heldItem = ((HeldItem*)GetItemInstance(chr->inventory.items[chr->inv_active_slot]));

		// Draw held item
		ItemDraw(chr->inventory.items[chr->inv_active_slot],
			GETITEMINST(chr->inventory.items[chr->inv_active_slot])->id_item_template,
			chr->t.position, chr->t.altitude + 0.3f, chr->viewYaw, chr->viewPitch);

		// Get hand positions
		m::Vector3 handPosR = ItemRHPos(chr->inventory.items[chr->inv_active_slot]);
		m::Vector3 handPosL = ItemLHPos(chr->inventory.items[chr->inv_active_slot]);

		// Arm orientation vectors
		//only used once but the calculation is so long it's easier to read here
		#define ELBOW_DIR_R m::Normalize(m::Cross(m::Normalize(chr->t_body.GetPosition() - handPosR), t_upperbody.GetUp() * -1.f) - t_upperbody.GetRight() * 0.4f - m::Vector3(0.f, -1.f, 0.f))
		#define ELBOW_DIR_L m::Normalize(m::Cross(m::Normalize(chr->t_body.GetPosition() - handPosL), t_upperbody.GetUp())        + t_upperbody.GetRight() * 0.4f - m::Vector3(0.f, -1.f, 0.f))

		// Arm right
		len = m::Length(jointPosR - handPosR);
		if (len > ARMLEN(chr->actorBase, 0)) len = ARMLEN(chr->actorBase, 0);
		lenUp = sqrtf(ARMLEN(chr->actorBase, 0) * ARMLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, ELBOW_DIR_R));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside)) * -1.f;
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (ARMLEN(chr->actorBase, 0) - len), vecfw, vecup);
		// Draw arm
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();

		// Arm left
		len = m::Length(jointPosL - handPosL);
		if (len > ARMLEN(chr->actorBase, 0)) len = ARMLEN(chr->actorBase, 0);
		lenUp = sqrtf(ARMLEN(chr->actorBase, 0) * ARMLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, ELBOW_DIR_L));
		vecup = m::Normalize(m::Cross(vecfw, vecside) * -1.f);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (ARMLEN(chr->actorBase, 0) - len), vecfw, vecup);
		// Draw arm
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpL, matLegLoL, matLegFootL);

		#undef ELBOW_DIR_R 
		#undef ELBOW_DIR_L
	}
	else
	{
		m::Vector2 fpoffs1 = chr->t.position - m::Vector2(chr->fpCurrentL.x, chr->fpCurrentL.z);
		m::Vector2 fpoffs2 = chr->t.position - m::Vector2(chr->fpCurrentR.x, chr->fpCurrentR.z);

		m::Vector3 armoffsL;
		m::Vector3 armoffsR;
		if (!chr->aniCrouch)
		{
			armoffsL = m::Normalize(m::Vector3(fpoffs1.x, -0.9f, fpoffs1.y) - graphics::MatrixGetRight(matBodyUp) * 0.25f) * ARMLEN(chr->actorBase, 0) * 0.98f;
			armoffsR = m::Normalize(m::Vector3(fpoffs2.x, -0.9f, fpoffs2.y) + graphics::MatrixGetRight(matBodyUp) * 0.25f) * ARMLEN(chr->actorBase, 0) * 0.98f;
		}
		else
		{
			armoffsL = m::Normalize(m::Vector3(fpoffs1.x, -0.9f, fpoffs1.y) - graphics::MatrixGetRight(matBodyUp) * 0.6f) * ARMLEN(chr->actorBase, 0) * 0.75f;
			armoffsR = m::Normalize(m::Vector3(fpoffs2.x, -0.9f, fpoffs2.y) + graphics::MatrixGetRight(matBodyUp) * 0.6f) * ARMLEN(chr->actorBase, 0) * 0.75f;
		}

		// Make hand positions
		m::Vector3 handPosR = jointPosR + armoffsR;
		m::Vector3 handPosL = jointPosL + armoffsL;

		// hold hands check
		if (chr->aniHandHoldTarget != ID_NULL)
		{
			if (m::Dot(chr->t.position - ACTOR(chr->aniHandHoldTarget)->t.position, m::AngToVec2(chr->t.yaw.Rad() - glm::degrees(90.f))) > 0.f)
				handPosL = (ACTOR(chr->aniHandHoldTarget)->t_body.GetPosition() + chr->t_body.GetPosition()) * 0.5f;
			else
				handPosR = (ACTOR(chr->aniHandHoldTarget)->t_body.GetPosition() + chr->t_body.GetPosition()) * 0.5f;
		}

		// Arm orientation vectors
		#define ELBOW_DIR_R m::Normalize(graphics::MatrixGetRight(matBodyUp) * -0.25f + graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)
		#define ELBOW_DIR_L m::Normalize(graphics::MatrixGetRight(matBodyUp) *  0.25f + graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)

		#define handlentemp 0.1f

		// Arm right
		len = m::Length(jointPosR - handPosR) - handlentemp;
		if (len > ARMLEN(chr->actorBase, 0)) len = ARMLEN(chr->actorBase, 0);
		lenUp = sqrtf(ARMLEN(chr->actorBase, 0) * ARMLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, ELBOW_DIR_R));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside)) * -1.f;
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (ARMLEN(chr->actorBase, 0) - len), vecfw, vecup);
		// Draw arm
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();

		// Arm left
		len = m::Length(jointPosL - handPosL) - handlentemp;
		if (len > ARMLEN(chr->actorBase, 0)) len = ARMLEN(chr->actorBase, 0);
		lenUp = sqrtf(ARMLEN(chr->actorBase, 0) * ARMLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, ELBOW_DIR_L));
		vecup = m::Normalize(m::Cross(vecfw, vecside) * -1.f);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (ARMLEN(chr->actorBase, 0) - len), vecfw, vecup);
		// Draw arm
		DrawMeshDeform(id, acv::GetMD(acv::actor_templates[chr->actorBase].m_arm), acv::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_NORMAL, 4u, matBodyUp, matLegUpL, matLegLoL, matLegFootL);

		#undef ELBOW_DIR_R 
		#undef ELBOW_DIR_L

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

	//-------------------------------- DRAW LEGS

	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		btf32 velocityAmt = m::Length(chr->velocity - chr->slideVelocity);

		m::Vector3 newpos = chr->t_body.GetPosition();

		jointPosR = newpos + graphics::MatrixGetRight(matBodyLo) * hip_width;
		jointPosL = newpos + graphics::MatrixGetRight(matBodyLo) * -hip_width;

		// generate matrices

		// Leg right
		btf32 len = m::Length(jointPosR - chr->fpCurrentR);
		if (len > LEGLEN(chr->actorBase, 0)) len = LEGLEN(chr->actorBase, 0);
		btf32 lenUp = sqrtf(LEGLEN(chr->actorBase, 0) * LEGLEN(chr->actorBase, 0) - len * len); // Pythagorean theorem
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
}
