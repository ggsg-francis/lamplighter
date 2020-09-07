// Included in core

#include "core_decl.hpp"

namespace core
{
	//________________________________________________________________________________________________________________________________
	// GENERAL FUNCTIONS -------------------------------------------------------------------------------------------------------------

	void RemoveAllReferences(btID index)
	{
		// Remove us from the cell we're on
		refCells[ENTITY(index)->t.csi.c[eCELL_I].x][ENTITY(index)->t.csi.c[eCELL_I].y].ref_ents.Remove(index);
		// Remove all references to us by other entities
		for (int i = 0; i <= GetLastEntity(); i++)
		{
			if (GetEntityExists(i) && i != index) // If entity exists and is not me
			{
				if (GetEntityType(i) == ENTITY_TYPE_ACTOR) // and is actor
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

	bool LOSCheck(btID enta, btID entb) {
		ECCommon* entity_a = ENTITY(enta);
		ECCommon* entity_b = ENTITY(entb);
		#if DEF_GRID
		return env::LineTraceBh(
			entity_a->t.csi.c[eCELL_I].x, entity_a->t.csi.c[eCELL_I].y,
			entity_b->t.csi.c[eCELL_I].x, entity_b->t.csi.c[eCELL_I].y,
			entity_a->t.height, entity_b->t.height);
		#else
		return env::LineTrace(
			entity_a->t.position.x, entity_a->t.position.y,
			entity_b->t.position.x, entity_b->t.position.y,
			entity_a->t.height, entity_b->t.height);
		#endif
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
		for (int i = 0; i <= GetLastEntity(); i++)
		{
			// If used, not me, and is alive
			if (GetEntityExists(i) && i != index && ENTITY(i)->state.stateFlags.get(ActiveState::eALIVE))
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
		for (int index_other = 0; index_other <= GetLastEntity(); index_other++)
		{
			// If used, not me, and is alive
			if (GetEntityExists(index_other) && index_other != index && ENTITY(index_other)->state.stateFlags.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(ENTITY(index)->faction, ENTITY(index_other)->faction) == allegiance)
				{
					btf32 check_distance = m::Length(ENTITY(index_other)->t.position - ENTITY(index)->t.position);
					if (check_distance < dist)
					{
						ECCommon* ent = ENTITY(index);
						ECCommon* ent_other = ENTITY(index_other);
						// LINE TRACE
						if (env::LineTraceBh(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y,
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
		for (int i = 0; i <= GetLastEntity(); i++)
		{
			// If used, not me, and is alive
			if (GetEntityExists(i) && i != index && ENTITY(i)->state.stateFlags.get(ActiveState::eALIVE))
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
		ECCommon* entity_index = ENTITY(index);
		btID current_closest = BUF_NULL;
		btf32 closest_distance = dist; // Effectively sets a max return range
		for (int i = 0; i <= GetLastEntity(); i++)
		{
			ECCommon* entity = ENTITY(i);
			// If used, not me, and is alive
			if (GetEntityExists(i) && i != index && entity->state.stateFlags.get(ActiveState::eALIVE))
			{
				// do I like THEM
				if (fac::GetAllegiance(entity_index->faction, entity->faction) == allegiance)
				{
					btf32 check_distance = m::Length(entity->t.position - entity_index->t.position);
					if (check_distance < closest_distance)
					{
						// Linetrace environment to see if the character is visible
						#if DEF_GRID
						if (env::LineTraceBh(
							entity_index->t.csi.c[eCELL_I].x, entity_index->t.csi.c[eCELL_I].y,
							entity->t.csi.c[eCELL_I].x, entity->t.csi.c[eCELL_I].y,
							entity_index->t.height, entity->t.height))
						#else
						if (!env::LineTrace(
							entity_index->t.position.x, entity_index->t.position.y,
							entity->t.position.x, entity->t.position.y,
							entity_index->t.height, entity->t.height))
						#endif
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

	btID GetClosestEntityButDifferent(btID index)
	{
		btID id = BUF_NULL;
		btf32 closestDist = 2.f;
		btf32 closest_angle = 20.f;

		ECCommon* entity_index = ENTITY(index);
		// Iterate through nearby cells
		for (int x = entity_index->t.csi.c[eCELL_I].x - 3u; x < entity_index->t.csi.c[eCELL_I].x + 3u; x++)
		{
			for (int y = entity_index->t.csi.c[eCELL_I].y - 3u; y < entity_index->t.csi.c[eCELL_I].y + 3u; y++)
			{
				// Iterate through every entity space in this cell
				for (int e = 0; e <= refCells[x][y].ref_ents.Size(); e++)
				{
					//if (cells[x][y].ents[e] != ID_NULL && block_entity.used[cells[x][y].ents[e]] && ENTITY(cells[x][y].ents[e])->Type() == Entity::eITEM)
					if (refCells[x][y].ref_ents[e] != ID_NULL && refCells[x][y].ref_ents[e] != index && GetEntityExists(refCells[x][y].ref_ents[e]))
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
		env::GetNearestSurfaceHeight(ENTITY(index)->t.height, ENTITY(index)->t.csi, ENTITY(index)->t.height);
		AddEntityCell(ENTITY(index)->t.csi.c[eCELL_I].x, ENTITY(index)->t.csi.c[eCELL_I].y, index);
		ENTITY(index)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(index)->state.damagestate = STATE_DAMAGE_MAX;
		ENTITY(index)->radius = 0.15f;
		ENTITY(index)->height = 0.7f;
		if (GetEntityType(index) == ENTITY_TYPE_ACTOR)
		{
			ACTOR(index)->atk_target = BUF_NULL;
			ACTOR(index)->ai_target_ent = BUF_NULL;
			ACTOR(index)->ai_ally_ent = BUF_NULL;

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

			ACTOR(index)->skin_col_a = col_base_a;
			ACTOR(index)->skin_col_b = col_eyes;
			ACTOR(index)->skin_col_c = col_base_b;
		}
	}

	char Capitalize(char c)
	{
		if (c >= 'a' && c <= 'z')
			return c + ('A' - 'a');
		return c;
	}
	void NameEntity(btID id)
	{
		char name1[32];
		char name2[32];

		btui32 random;
		btui32 usePrefix = (btui32)m::Random(0u, 100u);

		btui32 prefix_percent = 5u;

		if (usePrefix < prefix_percent) { // X% chance of having a prefix
			random = (btui32)roundf(m::Random(0u, TEMP_NAME_PREFIX_COUNT));
			strcpy(name1, TemplatePrefixes[random]);
		}
		else {
			random = (btui32)roundf(m::Random(0, TEMP_NAME_COUNT));
			strcpy(name1, TemplateNames[random]);
		}

		btui32 remaining_space = 31 - strlen(name1);

		// if there's enough space for a second name
		if (remaining_space > 8) {
		gen2ndname:
			if (usePrefix < prefix_percent) { // X% chance of having a prefix
				random = (btui32)roundf(m::Random(0, TEMP_NAME_COUNT));
				strcpy(name2, TemplateNames[random]);
			}
			else {
				random = (btui32)roundf(m::Random(0, TEMP_NAME_FAMILY_COUNT));
				strcpy(name2, TemplateFamilyNames[random]);
			}
			// if second name is short enough to fit
			if (strlen(name2) < remaining_space) {
				strcat(name1, " "); // add space
				strcat(name1, name2); // add second name
				strcpy((char*)ACTOR(id)->name, name1);
			}
			else goto gen2ndname;
		}
		// Name too long to fit a second, so just roll with it
		strcpy((char*)ACTOR(id)->name, name1);
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
		spawn_setup_t(id, pos, dir);
		NameEntity(id);
		ENTITY(id)->properties.set(ECCommon::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		ACTOR(id)->aiControlled = false;
		ACTOR(id)->speed = 1.45f; // default
		//ACTOR(id)->speed = 2.5f; // quake
		ACTOR(id)->agility = 0.f;

		//ACTOR(id)->inventory.AddNew(6u); // sword
		//ACTOR(id)->inventory.AddNew(5u); // annoying bug

		ACTOR(id)->foot_state = ECActor::FootState::eL_DOWN;
	}

	void prefab_aipc(btID id, m::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		NameEntity(id);
		ENTITY(id)->faction = fac::faction::player;
		ENTITY(id)->properties.set(ECCommon::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ACTOR(id)->aiControlled = true;
		ACTOR(id)->speed = 1.45f;
		ACTOR(id)->agility = 0.f;
		//ACTOR(id)->inventory.AddNew(6u);
		ACTOR(id)->foot_state = ECActor::FootState::eL_DOWN;
	}

	void prefab_npc(btID id, m::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		NameEntity(id);
		ENTITY(id)->faction = fac::faction::playerhunter;
		ENTITY(id)->properties.set(ECCommon::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ACTOR(id)->aiControlled = true;
		ACTOR(id)->speed = 1.45f;
		ACTOR(id)->agility = 0.f;
		ACTOR(id)->inventory.AddNew(3u); // gun
		ACTOR(id)->inventory.AddNew(4u); // ammo
		ACTOR(id)->inventory.AddNew(4u); // ammo
		ACTOR(id)->inventory.AddNew(4u); // ammo
		ACTOR(id)->inventory.AddNew(4u); // ammo
		ACTOR(id)->foot_state = ECActor::FootState::eL_DOWN;
		//ACTOR(id)->inventory.items[ACTOR(id)->inv_active_slot];
		//TODO: clean up this mess
	}

	void prefab_zombie(btID id, m::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		NameEntity(id);
		ENTITY(id)->faction = fac::faction::undead;
		ENTITY(id)->properties.set(ECCommon::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ACTOR(id)->aiControlled = true;
		ACTOR(id)->speed = 3.5f;
		ACTOR(id)->agility = 0.f;
		ACTOR(id)->inventory.AddNew(4u);
		ACTOR(id)->foot_state = ECActor::FootState::eL_DOWN;
	}

	void prefab_editorpawn(btID id, m::Vector2 pos, btf32 dir)
	{
		spawn_setup_t(id, pos, dir);
		NameEntity(id);
		ENTITY(id)->properties.set(ECCommon::ePREFAB_FULLSOLID);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->faction = fac::faction::player;
		ACTOR(id)->aiControlled = false;
		ACTOR(id)->speed = 0.1f;
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