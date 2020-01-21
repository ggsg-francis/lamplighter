// Included in index.cpp

#include "index_decl.cpp"

namespace index
{
	Entity* GetEnt(btID id)
	{
		if (block_entity.used[id])
			return ((Entity*)_entities[id]);
		else
			return nullptr;
	}

	#define CHARA(a) ((Chara*)_entities[a])
	#define ACTOR(a) ((Actor*)_entities[a])
	#define ENTITY(a) ((Entity*)_entities[a])
	#define ITEM(a) ((EItem*)_entities[a])

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
			|| !ENTITY(actor->ai_target_ent)->state.properties.get(ActiveState::eALIVE)/*
			|| !LOSCheck(id, actor->ai_target_ent)*/)
		{
			
			actor->ai_target_ent = GetClosestEntityAllegLOS(id, 100.f, fac::enemy); // Find the closest enemy
			actor->atk_target = actor->ai_target_ent;
		}
		//// umm
		//if (actor->ai_target_ent != BUF_NULL)
		//{
		//	if (!LOSCheck(id, actor->ai_target_ent))
		//	{
		//		int i = 0;
		//	}
		//}
	updateally:
		// If is null OR deleted OR dead
		if (actor->ai_ally_ent == BUF_NULL
			|| !block_entity.used[actor->ai_ally_ent]
			|| !ENTITY(actor->ai_ally_ent)->state.properties.get(ActiveState::eALIVE)
			|| !LOSCheck(id, actor->ai_ally_ent))
			actor->ai_ally_ent = GetClosestEntityAllegLOS(id, 100.f, fac::allied); // Find the closest ally

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
				m::Vector2 TargetVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
				btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
				float distance_to_target = m::Length(TargetVector);

				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				actor->ai_vy_target = angle2;

				if (distance_to_target > 5.f) // if ally is far away
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

					// if ally is closer than enemy
					if (distance_to_ally > distance_to_target || distance_to_ally < 4.f)
					{
						if (distance_to_ally > 2.f)
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
						if (distance_to_ally > 2.f) actor->input.y = 1.f;
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

	void ActorRunAIBak(btID id)
	{
		Actor* actor = ACTOR(id);

		actor->input.y = 1.f;
		actor->input.x = -1.f;

		actor->ai_target_ent = GetClosestEntityAlleg(id, 100.f, fac::enemy); // Find the closest enemy

		// If ally is null or deleted or dead
		if (actor->ai_ally_ent == BUF_NULL || !block_entity.used[actor->ai_ally_ent]
			|| !ENTITY(actor->ai_ally_ent)->state.properties.get(ActiveState::eALIVE))
			actor->ai_ally_ent = GetClosestEntityAlleg(id, 100.f, fac::allied); // Find the closest ally

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
				m::Vector2 TargetVector = ENTITY(actor->ai_ally_ent)->t.position - actor->t.position;
				btf32 angle2 = glm::degrees(m::Vec2ToAng(m::Normalize(TargetVector)));
				float distance_to_target = m::Length(TargetVector);

				//actor->viewYaw.Set(angle2);
				//actor->viewYaw.RotateTowards(angle2, HEAD_TURN_SPEED);
				actor->ai_vy_target = angle2;

				if (distance_to_target > 5.f) // if ally is far away
					actor->input.y = 1.f;
				else
					actor->input.y = 0.f;
				actor->input.x = 0.f;
				actor->inputBV.unset(Actor::IN_USE);
			}
		}
		else
		{
			//find new target if ours is dead (obsolete right now)
			//if (!ENTITY(actor->target_ent)->state.properties.get(ActiveState::eALIVE))
			//{
			//	//actor->target_ent = GetClosestEntity(id, 10.f); // Find the closest entity
			//	//actor->target_ent = GetClosestPlayer(id); // Find the closest player
			//	actor->target_ent = GetClosestEntityAlleg(id, 100.f, fac::enemy); // Find the closest enemy
			//}

			m::Vector2 TargetVector = ENTITY(actor->ai_target_ent)->t.position - actor->t.position;
			m::Vector2 TargetVectorVertical = m::Vector2(m::Length(TargetVector), ENTITY(actor->ai_target_ent)->t.height - actor->t.height);
			float distance_to_target = m::Length(TargetVector);

			actor->inputBV.unset(Actor::IN_USE);

			if (distance_to_target < 20.f) // if enemy is close enough to shoot at
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

					// if ally is closer than enemy
					if (distance_to_ally > distance_to_target || distance_to_ally < 4.f)
					{
						if (distance_to_ally > 2.f)
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
						if (distance_to_ally > 2.f) actor->input.y = 1.f;
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

	void EntDeintersect(Entity* ent, CellSpace& csi, btf32 rot, bool knockback)
	{
		btf32 offsetx, offsety;
		bool overlapN, overlapS, overlapE, overlapW, touchNS = false, touchEW = false;

		//if (actor::moving[i]) // Add if moving check?

		//-------------------------------- ENVIRONMENTAL COLLISION CHECK

		/*offsetx = ent->t.position.x - ent->t.cellx;
		offsety = ent->t.position.y - ent->t.celly;*/
		offsetx = ent->t.position.x - ent->csi.c[eCELL_I].x;
		offsety = ent->t.position.y - ent->csi.c[eCELL_I].y;

		overlapN = offsety > 0;
		overlapS = offsety < 0;
		overlapE = offsetx > 0;
		overlapW = offsetx < 0;

		ent->freeTurn = true;

		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eSurfN) && overlapN) // N
		{
			ent->t.position.y = ent->csi.c[eCELL_I].y; // + (1 - radius)
			ent->t.velocity.y = 0.f;
			touchNS = true;
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eSurfS) && overlapS) // S
		{
			ent->t.position.y = ent->csi.c[eCELL_I].y; // - (1 - radius)
			ent->t.velocity.y = 0.f;
			touchNS = true;
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eSurfE) && overlapE) // E
		{
			ent->t.position.x = ent->csi.c[eCELL_I].x; // + (1 - radius)
			ent->t.velocity.x = 0.f;
			touchEW = true;
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eSurfW) && overlapW) // W
		{
			ent->t.position.x = ent->csi.c[eCELL_I].x; // - (1 - radius)
			ent->t.velocity.x = 0.f;
			touchEW = true;
		}

		if (touchNS && !touchEW) // Touching north or south wall
		{
			if (rot < 180.f)
				ent->yaw.RotateTowards(90.f, rotdeg);
			else
				ent->yaw.RotateTowards(270.f, rotdeg);
			ent->freeTurn = false;
		}
		else if (touchEW && !touchNS) // Touching east or west wall
		{
			if (rot > 90.f && rot < 270.f)
				ent->yaw.RotateTowards(180.f, rotdeg);
			else
				ent->yaw.RotateTowards(0.f, rotdeg);
			ent->freeTurn = false;
		}

		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eCorOutNE) && overlapN && overlapE) // NE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eCorOutNW) && overlapN && overlapW) // NW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eCorOutSE) && overlapS && overlapE) // SE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->csi.c[eCELL_I].x, ent->csi.c[eCELL_I].y, env::eflag::eCorOutSW) && overlapS && overlapW) // SW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}

		//-------------------------------- ACTOR COLLISION CHECK

		for (int x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; x++)
			for (int y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; y++)
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
							if (dist < 0.8f && dist > 0.f)
							{
								ent->t.position += m::Normalize(vec) * (0.8f - dist);
								//massively slow collide callback (we'll speed it up later k?)
								collision::hit_info hit;
								hit.depenetrate = m::Normalize(vec) * (0.8f - dist);
								hit.hit = true;
								hit.surface = m::Normalize(vec);
								hit.inheritedVelocity = ENTITY(cells[x][y].ents[e])->t.velocity;
								//entity->Collide(hit);
								collision::hit_info hit2;
								hit2.depenetrate = m::Normalize(vec * -1.f) * (0.8f - dist);
								hit2.hit = true;
								hit2.surface = m::Normalize(vec * -1.f);
								hit2.inheritedVelocity = ent->t.velocity;

								if (knockback)
								{
									if (ent->IsActor())
										ent->t.velocity = hit2.surface * -0.1f; // set my velocity
									if (ENTITY(cells[x][y].ents[e])->IsActor())
										ENTITY(cells[x][y].ents[e])->t.velocity = hit2.surface * 0.3f; // set their velocity
								}
							}
						}
					} // End for each entity in cell
				} // End if entity count of this cell is bigger than zero
			} // End for each cell group


		/*
		for (int cell_group = 0; cell_group < 4; cell_group++)
		{
			//de-intersect entities
			if (cells[csi.c[cell_group].x][csi.c[cell_group].y].ents.size() > 0)
			{
				for (int e = 0; e < cells[csi.c[cell_group].x][csi.c[cell_group].y].ents.size(); e++)
				{
					#define ID cells[csi.c[cell_group].x][csi.c[cell_group].y].ents[e]
					if (block_entity.used[ID] && ENTITY(ID)->properties.get(Entity::eCOLLIDE_ENT))
					{
						m::Vector2 vec = ent->t.position - ENTITY(ID)->t.position;
						float dist = m::Length(vec);
						if (dist < 0.8f && dist > 0.f)
						{
							ent->t.position += m::Normalize(vec) * (0.8f - dist);
							//massively slow collide callback (we'll speed it up later k?)
							collision::hit_info hit;
							hit.depenetrate = m::Normalize(vec) * (0.8f - dist);
							hit.hit = true;
							hit.surface = m::Normalize(vec);
							hit.inheritedVelocity = ENTITY(ID)->t.velocity;
							//entity->Collide(hit);
							collision::hit_info hit2;
							hit2.depenetrate = m::Normalize(vec * -1.f) * (0.8f - dist);
							hit2.hit = true;
							hit2.surface = m::Normalize(vec * -1.f);
							hit2.inheritedVelocity = ent->t.velocity;

							if (knockback)
							{
								if (ent->Type() == etype::chara)
								{
									ent->t.velocity = hit2.surface * -0.1f; // set their velocity
									((Chara*)ent)->animPlayer.setAnim(res::a_knockback);
								}
								if (ENTITY(ID)->Type() == etype::chara)
								{
									ENTITY(ID)->t.velocity = hit2.surface * 0.2f; // set their velocity
									CHARA(ID)->animPlayer.setAnim(res::a_knockback);
								}
							}
						}
					}
					#undef ID
				} // End for each entity in cell
			} // End if entity count of this cell is bigger than zero
		} // End for each cell group
		*/
	}

	void RemoveAllReferences(btID index)
	{
		for (int i = 0; i <= block_entity.index_end; i++)
			if (block_entity.used[i] && i != index) // If entity exists and is not me
				if (ENTITY(i)->IsActor()) // and is actor
				{
					if (ACTOR(i)->ai_target_ent == index) // and is targeting me
						ACTOR(i)->ai_target_ent = BUF_NULL; // Reset it's target
					if (ACTOR(i)->ai_ally_ent == index) // and is ally with me
						ACTOR(i)->ai_ally_ent = BUF_NULL; // Reset it's ally
				}
	}

	bool LOSCheck(btID enta, btID entb)
	{
		return env::LineTrace(
			ENTITY(enta)->t.position.x, ENTITY(enta)->t.position.y,
			ENTITY(entb)->t.position.x, ENTITY(entb)->t.position.y);
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
			if (block_entity.used[i] && i != index && ENTITY(i)->state.properties.get(ActiveState::eALIVE))
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

	// TO DO ANGLE IS ALREADY CALCULATED HERE, SO REUSE IT FOR THE PROJECTILE CODE INSTEAD OF REGENNING
	btID GetViewTargetEntity(btID index, btf32 dist, fac::facalleg allegiance)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_angle = 15.f;
		for (int index_other = 0; index_other <= block_entity.index_end; index_other++)
		{
			// If used, not me, and is alive
			if (block_entity.used[index_other] && index_other != index && ENTITY(index_other)->state.properties.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(index_other)->faction) == allegiance)
				{
					btf32 check_distance = m::Length(ENTITY(index_other)->t.position - ENTITY(index)->t.position);
					if (check_distance < dist)
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
		return current_closest;
	}

	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			// If used, not me, and is alive
			if (block_entity.used[i] && i != index && ENTITY(i)->state.properties.get(ActiveState::eALIVE))
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
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			// If used, not me, and is alive
			if (block_entity.used[i] && i != index && ENTITY(i)->state.properties.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(i)->faction) == allegiance)
				{
					btf32 check_distance = m::Length(ENTITY(i)->t.position - ENTITY(index)->t.position);
					if (check_distance < closest_distance)
					{
						// Linetrace environment to see if the character is visible
						if (env::LineTrace(ENTITY(index)->t.position.x, ENTITY(index)->t.position.y,
							               ENTITY(i)->t.position.x,     ENTITY(i)->t.position.y))
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

		// Iterate through nearby cells
		for (int x = ENTITY(index)->csi.c[eCELL_I].x - 3u; x < ENTITY(index)->csi.c[eCELL_I].x + 3u; x++)
		{
			for (int y = ENTITY(index)->csi.c[eCELL_I].y - 3u; y < ENTITY(index)->csi.c[eCELL_I].y + 3u; y++)
			{
				// Iterate through every entity space in this cell
				for (int e = 0; e <= cells[x][y].ents.end(); e++)
				{
					//if (cells[x][y].ents[e] != ID_NULL && block_entity.used[cells[x][y].ents[e]] && ENTITY(cells[x][y].ents[e])->Type() == Entity::eITEM)
					if (cells[x][y].ents[e] != ID_NULL && cells[x][y].ents[e] != index && block_entity.used[cells[x][y].ents[e]])
					{
						btf32 check_distance = m::Length(ENTITY(cells[x][y].ents[e])->t.position - ENTITY(index)->t.position);
						if (check_distance < closestDist)
						{
							m::Vector2 targetoffset = m::Normalize(ENTITY(cells[x][y].ents[e])->t.position - (ENTITY(index)->t.position));

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

	inline void spawn_setup_t(btID index, m::Vector2 pos, btf32 dir)
	{
		ePos = pos;
		eYaw2.Set(dir);
		GetCellSpaceInfo(ePos, eCSI);
		env::GetHeight(eHgt, eCSI);
		AddEntityCell(eCSI.c[eCELL_I].x, eCSI.c[eCELL_I].y, index);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- PREFABS --------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->t_skin = 0u;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 4.f;
		CHARA(id)->inventory.AddItem(6u);
	}

	void prefab_aipc(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::player;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 1u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 6.f;
		CHARA(id)->inventory.AddItem(6u);
	}

	void prefab_npc(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::playerhunter;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 2u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 5.f;
		CHARA(id)->inventory.AddItem(0u);
	}

	void prefab_zombie(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::undead;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = 3u;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 1.f;
		CHARA(id)->inventory.AddItem(4u);
	}

	void prefab_editorpawn(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new EditorPawn();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->t_skin = res::t_skin_template;
		CHARA(id)->aiControlled = false;
		CHARA(id)->speed = 6.f;
	}

	void(*PrefabEntity[])(btID, m::Vector2, btf32) = { prefab_pc, prefab_aipc, prefab_npc, prefab_zombie, prefab_editorpawn };

	// THESE FUNCTION DECLARATIONS ARE KEPT HERE TEMPORARILY

	// Creates an Entity instance, adds it to the index and allocates it an ID
	btID SpawnEntity(prefab::prefabtype TYPE, m::Vector2 pos, float dir);
	// Removes a given Entity from the index
	void DestroyEntity(btID ID);

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- CELL STUFF -----------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	enum gcg_dir : btui8 { eN = 1u, eE = 2u };

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
}