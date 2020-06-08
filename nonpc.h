#ifndef NPC_H
#define NPC_H

#include "core.h"
#include "index.h"
#include "objects_entities.h"

void NPCFocusEnemy(Actor* actor)
{
	m::Vector2 TargetVector = ENTITY(actor->ai_target_ent)->t.position - actor->t.position;
	m::Vector2 TargetVectorVertical = m::Vector2(m::Length(TargetVector), ENTITY(actor->ai_target_ent)->t.height - actor->t.height);
	float distance_to_target = m::Length(TargetVector);

	actor->inputBV.unset(Actor::IN_USE);

	btf32 attack_dist = 1.5f;
	// if its a ranged weapon, set the attack range higher
	if (GetItemType(actor->inventory.items[actor->inv_active_slot]) == ITEM_TYPE_WPN_MATCHGUN)
	{
		attack_dist = 30.f;
	}

	if (distance_to_target < attack_dist) // if enemy is close enough to swing at
	{
		//compute rotation
		//float offset = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), ENTITY(actor->target_ent)->t.position - actor->t.position);
		//float forwards = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg())), ENTITY(actor->target_ent)->t.position - actor->t.position);

		actor->input.y = 0.f;

		if (actor->ai_ally_ent != BUF_NULL)
		{
			m::Vector2 AllyVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
			float distance_to_ally = m::Length(AllyVector);
			float offsetLR_ally = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), AllyVector);

			if (distance_to_ally < 10.f) // if ally is close, spread
				if (offsetLR_ally > 0.5f) actor->input.x = -1.f;
				else if (offsetLR_ally < -0.5f) actor->input.x = 1.f;
				else actor->input.x = 0.f;
			else if (actor->state.damagestate > 600u) // if ally is far and hitpoints high, move in
				if (offsetLR_ally > 0.5f) { actor->input.x = -0.5f; actor->input.y = 1.f; }
				else if (offsetLR_ally < -0.5f) { actor->input.x = 0.5f; actor->input.y = 1.f; }
				else actor->input.x = 1.f;
			else // if low on hp, retreat
				if (offsetLR_ally > 0.5f) { actor->input.x = 1.f; actor->input.y = -1.f; }
				else if (offsetLR_ally < -0.5f) { actor->input.x = -1.f; actor->input.y = -1.f; }
				else actor->input.x = -1.f;
		}
		else
		{
			actor->input.x = 0.f;
		}

		btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
		btf32 angle22 = -90.f + glm::degrees(m::Vec2ToAng(m::Normalize(TargetVectorVertical)));

		//btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(actor->t.position - ENTITY(actor->target_ent)->t.position)));

		//actor->viewYaw.Set(angle2);
		//actor->viewPitch.Set(angle22);
		//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
		//actor->viewPitch.RotateTowards(angle22, HEAD_TURN_SPEED);
		actor->ai_vy_target = angle2;
		if (angle22 < -80.f) angle22 = -80.f;
		if (angle22 > 70.f) angle22 = 70.f;
		actor->ai_vp_target = angle22;

		//actor->input.y = 0.f; actor->input.x = 1.f;

		actor->inputBV.set(Actor::IN_USE);
	}
	else
	{
		//compute rotation
		//float offset = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), ENTITY(actor->target_ent)->t.position - actor->t.position);
		//float forwards = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg())), ENTITY(actor->target_ent)->t.position - actor->t.position);

		if (actor->ai_ally_ent != BUF_NULL)
		{
			m::Vector2 AllyVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
			float distance_to_ally = m::Length(AllyVector);
			float offsetLR_ally = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), AllyVector);

			const btf32 ally_follow_dist = 2.f;

			// if ally is farther than enemy
			if (distance_to_ally > distance_to_target || distance_to_ally < 4.f)
			{
				if (distance_to_ally > ally_follow_dist)
					if (offsetLR_ally > 0.5f) actor->input.x = 1.f;
					else if (offsetLR_ally < -0.5f) actor->input.x = -1.f;
					else actor->input.x = 0.f;
				else actor->input.x = 0.f;

				btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				actor->ai_vy_target = angle2;

				actor->input.y = 1.f;
			}
			else
			{
				if (distance_to_ally > ally_follow_dist) actor->input.y = 1.f;
				else actor->input.y = 0.f;
				actor->input.x = 0.f;

				btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(AllyVector)));

				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				actor->ai_vy_target = angle2;

			}
		}
		else
		{
			btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
			//btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(actor->t.position - ENTITY(actor->target_ent)->t.position)));

			//actor->viewYaw.Set(angle2);
			//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
			actor->ai_vy_target = angle2;

			actor->input.y = 1.f;// actor->input.x = 0.f;

			actor->input.x = 0.f;
		}
	}
}

void NPCFollowAlly(Actor* actor)
{
	m::Vector2 TargetVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
	btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
	float distance_to_target = m::Length(TargetVector);

	//actor->viewYaw.Set(angle2);
	//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
	actor->ai_vy_target = angle2;

	actor->inputBV.setto(Actor::ActorInput::IN_RUN, distance_to_target > 5.f);

	if (distance_to_target > 1.5f) // if ally is far away
		actor->input = TargetVector;
	else {
		actor->input.x = 0.f;
		actor->input.y = 0.f;
	}
	actor->inputBV.unset(Actor::IN_USE);
}

void NPCIdle(Actor* actor)
{
	actor->input.y = 0.f;
	actor->input.x = 0.f;
	actor->inputBV.unset(Actor::IN_USE);
}

void NPCTick(btID id)
{
	Actor* actor = ACTOR(id);

	actor->input.y = 1.f;
	actor->input.x = -1.f;

	//updatetarg:
	// If is null OR deleted OR dead OR no LOS
	if (actor->ai_target_ent == BUF_NULL
		|| !block_entity.used[actor->ai_target_ent]
		|| !ENTITY(actor->ai_target_ent)->state.stateFlags.get(ActiveState::eALIVE)
		|| !core::LOSCheck(id, actor->ai_target_ent))
	{
		actor->ai_target_ent = core::GetClosestEntityAllegLOS(id, 100.f, fac::enemy); // Find the closest enemy
		actor->atk_target = actor->ai_target_ent;
	}
	//updateally:
	// If is null OR deleted OR dead
	if (actor->ai_ally_ent == BUF_NULL
		|| !block_entity.used[actor->ai_ally_ent]
		|| !ENTITY(actor->ai_ally_ent)->state.stateFlags.get(ActiveState::eALIVE)
		|| !core::LOSCheck(id, actor->ai_ally_ent))
		actor->ai_ally_ent = core::GetClosestEntityAllegLOS(id, 100.f, fac::allied); // Find the closest ally

	// bad and temporary :P
	// makes npc only point its gun if its looking at an enemy
	if (GetItemType(actor->inventory.items[actor->inv_active_slot]) == ITEM_TYPE_WPN_MATCHGUN)
	{
		actor->inputBV.setto(Actor::ActorInput::IN_ACTN_B, actor->ai_target_ent == BUF_NULL);
		actor->inputBV.setto(Actor::ActorInput::IN_ACTN_A, actor->ai_target_ent != BUF_NULL);
	}

	if (actor->ai_target_ent == BUF_NULL)
	{
		if (actor->ai_ally_ent == BUF_NULL)
			NPCIdle(actor);
		else // if we have an ally, follow it
			NPCFollowAlly(actor);
	}
	else // if we have a target
		NPCFocusEnemy(actor);

	actor->viewYaw.RotateTowards(actor->ai_vy_target, HEAD_TURN_SPEED);
	actor->viewPitch.RotateTowards(actor->ai_vp_target, HEAD_TURN_SPEED);
}

#endif
