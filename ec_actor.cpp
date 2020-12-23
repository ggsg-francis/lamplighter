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
#define shoulder_width acv::actor_templates[0].jpos_arm_rt
#define hip_width acv::actor_templates[0].jpos_leg_rt

#define velocityStepMult (8.f * LEGLEN(chr->actorBase,0)) // How far to place our foot ahead when walking

// Multiplier for animation playback
#define STEP_LEN 0.45f

AnimFrames framesStand(2,
	AnimKeyframe(9.f, 11.f, 5.f),
	AnimKeyframe(11.f, 9.f, 5.f));

AnimFrames framesWalk(2,
	AnimKeyframe(0.f, 8.f, 1.f),
	AnimKeyframe(0.f, 8.f, 1.f));

AnimFrames framesRun(2,
	AnimKeyframe(22.f, 30.f, 1.5f),
	AnimKeyframe(22.f, 30.f, 1.5f));

AnimFrames framesJump(2,
	AnimKeyframe(31.f, 34.f, 0.45f),
	AnimKeyframe(34.f, 34.f, INFINITY));

AnimFrames framesDie(2,
	AnimKeyframe(12.f, 18.f, 0.7f),
	AnimKeyframe(18.f, 18.f, INFINITY));


void ActorOnHitGround(ECActor* chr)
{
	//if (chr->foot_state == ECActor::eL_DOWN) aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_A, chr->fpCurrentL);
	//else aud::PlaySnd3D(aud::FILE_FOOTSTEP_SNOW_B, chr->fpCurrentR);
	// Swap feet
	if (chr->foot_state == ECActor::eL_DOWN) chr->foot_state = ECActor::eR_DOWN;
	else if (chr->foot_state == ECActor::eR_DOWN) chr->foot_state = ECActor::eL_DOWN;
}

void ActorTryHoldHand(LtrID id_self, LtrID id_other)
{
	ECActor* self = ACTOR(id_self);
	ECActor* actr = ACTOR(id_other);

	lui32 id1 = self->inventory.GetFirstEmptySpace();
	lui32 id2 = actr->inventory.GetFirstEmptySpace();
	if (id1 != ID_NULL && id2 != ID_NULL) {
		self->inv_active_slot = id1;
		actr->inv_active_slot = id2;
		if (IDCOMPARE(self->aniHandHoldTarget, id_other))
			self->aniHandHoldTarget = ID2_NULL;
		else
			self->aniHandHoldTarget = id_other;
		if (IDCOMPARE(actr->aniHandHoldTarget, id_self))
			actr->aniHandHoldTarget = ID2_NULL;
		else
			actr->aniHandHoldTarget = id_self;
	}
}

void ActorTakeItem(LtrID id_self, LtrID id_item)
{
	ECActor* self = ACTOR(id_self);
	ECSingleItem* item = ITEM(id_item);
	HeldItem* item_held = GETITEMINST(item->item_instance);
	lui32 slot_added = self->inventory.TransferItemRecv(item->item_instance);
	core::DestroyEntity(id_item);
	if (slot_added == self->inv_active_slot)
		ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	if (IDCOMPARE(core::players[0], id_self)) {
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::ItemRecord*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(0, string);
	}
	else if (IDCOMPARE(core::players[1], id_self)) {
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::ItemRecord*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(1, string);
	}
}

void ActorDropItem(LtrID id_self, ID16 slot)
{
	ECActor* self = ACTOR(id_self);
	if (slot < self->inventory.items.Size() && self->inventory.items.Used(slot))
	{
		m::Vector2 throwDir = m::AngToVec2(self->viewYaw.Rad());
		LtrID item_entity = core::SpawnEntityItem(self->inventory.items[slot],
			self->t.position + (throwDir * (self->radius + acv::items[((HeldItem*)GetItemInstance(self->inventory.items[slot].Index()))->id_item_template]->f_radius)),
			self->t.altitude, self->viewYaw.Deg());
		ENTITY(item_entity)->velocity = throwDir * 0.05f + self->velocity;
		self->inventory.TransferItemSendIndex(slot);
		ActorDecrEquipSlot(id_self);
	}
}

void ActorDropAllItems(LtrID id_self)
{
	ECActor* self = ACTOR(id_self);
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

void ActorSetEquipSlot(ID16 id_self, lui32 index)
{
	ECActor* self = (ECActor*)GetEntityPtr(id_self);
	if (index < self->inventory.items.Size())
	{
		self->inv_active_slot = index;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorIncrEquipSlot(LtrID id_self)
{
	ECActor* self = ACTOR(id_self);
	if (self->inv_active_slot < self->inventory.items.Size() - 1u)
	{
		++self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorDecrEquipSlot(LtrID id_self)
{
	ECActor* self = ACTOR(id_self);
	if (self->inv_active_slot > 0u)
	{
		--self->inv_active_slot;
		if (self->inventory.items.Used(self->inv_active_slot))
			ItemOnEquip(self->inventory.items[self->inv_active_slot], self);
	}
}

void ActorTick(LtrID id, void* ent, lf32 dt)
{
	ECActor* chr = (ECActor*)ent;

	// temp
	chr->aniStandHeight = 0.25f;

	// Update view target (used to pick up items and stuff)
	chr->viewtarget = core::GetEntityViewTarget(id);

	chr->input.move = m::Normalize(chr->input.move);

	// modify input to keep handholding characters together
	if (GetEntityExists(chr->aniHandHoldTarget)) {
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

	// hold hands check if our target is bullshit (gone, or too far away, or has started holding an item
	if (!GetEntityExists(chr->aniHandHoldTarget) ||
		m::Length(ACTOR(chr->aniHandHoldTarget)->t.position - chr->t.position) > 1.4f ||
		ACTOR(chr->aniHandHoldTarget)->inventory.items.Used(ACTOR(chr->aniHandHoldTarget)->inv_active_slot) ||
		chr->inventory.items.Used(chr->inv_active_slot)) // or we started holding a thing
		chr->aniHandHoldTarget = ID2_NULL;
	// or if our target is ourself somehow
	if (IDCOMPARE(chr->aniHandHoldTarget, id))
		chr->aniHandHoldTarget = ID2_NULL;

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

	// hold hands override
	if (GetEntityExists(chr->aniHandHoldTarget)) {
	}

	// jump anim
	if (!chr->Grounded()) {
	}

	//chr->aniAnimPlayer.Tick(dt * 0.02f, &framesDie, &chr->aniAnimstate);

	///*
	if (chr->activeFlags.get(ECCommon::eALIVE)) {
		if (chr->Grounded()) {
			if (m::Length(chr->input.move) > 0.001f) {
				if (chr->aniRun)
					chr->aniAnimPlayer.Tick((0.25f / STEP_LEN) * velLen, &framesRun, &chr->aniAnimstate);
				else
					chr->aniAnimPlayer.Tick((0.25f / STEP_LEN) * velLen, &framesWalk, &chr->aniAnimstate);
			}
			else {
				chr->aniAnimPlayer.Tick(dt, &framesStand, &chr->aniAnimstate);
			}
		}
		else {
			chr->aniAnimPlayer.Tick(dt, &framesJump, &chr->aniAnimstate);

		}
	}
	else {
		chr->aniAnimPlayer.Tick(dt, &framesDie, &chr->aniAnimstate);
	}//*/

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

	if (chr->activeFlags.get(ECCommon::eALIVE)) {
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		// Set head transform
		/*chr->t_head.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude
			- (chr->aniSlideResponse * 0.25f) + BODYLEN * animBody.y, chr->t.position.y));*/
		chr->t_head.SetPosition(m::Vector3(chr->t.position.x, chr->t.altitude + 0.4f, chr->t.position.y));

		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_head.Rotate(chr->viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

		// Set body transform
		chr->t_body.SetPosition(m::Vector3(
			chr->t.position.x,
			chr->t.altitude,
			chr->t.position.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));

		// Head position distance check
		if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > BODYLEN)
			chr->t_head.SetPosition(m::Lerp(chr->t_head.GetPosition(), chr->t_body.GetPosition()
				+ m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * BODYLEN, 0.001f));

		//-------------------------------- RUN ITEM TICK (SHOULDNT REALLY BE HERE BUT HAX I GUESS)

		if (chr->inventory.items.Used(chr->inv_active_slot))
			ItemTick(chr->inventory.items[chr->inv_active_slot], dt, id, chr);
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

void ActorDraw(LtrID id, void* ent)
{
	ECActor* chr = (ECActor*)ent;

	// will hopefully be used as a root for all matrix calculations
	// god knows how long until that can happen though
	graphics::Matrix4x4 rootMatrix;
	{
		m::Angle angtest = chr->t.yaw;
		lf32 angdif = m::AngDifAbs(chr->t.yaw.Deg(), chr->viewYaw.Deg());
		angtest.RotateTowards(chr->viewYaw.Deg(), angdif * 0.5f);

		graphics::MatrixTransform(rootMatrix, m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y), chr->t.yaw.Rad());
		//graphics::MatrixTransform(rootMatrix, m::Vector3(chr->t.position.x, chr->t.altitude, chr->t.position.y), angtest.Rad());
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

	//DrawMeshSet(acv::GetMS(acv::ms_default), chr->aniAnimstate, acv::GetT(acv::t_default), SS_NORMAL, rootMatrix);
	DrawMeshSet(acv::GetMS(acv::ms_default), chr->aniAnimstate, acv::GetT(acv::t_default), SS_CHARA, rootMatrix);

	//-------------------------------- DRAW HELD ITEM

	if (chr->inventory.items.Used(chr->inv_active_slot)) {
		// Get held item
		HeldItem* heldItem = ((HeldItem*)GetItemInstance(chr->inventory.items[chr->inv_active_slot].Index()));
		// Draw held item
		ItemDraw(chr->inventory.items[chr->inv_active_slot]);
	}
	
	//-------------------------------- DRAW DEBUG PATHING

	#if !DEF_GRID
	// debug draw path
	//if (chr->ai_pathing) {
	//	for (int i = 0; i < chr->ai_path.len; ++i) {
	//		//CellSpace cs;
	//		//core::GetCellSpaceInfo();
	//		//env::GetHeight();
	//		graphics::Matrix4x4 mattt;
	//		graphics::MatrixTransform(mattt, m::Vector3(chr->ai_path.pos_x[chr->ai_path_current_index], chr->t.altitude, chr->ai_path.pos_y[chr->ai_path_current_index]));
	//		DrawMesh(acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_debug_bb), SS_NORMAL, mattt);
	//	}
	//}
	#endif
}
