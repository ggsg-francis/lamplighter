// Included in index.cpp

#include "core_decl.hpp"

namespace index
{
	//________________________________________________________________________________________________________________________________
	// GENERAL FUNCTIONS -------------------------------------------------------------------------------------------------------------

	#define HEAD_TURN_SPEED 8.f

	void ActorRunAI(btID id)
	{
		Actor* actor = ACTOR(id);

		actor->input.y = 1.f;
		actor->input.x = -1.f;

	updatetarg:
		// If is null OR deleted OR dead OR no LOS
		if (actor->ai_target_ent == BUF_NULL 
			|| !block_entity.used[actor->ai_target_ent]
			|| !ENTITY(actor->ai_target_ent)->state.stateFlags.get(ActiveState::eALIVE)
			|| !LOSCheck(id, actor->ai_target_ent))
		{
			actor->ai_target_ent = GetClosestEntityAllegLOS(id, 100.f, fac::enemy); // Find the closest enemy
			actor->atk_target = actor->ai_target_ent;
		}
	updateally:
		// If is null OR deleted OR dead
		if (actor->ai_ally_ent == BUF_NULL
			|| !block_entity.used[actor->ai_ally_ent]
			|| !ENTITY(actor->ai_ally_ent)->state.stateFlags.get(ActiveState::eALIVE)
			|| !LOSCheck(id, actor->ai_ally_ent))
			actor->ai_ally_ent = GetClosestEntityAllegLOS(id, 100.f, fac::allied); // Find the closest ally

		// bad and temporary :P
		// makes npc not point the gun at allies
		if (GetItemType(actor->inventory.items[actor->inv_active_slot]) == ITEM_TYPE_WPN_MATCHGUN)
		{
			actor->inputBV.setto(Actor::ActorInput::IN_ACTN_B, actor->ai_target_ent == BUF_NULL);
			actor->inputBV.setto(Actor::ActorInput::IN_ACTN_A, actor->ai_target_ent != BUF_NULL);
		}
		
		if (actor->ai_target_ent == BUF_NULL)
		{
			if (actor->ai_ally_ent == BUF_NULL)
			{
				actor->input.y = 0.f;
				actor->input.x = 0.f;
				actor->inputBV.unset(Actor::IN_USE);
			}
			else // if we have an ally, follow it
			{
				// ALLY FOLLOW!!!!!!!!!!!!!!!!!!

				m::Vector2 TargetVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
				btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
				float distance_to_target = m::Length(TargetVector);

				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				actor->ai_vy_target = angle2;

				if (distance_to_target > 1.5f) // if ally is far away
					actor->input.y = 1.f;
				else
					actor->input.y = 0.f;
				actor->input.x = 0.f;
				actor->inputBV.unset(Actor::IN_USE);
			}
		}
		// if we have a target
		else
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

		actor->viewYaw.RotateTowards(actor->ai_vy_target, HEAD_TURN_SPEED);
		actor->viewPitch.RotateTowards(actor->ai_vp_target, HEAD_TURN_SPEED);
	}

	void EntDeintersect(Entity* ent, CellSpace& csi)
	{
		btf32 offsetx, offsety;
		bool overlapN, overlapS, overlapE, overlapW, touchNS = false, touchEW = false;

		//-------------------------------- ACTOR COLLISION CHECK

		if (ent->properties.get(Entity::eCOLLIDE_ENT))
		{
			for (btcoord x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; ++x)
			{
				for (btcoord y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; ++y)
				{
					//de-intersect entities
					for (int e = 0; e <= refCells[x][y].ref_ents.end(); e++)
					{
						if (refCells[x][y].ref_ents[e] != ID_NULL)
						{
							if (block_entity.used[refCells[x][y].ref_ents[e]] && ENTITY(refCells[x][y].ref_ents[e])->properties.get(Entity::eCOLLIDE_ENT))
							{
								m::Vector2 vec = ent->t.position - ENTITY(refCells[x][y].ref_ents[e])->t.position;
								float dist = m::Length(vec);
								btf32 combined_radius = ent->radius + ENTITY(refCells[x][y].ref_ents[e])->radius;
								if (dist < combined_radius && dist > 0.f)
								{
									// TEMP! if same type
									if (ent->type == ENTITY(refCells[x][y].ref_ents[e])->type)
									{
										ent->t.position += m::Normalize(vec) * (combined_radius - dist) * 0.5f;
										ENTITY(refCells[x][y].ref_ents[e])->t.position -= m::Normalize(vec) * (combined_radius - dist) * 0.5f;
									}

									// consider using some kind of collision callback

									/*
									// knockback effect
									m::Vector2 surface = m::Normalize(vec * -1.f);
									if (ent->type == ENTITY_TYPE_CHARA && m::Length(ent->t.velocity) > 0.02f)
									{
										// if other is also a character
										if (ENTITY(cells[x][y].ents[e])->type == ENTITY_TYPE_CHARA)
										{
											ent->t.velocity = surface * -0.1f; // set my velocity
											ENTITY(cells[x][y].ents[e])->t.velocity = surface * 0.3f; // set their velocity
										}
									}
									*/
								}
							}
						} // End for each entity in cell
					} // End if entity count of this cell is bigger than zero
				} // End for each cell group Y
			} // End for each cell group X
		} // end does collide entities check

		///*
		//-------------------------------- ENVIRONMENTAL COLLISION CHECK (2ND THEREFORE PRIORITIZED)

		offsetx = ent->t.position.x - ent->t.csi.c[eCELL_I].x;
		offsety = ent->t.position.y - ent->t.csi.c[eCELL_I].y;

		overlapN = offsety > 0;
		overlapS = offsety < 0;
		overlapE = offsetx > 0;
		overlapW = offsetx < 0;

		//-------------------------------- STRAIGHT EDGE COLLISION CHECK
		// North
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
		>(ent->t.height + 0.5f) && overlapN)
		{
			ent->t.position.y = ent->t.csi.c[eCELL_I].y; // + (1 - radius)
			ent->t.velocity.y = 0.f;
			touchNS = true;
		}
		// South
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapS)
		{
			ent->t.position.y = ent->t.csi.c[eCELL_I].y; // - (1 - radius)
			ent->t.velocity.y = 0.f;
			touchNS = true;
		}
		// East
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapE)
		{
			ent->t.position.x = ent->t.csi.c[eCELL_I].x; // + (1 - radius)
			ent->t.velocity.x = 0.f;
			touchEW = true;
		}
		// West
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapW)
		{
			ent->t.position.x = ent->t.csi.c[eCELL_I].x; // - (1 - radius)
			ent->t.velocity.x = 0.f;
			touchEW = true;
		}

		//-------------------------------- CORNER COLLISION CHECK

		// North-east
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapN && overlapE) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		// North-west
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapN && overlapW) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		// South-east
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapS && overlapE) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		// South-west
		if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.height + 0.5f) && overlapS && overlapW) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		//*/
	}

	void RemoveAllReferences(btID index)
	{
		// Remove us from the cell we're on
		refCells[ENTITY(index)->t.csi.c[eCELL_I].x][ENTITY(index)->t.csi.c[eCELL_I].y].ref_ents.remove(index);
		// Remove all references to us by other entities
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			if (block_entity.used[i] && i != index) // If entity exists and is not me
			{
				if (ENTITY(i)->type == ENTITY_TYPE_CHARA) // and is actor
				{
					if (ACTOR(i)->atk_target == index)
						ACTOR(i)->atk_target = BUF_NULL;
					if (ACTOR(i)->ai_target_ent == index)
						ACTOR(i)->ai_target_ent = BUF_NULL;
					if (ACTOR(i)->ai_ally_ent == index)
						ACTOR(i)->ai_ally_ent = BUF_NULL;
				}
			}
		}
	}

	bool LOSCheck(btID enta, btID entb)
	{
		Entity* entity_a = ENTITY(enta);
		Entity* entity_b = ENTITY(entb);
		return env::LineTrace_Bresenham(
			entity_a->t.csi.c[eCELL_I].x, entity_a->t.csi.c[eCELL_I].y,
			entity_b->t.csi.c[eCELL_I].x, entity_b->t.csi.c[eCELL_I].y,
			entity_a->t.height, entity_b->t.height);
	}

	btID GetClosestPlayer(btID index)
	{
		btf32 check_distance_0 = m::Length(ENTITY(players[0])->t.position - ENTITY(index)->t.position);
		btf32 check_distance_1 = m::Length(ENTITY(players[1])->t.position - ENTITY(index)->t.position);
		if (check_distance_1 > check_distance_0) // Which player is closer to me
			return players[1];
		else
			return players[0];
	}

	btID GetClosestEntity(btID index, btf32 dist)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			// If used, not me, and is alive
			if (block_entity.used[i] && i != index && ENTITY(i)->state.stateFlags.get(ActiveState::eALIVE))
			{
				btf32 check_distance = m::Length(ENTITY(i)->t.position - ENTITY(index)->t.position);
				if (check_distance < closest_distance)
				{
					current_closest = i;
					closest_distance = check_distance;
				}
			}
		}
		return current_closest;
	}

	// TODO: ANGLE IS ALREADY CALCULATED HERE, SO REUSE IT FOR THE PROJECTILE CODE INSTEAD OF REGENNING
	btID GetViewTargetEntity(btID index, btf32 dist, fac::facalleg allegiance)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_angle = 15.f;
		for (int index_other = 0; index_other <= block_entity.index_end; index_other++)
		{
			// If used, not me, and is alive
			if (block_entity.used[index_other] && index_other != index && ENTITY(index_other)->state.stateFlags.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(index_other)->faction) == allegiance)
				{
					btf32 check_distance = m::Length(ENTITY(index_other)->t.position - ENTITY(index)->t.position);
					if (check_distance < dist)
					{
						Entity* ent = ENTITY(index);
						Entity* ent_other = ENTITY(index_other);
						// LINE TRACE
						if (env::LineTrace_Bresenham(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y,
							ent_other->t.csi.c[eCELL_I].x, ent_other->t.csi.c[eCELL_I].y,
							ent->t.height, ent_other->t.height))
						{
							m::Vector2 targetoffset = m::Normalize(ENTITY(index_other)->t.position - (ENTITY(index)->t.position));
							m::Angle angle_yaw(glm::degrees(m::Vec2ToAng(targetoffset)));
							btf32 angdif = abs(m::AngDif(angle_yaw.Deg(), ACTOR(index)->viewYaw.Deg()));
							if (abs(angdif) < closest_angle)
							{
								closest_angle = angdif;
								current_closest = index_other;
							}
						}
					}
				}
			}
		}
		return current_closest;
	}

	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			// If used, not me, and is alive
			if (block_entity.used[i] && i != index && ENTITY(i)->state.stateFlags.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(i)->faction) == allegiance)
				{
					btf32 check_distance = m::Length(ENTITY(i)->t.position - ENTITY(index)->t.position);
					if (check_distance < closest_distance)
					{
						current_closest = i;
						closest_distance = check_distance;
					}
				}
			}
		}
		return current_closest;
	}

	btID GetClosestEntityAllegLOS(btID index, btf32 dist, fac::facalleg allegiance)
	{
		Entity* entity_index = ENTITY(index);
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			Entity* entity = ENTITY(i);
			// If used, not me, and is alive
			if (block_entity.used[i] && i != index && entity->state.stateFlags.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(entity_index->faction, entity->faction) == allegiance)
				{
					btf32 check_distance = m::Length(entity->t.position - entity_index->t.position);
					if (check_distance < closest_distance)
					{
						// Linetrace environment to see if the character is visible
						/*if (env::LineTrace(entity_index->t.position.x, entity_index->t.position.y,
							entity->t.position.x, entity->t.position.y))*/
						if (env::LineTrace_Bresenham(
							entity_index->t.csi.c[eCELL_I].x, entity_index->t.csi.c[eCELL_I].y,
							entity->t.csi.c[eCELL_I].x, entity->t.csi.c[eCELL_I].y,
							entity_index->t.height, entity->t.height))
						{
							current_closest = i;
							closest_distance = check_distance;
						}
					}
				}
			}
		}
		return current_closest;
	}

	btID GetClosestActivator(btID index)
	{
		btID id = BUF_NULL;
		btf32 closestDist = 2.f;
		btf32 closest_angle = 20.f;

		Entity* entity_index = ENTITY(index);
		// Iterate through nearby cells
		for (int x = entity_index->t.csi.c[eCELL_I].x - 3u; x < entity_index->t.csi.c[eCELL_I].x + 3u; x++)
		{
			for (int y = entity_index->t.csi.c[eCELL_I].y - 3u; y < entity_index->t.csi.c[eCELL_I].y + 3u; y++)
			{
				// Iterate through every entity space in this cell
				for (int e = 0; e <= refCells[x][y].ref_ents.end(); e++)
				{
					//if (cells[x][y].ents[e] != ID_NULL && block_entity.used[cells[x][y].ents[e]] && ENTITY(cells[x][y].ents[e])->Type() == Entity::eITEM)
					if (refCells[x][y].ref_ents[e] != ID_NULL && refCells[x][y].ref_ents[e] != index && block_entity.used[refCells[x][y].ref_ents[e]])
					{
						btf32 check_distance = m::Length(ENTITY(refCells[x][y].ref_ents[e])->t.position - entity_index->t.position);
						if (check_distance < closestDist)
						{
							m::Vector2 targetoffset = m::Normalize(ENTITY(refCells[x][y].ref_ents[e])->t.position - entity_index->t.position);

							m::Angle angle_yaw(glm::degrees(m::Vec2ToAng(targetoffset)));

							btf32 angdif = abs(m::AngDif(angle_yaw.Deg(), ACTOR(index)->viewYaw.Deg()));
							if (abs(angdif) < closest_angle)
							{
								closest_angle = angdif;
								id = refCells[x][y].ref_ents[e];
							}
						}
					}
				}
			}
		}
		return id;
	}

	//________________________________________________________________________________________________________________________________
	// PREFABS -----------------------------------------------------------------------------------------------------------------------

	inline void spawn_setup_t(btID index, m::Vector2 pos, btf32 dir)
	{
		ENTITY(index)->t.position = pos;
		ENTITY(index)->t.velocity = 0.f;
		ENTITY(index)->t.height_velocity = 0.f;
		ENTITY(index)->t.yaw.Set(dir);
		GetCellSpaceInfo(ENTITY(index)->t.position, ENTITY(index)->t.csi);
		env::GetHeight(ENTITY(index)->t.height, ENTITY(index)->t.csi);
		AddEntityCell(ENTITY(index)->t.csi.c[eCELL_I].x, ENTITY(index)->t.csi.c[eCELL_I].y, index);
		ENTITY(index)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(index)->state.damagestate = STATE_DAMAGE_MAX;
		ENTITY(index)->radius = 0.15f;
		ENTITY(index)->height = 0.7f;
		if (ENTITY(index)->type == ENTITY_TYPE_CHARA)
		{
			CHARA(index)->atk_target = BUF_NULL;
			CHARA(index)->ai_target_ent = BUF_NULL;
			CHARA(index)->ai_ally_ent = BUF_NULL;
			CHARA(index)->footPosL = m::Vector3(pos.x, 0, pos.y);
			CHARA(index)->footPosTargL = m::Vector3(pos.x, 0, pos.y);
			CHARA(index)->footPosR = m::Vector3(pos.x, 0, pos.y);
			CHARA(index)->footPosTargR = m::Vector3(pos.x, 0, pos.y);

			const m::Vector3 colEyes[]{
				m::Vector3(232.f / 256.f, 17.f / 256.f, 17.f / 256.f), // red
				m::Vector3(241.f / 256.f, 236.f / 256.f, 231.f / 256.f), // white
				m::Vector3(239.f / 256.f, 169.f / 256.f, 18.f / 256.f), // yellow
				m::Vector3(177.f / 256.f, 205.f / 256.f, 21.f / 256.f), // acid green
				m::Vector3(52.f / 256.f, 142.f / 256.f, 199.f / 256.f), // blue
				m::Vector3(61.f / 256.f, 155.f / 256.f, 9.f / 256.f), // green
				m::Vector3(178.f / 256.f, 107.f / 256.f, 22.f / 256.f), // gold
				m::Vector3(18.f / 256.f, 144.f / 256.f, 137.f / 256.f), // aqua
			};

			const m::Vector3 colBase[]{
				m::Vector3(233.f / 256.f, 231.f / 256.f, 226.f / 256.f), // white
				m::Vector3(127.f / 256.f, 117.f / 256.f, 111.f / 256.f), // midgrey
				m::Vector3(38.f / 256.f, 37.f / 256.f, 36.f / 256.f), // black-blue
				m::Vector3(16.f / 256.f, 16.f / 256.f, 16.f / 256.f), // black
				m::Vector3(97.f / 256.f, 84.f / 256.f, 75.f / 256.f), // grey-brown
				m::Vector3(242.f / 256.f, 222.f / 256.f, 187.f / 256.f), // cream
				m::Vector3(150.f / 256.f, 121.f / 256.f, 76.f / 256.f), // brown
				m::Vector3(143.f / 256.f, 109.f / 256.f, 50.f / 256.f), // yellow
				m::Vector3(152.f / 256.f, 144.f / 256.f, 127.f / 256.f), // template grey
				m::Vector3(91.f / 256.f, 71.f / 256.f, 75.f / 256.f), // purple
			};

			const m::Vector3 colNose[]{
				m::Vector3(222.f / 256.f, 193.f / 256.f, 185.f / 256.f),
				m::Vector3(233.f / 256.f, 152.f / 256.f, 136.f / 256.f),
				m::Vector3(103.f / 256.f, 57.f / 256.f, 57.f / 256.f),
				m::Vector3(31.f / 256.f, 29.f / 256.f, 29.f / 256.f),
			};

			const btf32 hue_offs = 0.25f;
			const btf32 bri_offs = 0.05f;
			m::Vector3 col_base_a = colBase[rand() % 10]
				+ (colBase[rand() % 10] * hue_offs) - hue_offs
				+ m::Vector3(m::Random(-bri_offs, bri_offs));
			m::Vector3 col_eyes = colEyes[rand() % 8]
				+ m::Vector3(m::Random(-bri_offs, bri_offs));
			m::Vector3 col_base_b = colBase[rand() % 10]
				+ (colBase[rand() % 10] * hue_offs) 
				+ m::Vector3(m::Random(-bri_offs, bri_offs));

			CHARA(index)->skin_col_a = col_base_a;
			CHARA(index)->skin_col_b = col_eyes;
			CHARA(index)->skin_col_c = col_base_b;
		}
	}

	namespace prefab
	{
		enum prefabtype : btui8
		{
			prefab_player,
			prefab_ai_player,
			prefab_npc,
			prefab_zombie,
			PREFAB_EDITORPAWN,
		};
	}

	void prefab_pc(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 1.45f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(6u); // long smig
		CHARA(id)->inventory.AddNew(4u); // fist
		CHARA(id)->inventory.AddNew(7u); // heal
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(10u); // time gun
		CHARA(id)->inventory.AddNew(11u); // time mag
		CHARA(id)->foot_state = FootState::eL_DOWN;
	}

	void prefab_aipc(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::player;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 1.45f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(6u);
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(7u); // heal
		CHARA(id)->foot_state = FootState::eL_DOWN;
	}

	void prefab_npc(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::playerhunter;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 1.45f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(0u);
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->foot_state = FootState::eL_DOWN;
	}

	void prefab_zombie(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::undead;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 3.5f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(4u);
		CHARA(id)->foot_state = FootState::eL_DOWN;
	}

	void prefab_editorpawn(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_EDITOR_PAWN);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 0.1f;
	}

	void(*PrefabEntity[])(btID, m::Vector2, btf32) = { prefab_pc, prefab_aipc, prefab_npc, prefab_zombie, prefab_editorpawn };

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- CELL STUFF -----------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	#define eN 1u
	#define eE 2u

	// Order is always: This -> X -> Y -> Diagonal

	void GetCgNE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x + 1u; cc[eCELL_X].y = y; // E (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y + 1u; // N (Y)
		cc[eCELL_XY].x = x + 1u; cc[eCELL_XY].y = y + 1u; // NE
	};
	void GetCgNW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x - 1u; cc[eCELL_X].y = y; // W (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y + 1u; // N (Y)
		cc[eCELL_XY].x = x - 1u; cc[eCELL_XY].y = y + 1u; // NW
	};
	void GetCgSE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x + 1u; cc[eCELL_X].y = y; // E (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y - 1u; // S (Y)
		cc[eCELL_XY].x = x + 1u; cc[eCELL_XY].y = y - 1u; // SE
	};
	void GetCgSW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x - 1u; cc[eCELL_X].y = y; // W (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y - 1u; // S (Y)
		cc[eCELL_XY].x = x - 1u; cc[eCELL_XY].y = y - 1u; // SW
	};

	void(*GetCellNeighbors[])(btcoord, btcoord, CellCoord*) = { GetCgSW, GetCgNW, GetCgSE, GetCgNE };

	void GetCellGroup(m::Vector2 vec, CellGroup& cg)
	{
		btui8 dir = 0u; // Represents direction of offset

		cg.c[eCELL_I].x = (btcoord)roundf(vec.x); // X cell coordinate
		cg.c[eCELL_I].y = (btcoord)roundf(vec.y); // Y cell coordinate

		dir = eN * (vec.y - (btf32)cg.c[eCELL_I].y > 0.f);  // Set direction bit N
		dir |= eE * (vec.x - (btf32)cg.c[eCELL_I].x > 0.f); // Add direction bit E

		GetCellNeighbors[dir](cg.c[eCELL_I].x, cg.c[eCELL_I].y, cg.c); // Get cell group from direction
	}

	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& cs)
	{
		if (vec.x < 1.f) vec.x = 1.f;
		if (vec.y < 1.f) vec.y = 1.f;
		if (vec.x > WORLD_SIZE_MAXINT - 1.f) vec.x = WORLD_SIZE_MAXINT - 1.f;
		if (vec.y > WORLD_SIZE_MAXINT - 1.f) vec.y = WORLD_SIZE_MAXINT - 1.f;

		btui8 dir = 0u; // Represents direction of offset

		cs.c[eCELL_I].x = (btcoord)roundf(vec.x); // X cell coordinate
		cs.c[eCELL_I].y = (btcoord)roundf(vec.y); // Y cell coordinate

		cs.offsety = vec.y - (btf32)cs.c[eCELL_I].y; // X offset
		cs.offsetx = vec.x - (btf32)cs.c[eCELL_I].x; // Y offset

		dir = eN * (cs.offsety > 0.f);  // Set direction bit N
		dir |= eE * (cs.offsetx > 0.f); // Add direction bit E

		GetCellNeighbors[dir](cs.c[eCELL_I].x, cs.c[eCELL_I].y, cs.c); // Get cell group from direction
	}

	#undef eN
	#undef eE
}