// For use in index.cpp only

#include "index_decl.h"

namespace index
{
	inline Chara* GetChara(btID index)
	{
		return (Chara*)_entities[index];
	}

	// errrr
	#define CHARA GetChara
	#define CHARA2 _entities
	#define ENTITY _entities
	//#define CHARA_INDEX ((Chara*)_chara[index]) // set 1 underscore to test

	void EntRunAI(btID index)
	{
		f2Input.y = 1.f;
		f2Input.x = -1.f;

		//if (state == AUTO_STATE_FOLLOW)
		{
			float mov = 0.f;

			//find new path
			if (!aiHasPath)
			{
				aiTarget = GetClosestEntity(index, 5.f); // Find the closest target
				if (aiTarget != BUF_NULL && aiTarget != index) // If its not me, or null
				{
					aiPath.clear();

					env::node_coord current(ENTITY[aiTarget]->t.cellx, ENTITY[aiTarget]->t.celly);
					while (current.x != eCellX || current.y != eCellY) // If the tile is not the one I'm standing on
					{
						aiPath.push_back(current);
						current = env::get_node_from(eCellX, eCellY, current.x, current.y);
					}
					if (aiPath.size() > 0) // Failsafe against trying to path to where it already is standing
					{
						aiHasPath = true;
						aiNode = aiPath.size() - 1;
					}
				}
			}

			float distance = 0.f;
			if (aiPath.size() > 0) // Failsafe against trying to path to where it already is standing
			{
				distance = fw::Length(fw::Vector2(aiPath[aiNode].x, aiPath[aiNode].y) - ePos); // replace 0 with target id
			}

			if (aiNode != 0 && distance > 0.5f)
			{
				//compute rotation
				float offset = fw::Dot(fw::AngToVec2(glm::radians(aViewYaw.Deg() + 90.f)), fw::Vector2(aiPath[aiNode].x, aiPath[aiNode].y) - ePos);
				float forwards = fw::Dot(fw::AngToVec2(glm::radians(aViewYaw.Deg())), fw::Vector2(aiPath[aiNode].x, aiPath[aiNode].y) - ePos);

				if (forwards < 0.f && offset < 0.4f && offset > -0.4f) // If moving backwards,
				{ // And the rotation amount to walk backwards to the target is small
					mov = -1.f; // Walk backwards
					if (offset < 0.1f) aViewYaw.Rotate(5.f);
					else if (offset > -0.1f) aViewYaw.Rotate(-5.f);
				}
				else // Else, navigate normally (forwards)
				{
					mov = 1.f; // Walk forwards
					if (offset > 0.1f) aViewYaw.Rotate(5.f);
					else if (offset < -0.1f) aViewYaw.Rotate(-5.f);
					if (offset > 0.4f || offset < -0.4f) mov = 0.f; // If the angle difference is too great, stop moving forward but keep turning
				}

				f2Input.y = mov;
				f2Input.x = 0.f;
			}
			else if (aiNode != 0)
			{
				--aiNode;
			}
			else
			{
				aiHasPath = false;
				f2Input.y = 0.f;
				f2Input.x = 0.f;
			}
		}
	}

	void EntCheckDeath(btID start, btID end)
	{
		for (btID i = start; i < end; i++)
		{
			if (ENTITY[i]->state.hp <= 0.f) // Death
				ENTITY[i]->state.alive = false;
		}
	}

	void EntDeintersect(btID start, btID end)
	{
		btf32 offsetx, offsety;
		bool overlapN, overlapS, overlapE, overlapW;
		cell_group cg;

		for (btID index = start; index < end; index++)
		{
			//if (actor::moving[i]) // Add if moving check?

			//******************************** ENVIRONMENTAL COLLISION CHECK

			offsetx = ePos.x - eCellX;
			offsety = ePos.y - eCellY;

			overlapN = offsety > 0;
			overlapS = offsety < 0;
			overlapE = offsetx > 0;
			overlapW = offsetx < 0;

			if (env::Get(eCellX, eCellY, env::eflag::eSurfN) && overlapN) // N
			{
				ePos.y = eCellY; // + (1 - radius)
				if (aViewYaw.Deg() < 180.f)
					eYaw.RotateTowards(90.f, rotdeg);
				else
					eYaw.RotateTowards(270.f, rotdeg);
			}
			if (env::Get(eCellX, eCellY, env::eflag::eSurfS) && overlapS) // S
			{
				ePos.y = eCellY; // - (1 - radius)
				if (aViewYaw.Deg() < 180.f)
					eYaw.RotateTowards(90.f, rotdeg);
				else
					eYaw.RotateTowards(270.f, rotdeg);
			}
			if (env::Get(eCellX, eCellY, env::eflag::eSurfE) && overlapE) // E
			{
				ePos.x = eCellX; // + (1 - radius)
				if (aViewYaw.Deg() > 90.f && aViewYaw.Deg() < 270.f)
					eYaw.RotateTowards(180.f, rotdeg);
				else
					eYaw.RotateTowards(0.f, rotdeg);
			}
			if (env::Get(eCellX, eCellY, env::eflag::eSurfW) && overlapW) // W
			{
				ePos.x = eCellX; // - (1 - radius)
				if (aViewYaw.Deg() > 90.f && aViewYaw.Deg() < 270.f)
					eYaw.RotateTowards(180.f, rotdeg);
				else
					eYaw.RotateTowards(0.f, rotdeg);
			}

			if (env::Get(eCellX, eCellY, env::eflag::eCorOutNE) && overlapN && overlapE) // NE
			{
				fw::Vector2 offset = fw::Vector2(offsetx, offsety) - fw::Vector2(0.5f, 0.5f);
				if (fw::Length(offset) < 0.5f)
					ePos += fw::Normalize(offset) * (0.5f - fw::Length(offset));
			}
			if (env::Get(eCellX, eCellY, env::eflag::eCorOutNW) && overlapN && overlapW) // NW
			{
				fw::Vector2 offset = fw::Vector2(offsetx, offsety) - fw::Vector2(-0.5f, 0.5f);
				if (fw::Length(offset) < 0.5f)
					ePos += fw::Normalize(offset) * (0.5f - fw::Length(offset));
			}
			if (env::Get(eCellX, eCellY, env::eflag::eCorOutSE) && overlapS && overlapE) // SE
			{
				fw::Vector2 offset = fw::Vector2(offsetx, offsety) - fw::Vector2(0.5f, -0.5f);
				if (fw::Length(offset) < 0.5f)
					ePos += fw::Normalize(offset) * (0.5f - fw::Length(offset));
			}
			if (env::Get(eCellX, eCellY, env::eflag::eCorOutSW) && overlapS && overlapW) // SW
			{
				fw::Vector2 offset = fw::Vector2(offsetx, offsety) - fw::Vector2(-0.5f, -0.5f);
				if (fw::Length(offset) < 0.5f)
					ePos += fw::Normalize(offset) * (0.5f - fw::Length(offset));
			}

			//******************************** ACTOR COLLISION CHECK

			///*

			cg = GetCollisionCells(ePos);

			for (int cell_group = 0; cell_group < 4; cell_group++)
			{
				//de-intersect entities
				if (cells[cg.c[cell_group].x][cg.c[cell_group].y].ents.size() > 0)
				{
					for (int e = 0; e < cells[cg.c[cell_group].x][cg.c[cell_group].y].ents.size(); e++)
					{
						#define ID cells[cg.c[cell_group].x][cg.c[cell_group].y].ents[e]
						if (block_entity.used[ID] && ENTITY[ID]->state.alive)
						{
							fw::Vector2 vec = ePos - ENTITY[ID]->t.position;
							float dist = fw::Length(vec);
							if (dist < 0.8f && dist > 0.f)
							{
								ePos += fw::Normalize(vec) * (0.8f - dist);
								//massively slow collide callback (we'll speed it up later k?)
								collision::hit_info hit;
								hit.depenetrate = fw::Normalize(vec) * (0.8f - dist);
								hit.hit = true;
								hit.surface = fw::Normalize(vec);
								hit.inheritedVelocity = ENTITY[ID]->t.velocity;
								//entity->Collide(hit);
								collision::hit_info hit2;
								hit2.depenetrate = fw::Normalize(vec * -1.f) * (0.8f - dist);
								hit2.hit = true;
								hit2.surface = fw::Normalize(vec * -1.f);
								hit2.inheritedVelocity = eVel;

								if (resAniStep.Flag(res::f::inflict_knockback))
								{
									ENTITY[index]->t.velocity = hit2.surface * -0.1f; // set their velocity
									CHARA(index)->ani_Lower.setAnim(res::knockback);
									ENTITY[ID]->t.velocity = hit2.surface * 0.2f; // set their velocity
									CHARA(ID)->ani_Lower.setAnim(res::knockback);
								}
							}
						}
						#undef ID
					} // End for each entity in cell
				} // End if entity count of this cell is bigger than zero
			} // End for each cell group

			//*/

			//******************************** C2 COLLISION CHECK

			c2Poly poly;
			poly.count = 4;
			//poly.verts[0] = c2V(-1.f, -1.f);
			//poly.verts[1] = c2V(1.f, -1.f);
			//poly.verts[2] = c2V(1.f, 1.f);
			//poly.verts[3] = c2V(-1.f, 1.f);
			// diamond shape
			poly.verts[0] = c2V(-0.5f, 0.f);
			poly.verts[1] = c2V(0.f, -0.5f);
			poly.verts[2] = c2V(0.5f, 0.f);
			poly.verts[3] = c2V(0.f, 0.5f);
			c2MakePoly(&poly);

			c2Circle circle;
			circle.p = c2V(ePos.x, ePos.y);
			circle.r = 0.5f;

			c2Circle circle2;
			circle2.p = c2V(5.f, 3.f);
			circle2.r = 0.5f;

			c2x pos;
			pos.p = c2V(2.f, 2.f);
			pos.r = c2RotIdentity();
			c2Manifold manifold;

			//c2CircletoCircleManifold(circle, circle2, &manifold);
			//if (c2CircletoCircle(circle, circle2))
			//{
			//	if (manifold.count > 0)
			//	{
			//		/*float ofx = tPos.x - manifold.contact_points[0].x;
			//		float ofy = tPos.y - manifold.contact_points[0].y;*/
			//		float ofx = manifold.n.x * -manifold.depths[0];
			//		float ofy = manifold.n.y * -manifold.depths[0];
			//		tPos += fw::Vector2(ofx, ofy);
			//	}
			//}

			c2CircletoPolyManifold(circle, &poly, &pos, &manifold);
			if (c2CircletoPoly(circle, &poly, &pos))
			{
				if (manifold.count > 0)
				{
					float ofx = manifold.n.x * -manifold.depths[0];
					float ofy = manifold.n.y * -manifold.depths[0];
					ePos += fw::Vector2(ofx, ofy);
				}
			}
		}
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

	btID GetClosestPlayer(btID index, btf32 dist)
	{
		btf32 check_distance_0 = fw::Length(ENTITY[players[0]]->t.position - ENTITY[index]->t.position);
		btf32 check_distance_1 = fw::Length(ENTITY[players[1]]->t.position - ENTITY[index]->t.position);
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
			if (block_entity.used[i] && i != index && ENTITY[i]->state.alive)
			{
				btf32 check_distance = fw::Length(ENTITY[i]->t.position - ENTITY[index]->t.position);
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
			if (block_entity.used[i] && i != index && ENTITY[i]->state.alive)
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY[index]->faction, ENTITY[i]->faction) == allegiance)
				{
					btf32 check_distance = fw::Length(ENTITY[i]->t.position - ENTITY[index]->t.position);
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
			if (block_entity.used[i] && i != index && ENTITY[i]->state.alive)
			{
				// Calculate the distance between this entity and the other
				btf32 check_distance = fw::Length(ENTITY[i]->t.position - ENTITY[index]->t.position);
				// Weight
				if (check_distance < dist)
				{
					if (fac::GetAllegiance(ENTITY[index]->faction, ENTITY[i]->faction) == weight) // do I like THEM
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

	inline void spawn_setup_t(btID index, fw::Vector2 pos, btf32 dir)
	{
		ePos = pos;
		eYaw.Set(dir);
	}

	void prefab_pc(btID id, fw::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		ENTITY[id]->faction = fac::faction::player;
		CHARA(id)->aiControlled = false;
		#ifdef DEF_EDITOR
		CHARA(id)->speed = 5.f;
		#else
		CHARA(id)->speed = 1.3f;
		#endif // DEF_EDITOR
	}

	void prefab_npc(btID id, fw::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		ENTITY[id]->faction = fac::faction::playerhunter;
		CHARA(id)->aiControlled = true;
		CHARA(id)->speed = 0.7f;
	}

	void prefab_item_bb(btID id, fw::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		ENTITY[id]->faction = fac::faction::none;
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- CELL STUFF (TO BE REVISED) -------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	enum gcg_dir : btui8 { n = 1u, e = 2u };

	cell_group gcg_ne(btui8 x, btui8 y)
	{
		cell_group cg;
		cg.c[0] = cellcoord(x, y);			// this
		cg.c[1] = cellcoord(x, y + 1);		// n
		cg.c[2] = cellcoord(x + 1, y);		// e
		cg.c[3] = cellcoord(x + 1, y + 1);	// ne
		return cg;
	};
	cell_group gcg_nw(btui8 x, btui8 y)
	{
		cell_group cg;
		cg.c[0] = cellcoord(x, y);			// this
		cg.c[1] = cellcoord(x, y + 1);		// n
		cg.c[2] = cellcoord(x - 1, y);		// w
		cg.c[3] = cellcoord(x - 1, y + 1);	// nw
		return cg;
	};
	cell_group gcg_se(btui8 x, btui8 y)
	{
		cell_group cg;
		cg.c[0] = cellcoord(x, y);			// this
		cg.c[1] = cellcoord(x, y - 1);		// s
		cg.c[2] = cellcoord(x + 1, y);		// e
		cg.c[3] = cellcoord(x + 1, y - 1);	// se
		return cg;
	};
	cell_group gcg_sw(btui8 x, btui8 y)
	{
		cell_group cg;
		cg.c[0] = cellcoord(x, y);			// this
		cg.c[1] = cellcoord(x, y - 1);		// s
		cg.c[2] = cellcoord(x - 1, y);		// w
		cg.c[3] = cellcoord(x - 1, y - 1);	// sw
		return cg;
	};

	// Function pointers for the various offsets necessary for collision checking
	cell_group(*gcg_fn[])(btui8, btui8) = { gcg_sw, gcg_nw, gcg_se, gcg_ne };

	// Get the group of 4 cells applicable to [vector] position
	cell_group GetCollisionCells(fw::Vector2 vec)
	{
		btui8 dir = 0; // Represents fast direction of offset (char for speed)
		btui8 x = (btui8)roundf(vec.x); // X cell coordinate
		btui8 y = (btui8)roundf(vec.y); // Y cell coordinate
		if (vec.y - y > 0.f)	// Check North offset direction from center
			dir = n;			// Set direction bit N
		if (vec.x - x > 0.f)	// Check East offset direction from center
			dir |= e;			// Add direction bit E
		return gcg_fn[dir](x, y); // Return calls the function pointer corresponding to DIR
	}
}