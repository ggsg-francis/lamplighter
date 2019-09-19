// Included in index.cpp

#include "index_decl.cpp"
//temp proly
#include "resources-archive.h"

namespace index
{
	Entity* GetEnt(btID id)
	{
		if (block_entity.used[id])
			return ((Entity*)_entities[id]);
		else
			return nullptr;
	}

	// errrr
	#define CHARA(a) ((Chara*)_entities[a])
	//#define ENTITY _entities
	#define ACTOR(a) ((Actor*)_entities[a])
	#define ENTITY(a) ((Entity*)_entities[a])
	#define ITEM(a) ((EItem*)_entities[a])

	void DrawInv()
	{
		inv::InvItems* items = ACTOR(players[0])->inventory.Get();

		for (btui16 i = 0; i < 10; i++)
		{
			graphics::DrawGUITexture(&res::GetBTT(res::t_gui_inv_slot), &graphics::shader_gui, i * 32, -240 + 16, 32, 32);
			if (i < items->invSize)
			{
				graphics::DrawGUITexture(&res::GetBTT(archive::items[items->items[i]]->id_icon), &graphics::shader_gui, i * 32, -240 + 16, 32, 32);
				char* ctest = new char[6];
				_itoa(items->itemCounts[i], ctest, 10);
				invtext.ReGen(ctest, strlen(ctest), i * 32 - 14, i * 32 + 30, -210);
				invtext.Draw(&graphics::shader_gui, &res::GetBTT(res::t_gui_font));
				if (i == inv_active_slot)
				{
					guibox_selection.ReGen((i * 32) - 8, (i * 32) + 8, -240 + 8, -240 + 24, 8, 8);
					guibox_selection.Draw(&graphics::shader_gui, &res::GetBTT(res::t_gui_select_box));
				}
				delete ctest;
			}
		}
	}

	void ActorRunAI(btID id)
	{
		Actor* actor = ACTOR(id);

		actor->input.y = 1.f;
		actor->input.x = -1.f;

		actor->target_ent = GetClosestEntityAlleg(id, 10.f, fac::enemy); // Find the closest target


		if (actor->target_ent == BUF_NULL)
		{
			actor->input.y = 0.f;
			actor->input.x = 0.f;
			actor->inputbv.unset(Actor::in_atk);
		}
		else
		{
			float mov = 0.f;

			//find new path
			if (!actor->haspath || !ENTITY(actor->target_ent)->state.properties.get(ActiveState::eALIVE))
			{
				//actor->target_ent = GetClosestEntity(id, 10.f); // Find the closest target
				//actor->target_ent = GetClosestPlayer(id); // Find the closest target
				actor->target_ent = GetClosestEntityAlleg(id, 10.f, fac::enemy); // Find the closest target

				if (actor->target_ent != BUF_NULL) // If its not null
				{
					actor->pathnodes.clear();

					env::node_coord current(ENTITY(actor->target_ent)->t.cellx, ENTITY(actor->target_ent)->t.celly);
					while (current.x != actor->t.cellx || current.y != actor->t.celly) // If the tile is not the one I'm standing on
					{
						actor->pathnodes.push_back(current);
						//current = env::get_node_from(actor->t.cellx, actor->t.celly, current.x, current.y);
					}
					if (actor->pathnodes.size() > 0) // Failsafe against trying to path to where it already is standing
					{
						actor->haspath = true;
						actor->node = (btui32)actor->pathnodes.size() - 1ui32; // bad don't use vector stupid baka
					}
				}
			}

			float distance = 0.f;
			float distance_to_target = 0.f;
			if (actor->pathnodes.size() > 0) // Failsafe against trying to path to where it already is standing
			{
				distance = m::Length(m::Vector2(actor->pathnodes[actor->node].x, actor->pathnodes[actor->node].y) - actor->t.position); // replace 0 with target id
			}
			distance_to_target = m::Length(ENTITY(actor->target_ent)->t.position - actor->t.position);

			actor->inputbv.unset(Actor::in_atk);

			if (distance_to_target < 4.f) // if enemy is close enough to shoot at
			{
				//compute rotation
				float offset = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), ENTITY(actor->target_ent)->t.position - actor->t.position);
				float forwards = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg())), ENTITY(actor->target_ent)->t.position - actor->t.position);

				if (forwards < 0.f && offset < 0.4f && offset > -0.4f) // If moving backwards,
				{ // And the rotation amount to walk backwards to the target is small
					mov = -1.f; // Walk backwards
					if (offset < 0.1f) actor->viewYaw.Rotate(5.f);
					else if (offset > -0.1f) actor->viewYaw.Rotate(-5.f);
				}
				else // Else, navigate normally (forwards)
				{
					mov = 1.f; // Walk forwards
					if (offset > 0.1f) actor->viewYaw.Rotate(5.f);
					else if (offset < -0.1f) actor->viewYaw.Rotate(-5.f);
					if (offset > 0.4f || offset < -0.4f) mov = 0.f; // If the angle difference is too great, stop moving forward but keep turning
				}

				//actor->input.y = mov;
				actor->input.y = 0.f;
				actor->input.x = 0.f;
				actor->inputbv.set(Actor::in_atk);
			}
			else if (actor->node != 0 && distance > 0.5f)
			{
				//compute rotation
				float offset = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg() + 90.f)), m::Vector2(actor->pathnodes[actor->node].x, actor->pathnodes[actor->node].y) - actor->t.position);
				float forwards = m::Dot(m::AngToVec2(glm::radians(actor->viewYaw.Deg())), m::Vector2(actor->pathnodes[actor->node].x, actor->pathnodes[actor->node].y) - actor->t.position);

				if (forwards < 0.f && offset < 0.4f && offset > -0.4f) // If moving backwards,
				{ // And the rotation amount to walk backwards to the target is small
					mov = -1.f; // Walk backwards
					if (offset < 0.1f) actor->viewYaw.Rotate(5.f);
					else if (offset > -0.1f) actor->viewYaw.Rotate(-5.f);
				}
				else // Else, navigate normally (forwards)
				{
					mov = 1.f; // Walk forwards
					if (offset > 0.1f) actor->viewYaw.Rotate(5.f);
					else if (offset < -0.1f) actor->viewYaw.Rotate(-5.f);
					if (offset > 0.4f || offset < -0.4f) mov = 0.f; // If the angle difference is too great, stop moving forward but keep turning
				}

				actor->input.y = mov;
				actor->input.x = 0.f;
			}
			else if (actor->node != 0)
			{
				--actor->node;
			}
			else
			{
				actor->haspath = false;
				actor->input.y = 0.f;
				actor->input.x = 0.f;
			}
		}
	}

	void EntCheckDeath(btID start, btID end)
	{
		for (btID i = start; i < end; i++)
		{
			if (ENTITY(i)->state.hp <= 0.f) // Death
				ENTITY(i)->state.properties.unset(ActiveState::eALIVE);
		}
	}

	void EntDeintersect(Entity* ent, CellSpaceInfo& csi, btf32 rot, bool knockback)
	{
		btf32 offsetx, offsety;
		bool overlapN, overlapS, overlapE, overlapW, touchNS = false, touchEW = false;

		//if (actor::moving[i]) // Add if moving check?

		//******************************** ENVIRONMENTAL COLLISION CHECK

		offsetx = ent->t.position.x - ent->t.cellx;
		offsety = ent->t.position.y - ent->t.celly;

		overlapN = offsety > 0;
		overlapS = offsety < 0;
		overlapE = offsetx > 0;
		overlapW = offsetx < 0;

		ent->freeTurn = true;

		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eSurfN) && overlapN) // N
		{
			ent->t.position.y = ent->t.celly; // + (1 - radius)
			touchNS = true;
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eSurfS) && overlapS) // S
		{
			ent->t.position.y = ent->t.celly; // - (1 - radius)
			touchNS = true;
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eSurfE) && overlapE) // E
		{
			ent->t.position.x = ent->t.cellx; // + (1 - radius)
			touchEW = true;
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eSurfW) && overlapW) // W
		{
			ent->t.position.x = ent->t.cellx; // - (1 - radius)
			touchEW = true;
		}

		// effects all entities for some reason
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

		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eCorOutNE) && overlapN && overlapE) // NE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eCorOutNW) && overlapN && overlapW) // NW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eCorOutSE) && overlapS && overlapE) // SE
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
		if (env::Get(ent->t.cellx, ent->t.celly, env::eflag::eCorOutSW) && overlapS && overlapW) // SW
		{
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}

		//******************************** ACTOR COLLISION CHECK

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
	}

	void RemoveAllReferences(btID index)
	{
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			// If used, not me, and is targeting me
			if (block_entity.used[i] && i != index && CHARA(i)->target_ent == index) // and type is entity?
				CHARA(i)->target_ent = BUF_NULL; // Reset it's target
		}
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

	btID GetClosestEntityWeighted(btID index, btf32 dist, fac::facalleg weight)
	{
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= block_entity.index_end; i++)
		{
			if (block_entity.used[i] && i != index && ENTITY(i)->state.properties.get(ActiveState::eALIVE))
			{
				// Calculate the distance between this entity and the other
				btf32 check_distance = m::Length(ENTITY(i)->t.position - ENTITY(index)->t.position);
				// Weight
				if (check_distance < dist)
				{
					if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(i)->faction) == weight) // do I like THEM
					{
						check_distance *= 0.5f;
					}
				}
				// Is this closer than the last one we looked at?
				if (check_distance < closest_distance)
				{
					current_closest = i;
					closest_distance = check_distance;
				}
			}
		}
		return current_closest;
	}

	inline void spawn_setup_t(btID index, m::Vector2 pos, btf32 dir)
	{
		ePos = pos;
		eYaw2.Set(dir);
		GetCellSpaceInfo(ePos, eCSI);
		env::GetHeight(eHgt, eCSI);

		for (int i = 0; i < 4; i++) // Add me to all cells I'm touching
			AddEntityCell(eCSI.c[i].x, eCSI.c[i].y, i);
	}

	void prefab_pc(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		CHARA(id)->t_skin = res::t_skin1;
		CHARA(id)->aiControlled = false;
		#ifdef DEF_EDITOR
		CHARA(id)->speed = 5.f;
		#else
		CHARA(id)->speed = 1.3f;
		//CHARA(id)->speed = 2.f;
		//CHARA(id)->speed = 0.25f;
		//CHARA(id)->speed = 8.f;
		#endif // DEF_EDITOR
	}

	void prefab_aipc(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::player;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = res::t_skin1;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 1.3f;
	}

	void prefab_npc(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::playerhunter;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = res::t_skin2;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 0.7f;
	}

	void prefab_zombie(btID id, m::Vector2 pos, btf32 dir)
	{
		ENTITY(id) = new Chara();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::undead;
		ENTITY(id)->properties.set(Entity::ePREFAB_FULLSOLID);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		CHARA(id)->t_skin = res::t_gui_bar_red;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 0.35f;
	}

	void(*PrefabEntity[])(btID, m::Vector2, btf32) = { prefab_pc, prefab_aipc, prefab_npc, prefab_zombie };

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- CELL STUFF -----------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	enum gcg_dir : btui8 { eN = 1u, eE = 2u };

	// Order is always: This -> X -> Y -> Diagonal

	void GetCgNE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[0].x = x; cc[0].y = y;               // This
		cc[1].x = x + 1ui8; cc[1].y = y;        // E (X)
		cc[2].x = x; cc[2].y = y + 1ui8;        // N (Y)
		cc[3].x = x + 1ui8; cc[3].y = y + 1ui8; // NE
	};
	void GetCgNW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[0].x = x; cc[0].y = y;               // This
		cc[1].x = x - 1ui8; cc[1].y = y;        // W (X)
		cc[2].x = x; cc[2].y = y + 1ui8;        // N (Y)
		cc[3].x = x - 1ui8; cc[3].y = y + 1ui8; // NW
	};
	void GetCgSE(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[0].x = x; cc[0].y = y;               // This
		cc[1].x = x + 1ui8; cc[1].y = y;        // E (X)
		cc[2].x = x; cc[2].y = y - 1ui8;        // S (Y)
		cc[3].x = x + 1ui8; cc[3].y = y - 1ui8; // SE
	};
	void GetCgSW(btcoord x, btcoord y, CellCoord* cc)
	{
		cc[0].x = x; cc[0].y = y;               // This
		cc[1].x = x - 1ui8; cc[1].y = y;        // W (X)
		cc[2].x = x; cc[2].y = y - 1ui8;        // S (Y)
		cc[3].x = x - 1ui8; cc[3].y = y - 1ui8; // SW
	};

	void(*GetCg[])(btcoord, btcoord, CellCoord*) = { GetCgSW, GetCgNW, GetCgSE, GetCgNE };

	void GetCellGroup(m::Vector2 vec, CellGroup& cg)
	{
		btui8 dir = 0ui8; // Represents direction of offset

		btcoord x = (btcoord)roundf(vec.x); // X cell coordinate
		btcoord y = (btcoord)roundf(vec.y); // Y cell coordinate

		dir = eN * (vec.y - y > 0.f);  // Set direction bit N
		dir |= eE * (vec.x - x > 0.f); // Add direction bit E

		GetCg[dir](x, y, cg.c); // Get cell group from direction
	}

	void GetCellSpaceInfo(m::Vector2 vec, CellSpaceInfo& csi)
	{
		btui8 dir = 0ui8; // Represents direction of offset

		csi.c[0u].x = (btcoord)roundf(vec.x); // X cell coordinate
		csi.c[0u].y = (btcoord)roundf(vec.y); // Y cell coordinate

		csi.offsety = vec.y - csi.c[0u].y; // X offset
		csi.offsetx = vec.x - csi.c[0u].x; // Y offset

		dir = eN * (csi.offsety > 0.f);  // Set direction bit N
		dir |= eE * (csi.offsetx > 0.f); // Add direction bit E

		GetCg[dir](csi.c[0u].x, csi.c[0u].y, csi.c); // Get cell group from direction
	}
}