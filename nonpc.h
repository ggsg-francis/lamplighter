#ifndef NPC_H
#define NPC_H

#include "core.h"
#include "index.h"
#include "entity.h"

lf32 ai_vy_target = 0.f;
lf32 ai_vp_target = 0.f;

void NPCFocusEnemy(ECActor* actor)
{
	m::Vector2 TargetVector = ENTITY(actor->ai_target_ent)->t.position - actor->t.position;
	m::Vector2 TargetVectorVertical = m::Vector2(m::Length(TargetVector), ENTITY(actor->ai_target_ent)->t.altitude - actor->t.altitude);
	float distance_to_target = m::Length(TargetVector);

	actor->input.bits.unset(IN_USE);

	actor->input.bits.set(IN_RUN);


	lf32 attack_dist = 1.5f;
	// if its a ranged weapon, set the attack range higher
	if (GetItemInstanceType(actor->inventory.items[actor->inv_active_slot].Index()) == ITEM_TYPE_WPN_MATCHGUN) {
		attack_dist = 30.f;
	}

	actor->input.move = m::Normalize(ENTITY(actor->ai_target_ent)->t.position - actor->t.position);

	if (distance_to_target < attack_dist) // if enemy is close enough to swing at
	{
		actor->input.move.x = 0.f;
		actor->input.move.y = 0.f;

		lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
		lf32 angle22 = -90.f + glm::degrees(m::Vec2ToAng(m::Normalize(TargetVectorVertical)));

		//lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(actor->t.position - ENTITY(actor->target_ent)->t.position)));

		//actor->viewYaw.Set(angle2);
		//actor->viewPitch.Set(angle22);
		//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
		//actor->viewPitch.RotateTowards(angle22, HEAD_TURN_SPEED);
		ai_vy_target = angle2;
		if (angle22 < -80.f) angle22 = -80.f;
		if (angle22 > 70.f) angle22 = 70.f;
		ai_vp_target = angle22;

		// this method staggers their shooting
		lf32 rand = m::Random(0.f, 100.f);
		// spam single shot guns
		actor->input.bits.setto(IN_USE_HIT, rand < 7.f);
		// automatic guns
		actor->input.bits.set(IN_USE);
	}
	else
	{
		//compute rotation
		//float offset = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), ENTITY(actor->target_ent)->t.position - actor->t.position);
		//float forwards = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg())), ENTITY(actor->target_ent)->t.position - actor->t.position);

		if (GetEntityExists(actor->ai_ally_ent))
		{
			m::Vector2 AllyVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
			float distance_to_ally = m::Length(AllyVector);
			float offsetLR_ally = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), AllyVector);

			const lf32 ally_follow_dist = 2.f;

			// if ally is farther than enemy
			if (distance_to_ally > distance_to_target || distance_to_ally < 4.f)
			{
				lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				ai_vy_target = angle2;
			}
			else
			{
				lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(AllyVector)));

				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				ai_vy_target = angle2;

			}
		}
		else
		{
			lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
			//lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(actor->t.position - ENTITY(actor->target_ent)->t.position)));

			//actor->viewYaw.Set(angle2);
			//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
			ai_vy_target = angle2;
		}
	}
}

void NPCFollowAlly(ECActor* actor)
{
	ECCommon* target = ENTITY(actor->ai_ally_ent);
	m::Vector2 TargetVector = target->t.position - actor->t.position;
	lf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
	float distance_to_target = m::Length(TargetVector);

	// crouch if targ is
	actor->aniCrouch = ACTOR(actor->ai_ally_ent)->aniCrouch;

	if (!actor->ai_pathing) { // if not pathing, see if we need to path
		actor->input.move.x = 0.f;
		actor->input.move.y = 0.f;
		if (distance_to_target > 2.f) { // if far enough away from the target, make a path
			if (path::PathFind(&actor->ai_path,
				actor->t.position.x, actor->t.position.y,
				target->t.position.x, target->t.position.y))
			{
				printf("Found new path!\n");
				actor->ai_pathing = true;
				actor->ai_path_current_index = actor->ai_path.len - 1;
			}
			// if we can't reach our target, just forget it
			else actor->ai_ally_ent = ID2_NULL;
		}
	}
	else { // follow path
		#if DEF_GRID
		// have we reached the end of the path?
		if (actor->ai_path_current_index == 0u)
		{
			// stop pathing
			actor->ai_pathing = false;
		}
		// otherwise follow the path
		else {
			TargetVector = m::Vector2(actor->ai_path.nodes[actor->ai_path_current_index]) - actor->t.position;
			angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
			distance_to_target = m::Length(TargetVector);
			if (distance_to_target < 0.25f)
				--actor->ai_path_current_index;
		}
		#else
		// look at our target
		TargetVector = m::Vector2(
			actor->ai_path.pos_x[actor->ai_path_current_index],
			actor->ai_path.pos_y[actor->ai_path_current_index])
			- actor->t.position;
		// TODO: only do this if the target is view blocked
		// (look the direction we're walking in)
		angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
		distance_to_target = m::Length(TargetVector);
		//if (distance_to_target < 0.125f) { // if we're basically touching the nav point
		if (distance_to_target < 0.05f) { // if we're basically touching the nav point
			// have we reached the end of the path?
			if (actor->ai_path_current_index == 0u)
				actor->ai_pathing = false;
			// Otherwise just decrement index
			else --actor->ai_path_current_index;
		}
		printf("Pathing, index: %i\n", actor->ai_path_current_index);
		#endif
		actor->input.move = TargetVector; // go to target, whatever it is
	}
	ai_vy_target = angle2; // look at thing

	actor->input.bits.unset(IN_USE);
	actor->input.bits.unset(IN_USE_HIT);
}

void NPCIdle(ECActor* actor)
{
	actor->input.move.y = 0.f;
	actor->input.move.x = 0.f;
	actor->input.bits.unset(IN_USE);
}

void NPCTick(LtrID id)
{
	ai_vp_target = 0.f;
	ai_vy_target = 0.f;
	
	ECActor* actor = ACTOR(id);

	actor->input.move.y = 0.f;
	actor->input.move.x = 0.f;

	actor->input.bits.unset(IN_RUN);

	// line of sight check to target has to be done every frame, if it exists
	// If is null OR deleted OR dead OR no LOS
	if (!GetEntityExists(actor->ai_target_ent) || // gone
		!ENTITY(actor->ai_target_ent)->activeFlags.get(ECCommon::eALIVE) || // dead
		!core::LOSCheck(id, actor->ai_target_ent)) { // cant see
		actor->ai_target_ent = ID2_NULL;
	}

	// Random whether to search for an ally or enemy this turn
	bool btest = m::Random(0.f, 1.f) > 0.75f; // Favour looking for enemies by 3/4

	if (btest) {
		// update ally:
		// If is null OR deleted OR dead
		// dont forget about ally when they go out of sightline
		if (!GetEntityExists(actor->ai_ally_ent) ||
			!ENTITY(actor->ai_ally_ent)->activeFlags.get(ECCommon::eALIVE)) {
			if (actor->ai_timer <= tickCount) {
				actor->ai_ally_ent = core::GetClosestEntityAllegLOS(id, 100.f, fac::allied); // Find the closest ally
				actor->ai_timer = tickCount + (lui64)m::Random(50, 100);
			}
			else {
				actor->ai_ally_ent = ID2_NULL;
			}
		}
	}
	else {
		// if no target
		if (!GetEntityExists(actor->ai_target_ent)) {
			// Check if it's time to try and find a new enemy
			if (actor->ai_timer <= tickCount) {
				actor->ai_target_ent = core::GetClosestEntityAllegLOS(id, 100.f, fac::enemy); // Find the closest enemy
				actor->atk_target = actor->ai_target_ent;
				if (GetEntityExists(actor->ai_target_ent)) // if we do find an enemy
					aud::PlaySnd3D(aud::FILE_TAUNT, m::Vector3(actor->t_head.GetPosition()));
				actor->ai_timer = tickCount + (lui64)m::Random(50, 100);
			}
			// Otherwise just set as null and we can retry later
			else {
				actor->ai_target_ent = ID2_NULL;
				actor->atk_target = actor->ai_target_ent;
			}
		}
	}
	

	// bad and temporary :P
	// makes npc only point its gun if its looking at an enemy
	if (GetItemInstanceType(actor->inventory.items[actor->inv_active_slot].Index()) == ITEM_TYPE_WPN_MATCHGUN) {
		actor->input.bits.setto(IN_ACTN_B, IDCOMPARE(actor->ai_target_ent, ID2_NULL));
		actor->input.bits.setto(IN_ACTN_A, !IDCOMPARE(actor->ai_target_ent, ID2_NULL));
	}

	if (!GetEntityExists(actor->ai_target_ent)) {
		if (!GetEntityExists(actor->ai_ally_ent)
			|| actor->faction != fac::player) // temp: just dont follow each other if we're not on the player side
			NPCIdle(actor);
		else // if we have an ally, follow it
			NPCFollowAlly(actor);
	}
	else // if we have a target
		NPCFocusEnemy(actor);

	actor->viewYaw.RotateTowards(ai_vy_target, HEAD_TURN_SPEED);
	actor->viewPitch.RotateTowards(ai_vp_target, HEAD_TURN_SPEED);
}

#endif
