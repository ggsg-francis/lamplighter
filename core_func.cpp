// Included in index.cpp

#include "core_decl.cpp"

namespace index
{
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
		actor->inputBV.setto(Actor::ActorInput::IN_ACTN_B, actor->ai_target_ent == BUF_NULL);
		actor->inputBV.setto(Actor::ActorInput::IN_ACTN_A, actor->ai_target_ent != BUF_NULL);

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

			if (distance_to_target < 1.5f) // if enemy is close enough to swing at
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
					else if (actor->state.hp > 0.6f) // if ally is far and hitpoints high, move in
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
		bool overlapN, overlapS, overlapE, overlapW;

		//-------------------------------- ACTOR COLLISION CHECK

		if (ent->properties.get(Entity::eCOLLIDE_ENT))
		{
			for (btcoord x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; ++x)
			{
				for (btcoord y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; ++y)
				{
					//de-intersect entities
					for (int e = 0; e <= cells[x][y].ents.end(); e++)
					{
						if (cells[x][y].ents[e] != ID_NULL)
						{
							if (block_entity.used[cells[x][y].ents[e]] && ENTITY(cells[x][y].ents[e])->properties.get(Entity::eCOLLIDE_ENT))
							{
								m::Vector2 vec = ent->t.position - ENTITY(cells[x][y].ents[e])->t.position;
								float dist = m::Length(vec);
								btf32 combined_radius = ent->radius + ENTITY(cells[x][y].ents[e])->radius;
								if (dist < combined_radius && dist > 0.f)
								{
									// TEMP! if same type
									if (ent->type == ENTITY(cells[x][y].ents[e])->type)
									{
										ent->t.position += m::Normalize(vec) * (combined_radius - dist) * 0.5f;
										ENTITY(cells[x][y].ents[e])->t.position -= m::Normalize(vec) * (combined_radius - dist) * 0.5f;
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

		//-------------------------------- ENVIRONMENTAL COLLISION CHECK (2ND THEREFORE PRIORITIZED)

		offsetx = ent->t.position.x - ent->t.csi.c[eCELL_I].x;
		offsety = ent->t.position.y - ent->t.csi.c[eCELL_I].y;

		overlapN = offsety > 0;
		overlapS = offsety < 0;
		overlapE = offsetx > 0;
		overlapW = offsetx < 0;

		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eSurfN) && overlapN) // N
		{
			ent->t.position.y = ent->t.csi.c[eCELL_I].y; // + (1 - radius)
			ent->t.velocity.y = 0.f;
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eSurfS) && overlapS) // S
		{
			ent->t.position.y = ent->t.csi.c[eCELL_I].y; // - (1 - radius)
			ent->t.velocity.y = 0.f;
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eSurfE) && overlapE) // E
		{
			ent->t.position.x = ent->t.csi.c[eCELL_I].x; // + (1 - radius)
			ent->t.velocity.x = 0.f;
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eSurfW) && overlapW) // W
		{
			ent->t.position.x = ent->t.csi.c[eCELL_I].x; // - (1 - radius)
			ent->t.velocity.x = 0.f;
		}

		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eCorOutNE) && overlapN && overlapE) // NE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eCorOutNW) && overlapN && overlapW) // NW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eCorOutSE) && overlapS && overlapE) // SE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, env::eflag::eCorOutSW) && overlapS && overlapW) // SW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
	}

	void RemoveAllReferences(btID index)
	{
		// Remove us from the cell we're on
		cells[ENTITY(index)->t.csi.c[eCELL_I].x][ENTITY(index)->t.csi.c[eCELL_I].y].ents.remove(index);
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
			entity_b->t.csi.c[eCELL_I].x, entity_b->t.csi.c[eCELL_I].y);
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
							ent_other->t.csi.c[eCELL_I].x, ent_other->t.csi.c[eCELL_I].y))
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
							entity->t.csi.c[eCELL_I].x, entity->t.csi.c[eCELL_I].y))
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
				for (int e = 0; e <= cells[x][y].ents.end(); e++)
				{
					//if (cells[x][y].ents[e] != ID_NULL && block_entity.used[cells[x][y].ents[e]] && ENTITY(cells[x][y].ents[e])->Type() == Entity::eITEM)
					if (cells[x][y].ents[e] != ID_NULL && cells[x][y].ents[e] != index && block_entity.used[cells[x][y].ents[e]])
					{
						btf32 check_distance = m::Length(ENTITY(cells[x][y].ents[e])->t.position - entity_index->t.position);
						if (check_distance < closestDist)
						{
							m::Vector2 targetoffset = m::Normalize(ENTITY(cells[x][y].ents[e])->t.position - entity_index->t.position);

							m::Angle angle_yaw(glm::degrees(m::Vec2ToAng(targetoffset)));

							btf32 angdif = abs(m::AngDif(angle_yaw.Deg(), ACTOR(index)->viewYaw.Deg()));
							if (abs(angdif) < closest_angle)
							{
								closest_angle = angdif;
								id = cells[x][y].ents[e];
							}
						}
					}
				}
			}
		}
		return id;
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- PREFABS --------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	void IndexInitEntity(btID id, EntityType type)
	{
		block_entity.used[id] = true;
		block_entity_data[id].type = type;
		bool type_recognized = false;
		switch (type)
		{
		case ENTITY_TYPE_EDITOR_PAWN:
			//_entities[id] = new EditorPawn();
			memset(ENT_VOID(id), 0, sizeof(EditorPawn));
			//((EditorPawn)*((EditorPawn*)_entities[id])) = EditorPawn();
			//(ENTITY(id))->fpName = DisplayNameActor;
			//(ENTITY(id))->fpTick = TickEditorPawn;
			//(ENTITY(id))->fpDraw = DrawEditorPawn;
			type_recognized = true;
			break;
		case ENTITY_TYPE_RESTING_ITEM:
			block_entity_data[id].type_buffer_index = buf_resting_item.add();
			//_entities[id] = malloc(sizeof(RestingItem));
			memset(ENT_VOID(id), 0, sizeof(RestingItem));
			//((RestingItem)*((RestingItem*)_entities[id])) = RestingItem();
			//(ENTITY(id))->fpName = DisplayNameRestingItem;
			//(ENTITY(id))->fpTick = TickRestingItem;
			//(ENTITY(id))->fpDraw = DrawRestingItem;
			// TODO: this should be properly formalized, i'm not supposed to be using this like this
			// atm this is only supposed to make sure that the item reference is added to the cells
			//spawn_setup_t(id, ENTITY(id)->t.position, ENTITY(id)->t.yaw.Deg());
			type_recognized = true;
			break;
		case ENTITY_TYPE_CHARA:
			block_entity_data[id].type_buffer_index = buf_chara.add();
			//_entities[id] = malloc(sizeof(Chara));
			memset(ENT_VOID(id), 0, sizeof(Chara));
			((Chara)*(CHARA(id))) = Chara();
			//(ENTITY(id))->fpName = DisplayNameActor;
			//(ENTITY(id))->fpTick = TickChara;
			//(ENTITY(id))->fpDraw = DrawChara;
			type_recognized = true;
			break;
		default:
			std::cout << "Tried to initialize entity of no valid type" << std::endl;
			break;
		}

		if (type_recognized)
		{
			(ENTITY(id))->id = id;
			(ENTITY(id))->type = type;
		}
	}
	void IndexFreeEntity(btID id)
	{
		if (block_entity.used[id])
		{
			switch (ENTITY(id)->type)
			{
			case ENTITY_TYPE_EDITOR_PAWN:
				//delete _entities[id];
				break;
			case ENTITY_TYPE_RESTING_ITEM:
				buf_resting_item.remove(block_entity_data[id].type_buffer_index);
				break;
			case ENTITY_TYPE_CHARA:
				buf_chara.remove(block_entity_data[id].type_buffer_index);
				break;
			}
		}
		block_entity.remove(id);
	}
	void IndexInitItem(btID id, ItemType type)
	{
		block_item.used[id] = true;
		block_item_data[id].type = type;
		block_item_data[id].type_buffer_index = ObjBuf_add(ItemBufPtr[type]);
		HeldItem* held_item = GETITEM_MISC(id);
		switch (type)
		{
		case ITEM_TYPE_MISC:
			*held_item = HeldItem();
			held_item->fpTick = HeldItemTick;
			held_item->fpDraw = HeldItemDraw;
			held_item->fpOnEquip = HeldItemOnEquip;
			held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
			held_item->fpBlockTurn = HeldItemBlockTurn;
			held_item->fpBlockMove = HeldItemBlockMove;
			break;
		case ITEM_TYPE_EQUIP:
			*held_item = HeldItem();
			held_item->fpTick = HeldItemTick;
			held_item->fpDraw = HeldItemDraw;
			held_item->fpOnEquip = HeldItemOnEquip;
			held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
			held_item->fpBlockTurn = HeldItemBlockTurn;
			held_item->fpBlockMove = HeldItemBlockMove;
			break;
		case ITEM_TYPE_WPN_MELEE:
			*held_item = HeldMel();
			held_item->fpTick = HeldMelTick;
			held_item->fpDraw = HeldMelDraw;
			held_item->fpOnEquip = HeldMelOnEquip;
			held_item->fpGetLeftHandPos = HeldMelGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldMelGetRightHandPos;
			held_item->fpBlockTurn = HeldMelBlockTurn;
			held_item->fpBlockMove = HeldMelBlockMove;
			break;
		case ITEM_TYPE_WPN_MATCHGUN:
			*held_item = HeldGun();
			//memset(held_item, 0, sizeof(HeldGun));
			//*held_item = HeldGunMatchLock();
			held_item->fpTick = HeldGunTick;
			held_item->fpDraw = HeldGunDraw;
			held_item->fpOnEquip = HeldGunOnEquip;
			held_item->fpGetLeftHandPos = HeldGunGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldGunGetRightHandPos;
			held_item->fpBlockTurn = HeldGunBlockTurn;
			held_item->fpBlockMove = HeldGunBlockMove;
			break;
		case ITEM_TYPE_WPN_MAGIC:
			*held_item = HeldMgc();
			held_item->fpTick = HeldMgcTick;
			held_item->fpDraw = HeldMgcDraw;
			held_item->fpOnEquip = HeldMgcOnEquip;
			held_item->fpGetLeftHandPos = HeldMgcGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldMgcGetRightHandPos;
			held_item->fpBlockTurn = HeldMgcBlockTurn;
			held_item->fpBlockMove = HeldMgcBlockMove;
			break;
		case ITEM_TYPE_CONS:
			*held_item = HeldCons();
			held_item->fpTick = HeldConTick;
			held_item->fpDraw = HeldConDraw;
			held_item->fpOnEquip = HeldConOnEquip;
			held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
			held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
			held_item->fpBlockTurn = HeldItemBlockTurn;
			held_item->fpBlockMove = HeldItemBlockMove;
			break;
		}
	}
	void IndexFreeItem(btID id)
	{
		if (block_item.used[id])
		{
			switch (block_item_data[id].type)
			{
			case ITEM_TYPE_EQUIP:
				ObjBuf_remove(&buf_item_misc, block_item_data[id].type_buffer_index);
				break;
			case ITEM_TYPE_WPN_MELEE:
				ObjBuf_remove(&buf_item_melee, block_item_data[id].type_buffer_index);
				break;
			case ITEM_TYPE_WPN_MATCHGUN:
				ObjBuf_remove(&buf_item_gun, block_item_data[id].type_buffer_index);
				break;
			case ITEM_TYPE_WPN_MAGIC:
				ObjBuf_remove(&buf_item_mgc, block_item_data[id].type_buffer_index);
				break;
			case ITEM_TYPE_CONS:
				ObjBuf_remove(&buf_item_con, block_item_data[id].type_buffer_index);
				break;
			}
		}
		ObjBuf_remove(&block_item, id);
	}

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
		ENTITY(index)->state.hp = 1.f;
		ENTITY(index)->radius = 0.5f;
		ENTITY(index)->height = 1.9f;
		if (ENTITY(index)->type == ENTITY_TYPE_CHARA)
		{
			CHARA(index)->atk_target = BUF_NULL;
			CHARA(index)->ai_target_ent = BUF_NULL;
			CHARA(index)->ai_ally_ent = BUF_NULL;
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
		CHARA(id)->t_skin = 0u;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 2.9f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(6u); // long smig
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(7u); // heal
		CHARA(id)->foot_state = FootState::eBOTH_DOWN;
	}

	void prefab_aipc(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::player;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 0u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 2.9f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(6u);
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(7u); // heal
		CHARA(id)->foot_state = FootState::eBOTH_DOWN;
	}

	void prefab_npc(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::playerhunter;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 2u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 2.9f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(0u);
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->inventory.AddNew(8u); // magazine
		CHARA(id)->foot_state = FootState::eBOTH_DOWN;
	}

	void prefab_zombie(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_CHARA);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::undead;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 3u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 3.5f;
		CHARA(id)->agility = 0.f;
		CHARA(id)->inventory.AddNew(4u);
		CHARA(id)->foot_state = FootState::eBOTH_DOWN;
	}

	void prefab_editorpawn(btID id, m::Vector2 pos, btf32 dir)
	{
		IndexInitEntity(id, ENTITY_TYPE_EDITOR_PAWN);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->t_skin = res::t_skin_template;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 0.1f;
	}

	void(*PrefabEntity[])(btID, m::Vector2, btf32) = { prefab_pc, prefab_aipc, prefab_npc, prefab_zombie, prefab_editorpawn };

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- CELL STUFF -----------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	#define eN 1ui8
	#define eE 2ui8

	// Order is always: This -> X -> Y -> Diagonal

	void GetCgNE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x + 1ui8; cc[eCELL_X].y = y;          // E (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y + 1ui8;          // N (Y)
		cc[eCELL_XY].x = x + 1ui8; cc[eCELL_XY].y = y + 1ui8; // NE
	};
	void GetCgNW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x - 1ui8; cc[eCELL_X].y = y;          // W (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y + 1ui8;          // N (Y)
		cc[eCELL_XY].x = x - 1ui8; cc[eCELL_XY].y = y + 1ui8; // NW
	};
	void GetCgSE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x + 1ui8; cc[eCELL_X].y = y;          // E (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y - 1ui8;          // S (Y)
		cc[eCELL_XY].x = x + 1ui8; cc[eCELL_XY].y = y - 1ui8; // SE
	};
	void GetCgSW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[eCELL_X].x = x - 1ui8; cc[eCELL_X].y = y;          // W (X)
		cc[eCELL_Y].x = x; cc[eCELL_Y].y = y - 1ui8;          // S (Y)
		cc[eCELL_XY].x = x - 1ui8; cc[eCELL_XY].y = y - 1ui8; // SW
	};

	void(*GetCellNeighbors[])(btcoord, btcoord, CellCoord*) = { GetCgSW, GetCgNW, GetCgSE, GetCgNE };

	void GetCellGroup(m::Vector2 vec, CellGroup& cg)
	{
		btui8 dir = 0ui8; // Represents direction of offset

		cg.c[eCELL_I].x = (btcoord)roundf(vec.x); // X cell coordinate
		cg.c[eCELL_I].y = (btcoord)roundf(vec.y); // Y cell coordinate

		dir = eN * (vec.y - (btf32)cg.c[eCELL_I].y > 0.f);  // Set direction bit N
		dir |= eE * (vec.x - (btf32)cg.c[eCELL_I].x > 0.f); // Add direction bit E

		GetCellNeighbors[dir](cg.c[eCELL_I].x, cg.c[eCELL_I].y, cg.c); // Get cell group from direction
	}

	void GetCellSpaceInfo(m::Vector2 vec, CellSpace& cs)
	{
		btui8 dir = 0ui8; // Represents direction of offset

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