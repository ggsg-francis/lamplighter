#include "core.h"
#include "core_func.hpp"
#include "weather.h"

#include "index.h"

#include "core_save_load.h"

#include "collision.h"

btui64 tickCount;

namespace core
{
	btui16 GetHP(btID id)
	{
		return ENTITY(id)->state.damagestate;
	}

	void SetViewTargetID(btID id, btui32 player)
	{
		viewtarget_last_tick[player] = viewtarget[player];
		viewtarget[player] = id;
		//std::cout << "looking at id " << viewtarget << std::endl;
	}
	btID GetViewTargetID(btui32 player)
	{
		return viewtarget[player];
	}

	void SetViewFocus(btID index)
	{
		activePlayer = index;
		Actor* chara = ACTOR(players[activePlayer]);
		graphics::SetMatView(&viewTarget[activePlayer], &viewPosition[activePlayer], nullptr);
	}

	enum flood_fill_type
	{
		r_ambient,
		g_light,
		b_unused,
		a_unused,
	};

	void flood_fill_temp(btui16 srcx, btui16 srcy, btui16 x, btui16 y, graphics::colour col)
	{
		//t_EnvLightmap.SetPixelChannelG(x, y, col.g);
		if (!env::Get(x, y, env::eflag::EF_IMPASSABLE) && t_EnvLightmap.GetPixel(x, y).g < col.g)
		{
			if (env::eCells.terrain_height[x][y] < env::eCells.terrain_height[srcx][srcy] + 8u //incline check
			&& env::eCells.terrain_height[x][y] > env::eCells.terrain_height[srcx][srcy] - 12u) // decline check
			{
				t_EnvLightmap.SetPixelChannelG(x, y, col.g);
				if (col.g > 31u)
				{
					col.g -= 32u;
					flood_fill_temp(srcx, srcy, x + 1, y, col);
					flood_fill_temp(srcx, srcy, x - 1, y, col);
					flood_fill_temp(srcx, srcy, x, y + 1, col);
					flood_fill_temp(srcx, srcy, x, y - 1, col);
				}
			}
		}
	}

	GLuint shadowtex;
	glm::mat4 shadowmat_temp;
	void SetShadowTexture(btui32 id)
	{
		shadowtex = id;
	}

	void UpdateGlobalShaderParams()
	{
		for (int i = graphics::S_UTIL_FIRST_LIT; i <= graphics::S_UTIL_LAST_LIT; ++i)
		{
			graphics::GetShader((graphics::eShader)i).Use();
			graphics::GetShader((graphics::eShader)i).SetTexture(graphics::Shader::texLightMap, t_EnvLightmap.glID, graphics::Shader::TXTR_LIGHTMAP);
			graphics::GetShader((graphics::eShader)i).SetTexture(graphics::Shader::texSkyMap, acv::GetT(acv::t_sky).glID, graphics::Shader::TXTR_SKY);
		}
	}

	void UpdateOtherShaderParams(btf32 time2, m::Vector3 sunVec)
	{
		for (int i = graphics::S_UTIL_FIRST_LIT; i <= graphics::S_UTIL_LAST_LIT; ++i)
		{
			graphics::GetShader((graphics::eShader)i).Use();
			graphics::GetShader((graphics::eShader)i).setMat4(graphics::Shader::matLightProj, *(graphics::Matrix4x4*)&shadowmat_temp);
			graphics::GetShader((graphics::eShader)i).SetFloat(graphics::Shader::fTime, (float)time2);
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::vecPCam, (glm::vec3)graphics::GetViewPos());
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::vecVSun, (glm::vec3)sunVec);
			graphics::GetShader((graphics::eShader)i).SetTexture(graphics::Shader::texShadowMap, shadowtex, graphics::Shader::TXTR_SHADOWMAP);
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::Colour_Ambient, *(glm::vec3*)weather::AmbientColour());
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::Colour_Sunlight, *(glm::vec3*)weather::SunColour());
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::Colour_Fog, *(glm::vec3*)weather::FogColour());
			graphics::GetShader((graphics::eShader)i).SetFloat(graphics::Shader::Fog_Density, *(float*)weather::FogDensity());
		}
	}

	void DoSpawn()
	{
		#ifdef DEF_SPAWN_NPC

		// for every entity
		/*
		for (int e = 0; e < block_entity.index_end; e++)
		{
			if (block_entity.used[e])
			{
				if (!ENTITY(e)->state.stateFlags.get(ActiveState::eALIVE)
					&& !ENTITY(e)->state.stateFlags.get(ActiveState::eDIED_REPORT)
					&& ACTOR(e)->aiControlled)
				{
					m::Vector2 pos1 = ENTITY(players[0])->t.position;
					m::Vector2 pos2 = ENTITY(players[1])->t.position;
					m::Vector2 pos3 = ENTITY(e)->t.position;
					if (m::Length(pos1 - pos3) > 12.f && m::Length(pos2 - pos3) > 12.f) // only destroy if far away
						DestroyEntity(e);
				}
				if (EntityType(e) == ENTITY_TYPE_RESTING_ITEM)
				{
					m::Vector2 pos1 = ENTITY(players[0])->t.position;
					m::Vector2 pos2 = ENTITY(players[1])->t.position;
					m::Vector2 pos3 = ENTITY(e)->t.position;
					if (m::Length(pos1 - pos3) > 12.f && m::Length(pos2 - pos3) > 12.f) // only destroy if far away
						DestroyEntity(e);
				}
			}
		}//*/

		// temp
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			for (int y = 0; y < WORLD_SIZE; ++y)
			{
				if (env::Get(x, y, env::eflag::EF_SPAWN_TEST))
				{
					btf32 dist;
					m::Vector2 pos1 = ENTITY(players[0])->t.position;
					m::Vector2 pos2 = ENTITY(players[1])->t.position;
					m::Vector2 pos3 = m::Vector2(x, y);
					//if (m::Length(pos1 - pos3) > 8.f && m::Length(pos2 - pos3) > 8.f) // Don't spawn too close to either player
					//if (LOSCheck(ENTITY(players[0])->id, )) // Don't spawn too close to either player
					{
						//if (m::Length(pos1 - pos3) < 64.f && m::Length(pos2 - pos3) < 64.f) // ..or too far away
						{
							#ifdef DEF_SPAWN_ONLY_ENEMIES
							SpawnEntity(prefab::prefab_npc, m::Vector2(x, y), 0.f);
							#else
							btf32 random = m::Random(0.f, 15.f);
							btui32 rand_rnd = (btui32)floor(random);
							if (rand_rnd < 2u)
								SpawnEntity(prefab::prefab_ai_player, m::Vector2(x, y), 0.f);
							else if (rand_rnd < 9u)
								SpawnEntity(prefab::prefab_npc, m::Vector2(x, y), 0.f);
							else
								SpawnEntity(prefab::prefab_zombie, m::Vector2(x, y), 0.f);//*/
							#endif
						}
					}
				}
			}
		}

		#endif

		// spawn items
		for (int x = 0; x < WORLD_SIZE; ++x) {
			for (int y = 0; y < WORLD_SIZE; ++y) {
				if (env::Get(x, y, env::eflag::EF_SPAWN_ITEM_TEST)) {
					SpawnNewEntityItem(env::eCells.spawn_id[x][y], m::Vector2(x,y), 0.f);
				}
			}
		}
	}

	void Init()
	{
		IndexInitialize();

		// Generate debug version display
		char buffinal[32];
		sprintf(buffinal, "TSOA R%i-%i [%s]", VERSION_MAJOR, VERSION_MINOR, VERSION_COMMENT);
		text_version.ReGen(buffinal, cfg::iWinX * -0.25f, cfg::iWinX * 0.25f, cfg::iWinY * 0.5f);

		env::LoadBin();

		//-------------------------------- Lightmap

		t_EnvLightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(255u, 0u, 0u, 255u));
		for (btui32 x = 0u; x < WORLD_SIZE; ++x)
		{
			for (btui32 y = 0u; y < WORLD_SIZE; ++y)
			{
				t_EnvLightmap.SetPixelChannelA(x, y, env::eCells.terrain_height[x][y]);
				if (env::Get(x, y, env::eflag::EF_LIGHTSRC))
					flood_fill_temp(x, y, x, y, graphics::colour(0u, 255u, 0u, 0u));
			}
		}
		t_EnvLightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		//-------------------------------- Heightmap

		t_EnvHeightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(0u, 0u, 0u, 0u));

		for (btui16 x = 0; x < WORLD_SIZE; x++)
			for (btui16 y = 0; y < WORLD_SIZE; y++)
				t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells.terrain_height[x][y]);

		t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		//-------------------------------- Spawnz

		if (!cfg::bEditMode) {
			if (SaveExists()) LoadState();
			else {
				SpawnActivator(1024u, 1024u);

				players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
				players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(1023.f, 1022.f), 0.f);
				// test npc
				//SpawnEntity(prefab::prefab_ai_player, m::Vector2(1025.f, 1025.f), 0.f);
				#ifndef DEF_PLAYERS_ALLIED
				// PVP - align p2 with player hunter faction
				ENTITY(players[1])->faction = fac::playerhunter;
				#endif
				#ifdef DEF_SPAWN_ON_START
				DoSpawn();
				#endif
				#ifdef DEF_AUTOSAVE_ON_START
				SaveState();
				#endif
			}
		}

		//-------------------------------- factions

		// This is going to blow up extremely fast if I don't automate it somehow

		fac::SetAllegiance(fac::player, fac::player, fac::allied);
		fac::SetAllegiance(fac::player, fac::playerhunter, fac::enemy);
		fac::SetAllegiance(fac::player, fac::undead, fac::enemy);

		fac::SetAllegiance(fac::undead, fac::player, fac::enemy);
		fac::SetAllegiance(fac::undead, fac::playerhunter, fac::enemy);
		fac::SetAllegiance(fac::undead, fac::undead, fac::allied);

		fac::SetAllegiance(fac::playerhunter, fac::undead, fac::enemy);
		fac::SetAllegiance(fac::playerhunter, fac::player, fac::enemy);
		fac::SetAllegiance(fac::playerhunter, fac::playerhunter, fac::allied);

		graphics::SetMatProj(); // does not need to be continually called
		UpdateGlobalShaderParams();
	}
	void End()
	{
		ClearBuffers();
		env::Free();
	}

	//destroy all data
	void ClearBuffers()
	{
		for (int i = 0; i < BUF_SIZE; i++)
			//for (int i = 0; i <= block_entity.index_end; i++)
		{
			if (GetEntityExists(i))
			{
				IndexFreeEntity(i);
				//block_entity.remove(i);
			}
			if (ItemInstanceExists(i))
			{
				FreeItemInstance(i);
				//ObjBuf_remove(&block_item, i);
			}
		}
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			for (int y = 0; y < WORLD_SIZE; ++y)
			{
				refCells[x][y].ref_ents.Clear();
			}
		}
	}

	void RegenCellRefs()
	{
		for (int i = 0; i <= GetLastEntity(); i++) {
			Entity* ent = (Entity*)GetEntityPtr(i);
			AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, i);
		}
	}

	//btui64 spawnz_time_temp = 240u * 30u;
	btui64 spawnz_time_temp = 30 * 30u;

	void Tick(btf32 dt)
	{
		// TODO: think this over, there must be a better way to handle this
		#ifdef DEF_AUTO_RELOAD_ON_DEATH // Don't reload if it's multiplayer, this might be a temporary measure anyway
		// check if either player is dead
		if (!ENTITY(players[0])->state.stateFlags.get(ActiveState::eALIVE) || !ENTITY(players[1])->state.stateFlags.get(ActiveState::eALIVE))
		{
			// Load the last save state
			LoadState();
		}
		#endif

		#ifdef DEF_PERIODIC_SPAWN
		/*if (!cfg::bEditMode && spawnz_time_temp < tickCount_temp)
		{
			spawnz_time_temp = tickCount_temp + 30u * 30u;
			DoSpawn();
		}*/
		--spawnz_time_temp;
		if (!cfg::bEditMode && spawnz_time_temp == 1u)
		{
			// seconds * fps
			//spawnz_time_temp = 120u * 30u;
			spawnz_time_temp = 30u * 30u;
			DoSpawn();
		}
		#endif

		//temporary destroy dead entities
		///*
		for (int i = 0; i <= GetLastEntity(); i++)
			if (GetEntityExists(i))
				if (!ENTITY(i)->state.stateFlags.get(ActiveState::eALIVE)
					&& !ENTITY(i)->state.stateFlags.get(ActiveState::eDIED_REPORT))
				{
					//index::DestroyEntity(i);
					ENTITY(i)->properties.unset(Entity::EntityFlags::eCOLLIDE_ENT);
					ENTITY(i)->properties.unset(Entity::EntityFlags::eCOLLIDE_PRJ);
					ENTITY(i)->properties.unset(Entity::EntityFlags::eCOLLIDE_MAG);
				}
		//*/

		//-------------------------------- ITERATE THROUGH ENTITIES

		for (btID i = 0; i <= GetLastEntity(); i++) // For every entity
		{
			if (GetEntityExists(i))
			{
				ENTITY(i)->state.TickEffects(dt);
				fpTick[GetEntityType(i)](i, ENTITY(i), dt); // Call tick on entity
			}
		}

		//-------------------------------- SOME OTHER SHIT

		SetViewTargetID(GetClosestEntityButDifferent(players[0u]), 0u);
		SetViewTargetID(GetClosestEntityButDifferent(players[1u]), 1u);

		ProjectileTick(dt);

		#ifndef DEF_NMP
		if (cfg::bEditMode)
		{
			Entity* entity = ENTITY(0);

			if (input::GetHeld(input::key::RUN)) {
				editor_cam_pitch.RotateClamped(input::buf.mouse_y * 20.f * dt, 5.f, 85.f);
				editor_cam_yaw.Rotate(input::buf.mouse_x * 10.f * dt);
			}
			else {
				m::Vector2 vec = m::AngToVec2(editor_cam_yaw.Rad());
				editor_cursor +=
					((m::Vector2(vec.y, -vec.x) * input::buf.mouse_x)
						+ (vec * -input::buf.mouse_y)) * 0.35f * dt;
			}
			
			bti32 editor_cursor_x = roundf(editor_cursor.x);
			bti32 editor_cursor_y = roundf(editor_cursor.y);

			btf32 editor_cursor_offset_x = editor_cursor.x - (btf32)editor_cursor_x;
			btf32 editor_cursor_offset_y = editor_cursor.y - (btf32)editor_cursor_y;

			#define GetCellX editor_cursor_x
			#define GetCellY editor_cursor_y
			#define GetOffsX editor_cursor_offset_x
			#define GetOffsY editor_cursor_offset_y
			
			if (input::GetHit(input::key::USE))
			{
				++env::eCells.terrain_material[GetCellX][GetCellY];
				if (env::eCells.terrain_material[GetCellX][GetCellY] > 7u)
					env::eCells.terrain_material[GetCellX][GetCellY] = 0u;
				env::GenerateTerrainMeshEditor();
			}
			else if (input::GetHeld(input::key::USE_ALT))
			{
				env::eCells.terrain_material[GetCellX][GetCellY] = editor_material_copy;
				env::GenerateTerrainMeshEditor();
			}
			else if (input::GetHit(input::key::ACTIVATE))
			{
				//env::GeneratePhysicsSurfaces();
				//env::SaveBin();	
			}
			else if (input::GetHeld(input::key::DIR_F)) // Forward
				env::eCells.prop_dir[GetCellX][GetCellY] = env::eNORTH;
			else if (input::GetHeld(input::key::DIR_B)) // Back
				env::eCells.prop_dir[GetCellX][GetCellY] = env::eSOUTH;
			else if (input::GetHeld(input::key::DIR_R)) // Right
				env::eCells.prop_dir[GetCellX][GetCellY] = env::eEAST;
			else if (input::GetHeld(input::key::DIR_L)) // Left
				env::eCells.prop_dir[GetCellX][GetCellY] = env::eWEST;
			else if (input::GetHit(input::key::FUNCTION_1)) // COPY
			{
				editor_flags_copy = env::eCells.flags[GetCellX][GetCellY];
				editor_prop_copy = env::eCells.prop[GetCellX][GetCellY];
				editor_prop_dir_copy = env::eCells.prop_dir[GetCellX][GetCellY];
				editor_height_copy_ne = env::eCells.terrain_height_ne[GetCellX][GetCellY];
				editor_height_copy_nw = env::eCells.terrain_height_nw[GetCellX][GetCellY];
				editor_height_copy_se = env::eCells.terrain_height_se[GetCellX][GetCellY];
				editor_height_copy_sw = env::eCells.terrain_height_sw[GetCellX][GetCellY];
				editor_material_copy = env::eCells.terrain_material[GetCellX][GetCellY];
			}
			else if (input::GetHeld(input::key::FUNCTION_2)) // PASTE
			{
				env::eCells.flags[GetCellX][GetCellY] = (env::eflag::flag)editor_flags_copy;
				env::eCells.prop[GetCellX][GetCellY] = editor_prop_copy;
				env::eCells.prop_dir[GetCellX][GetCellY] = editor_prop_dir_copy;
				env::eCells.terrain_height_ne[GetCellX][GetCellY] = editor_height_copy_ne;
				env::eCells.terrain_height_nw[GetCellX][GetCellY] = editor_height_copy_nw;
				env::eCells.terrain_height_se[GetCellX][GetCellY] = editor_height_copy_se;
				env::eCells.terrain_height_sw[GetCellX][GetCellY] = editor_height_copy_sw;
				env::eCells.terrain_material[GetCellX][GetCellY] = editor_material_copy;
				env::GenerateTerrainMesh();
			}
			else if (input::GetHit(input::key::FUNCTION_3)) // TOGGLE LIGHT
			{
				env::Get(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC)
					? env::UnSet(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC)
					: env::Set(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC);
			}
			else if (input::GetHit(input::key::FUNCTION_4)) // TOGGLE SPAWN
			{
				if (env::Get(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST)) {
					env::UnSet(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST);
					env::Set(GetCellX, GetCellY, env::eflag::EF_SPAWN_ITEM_TEST);
				}
				else if (env::Get(GetCellX, GetCellY, env::eflag::EF_SPAWN_ITEM_TEST)) {
					env::UnSet(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST);
					env::UnSet(GetCellX, GetCellY, env::eflag::EF_SPAWN_ITEM_TEST);
				}
				else {
					env::Set(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST);
				}
			}
			else if (input::GetHit(input::key::FUNCTION_5)) // SAVE
			{
				env::SaveBin();
			}
			else if (input::GetHit(input::key::FUNCTION_6)) // TOGGLE SPAWN INDEX
			{
				if (env::Get(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST)) {
					++env::eCells.spawn_id[GetCellX][GetCellY];
					// inactive because we don't have a proper archive setup for entity templates
					//if (env::eCells.spawn_id[GetCellX][GetCellY] >= acv::actor_template_index)
					//	env::eCells.spawn_id[GetCellX][GetCellY] = 0u;
				}
				else if (env::Get(GetCellX, GetCellY, env::eflag::EF_SPAWN_ITEM_TEST)) {
					++env::eCells.spawn_id[GetCellX][GetCellY];
					if (env::eCells.spawn_id[GetCellX][GetCellY] >= acv::item_index)
						env::eCells.spawn_id[GetCellX][GetCellY] = 0u;
				}
			}
			else if (input::GetHit(input::key::ACTION_A))
			{
				env::Get(GetCellX, GetCellY, env::eflag::EF_IMPASSABLE)
					? env::UnSet(GetCellX, GetCellY, env::eflag::EF_IMPASSABLE)
					: env::Set(GetCellX, GetCellY, env::eflag::EF_IMPASSABLE);
			}
			else if (input::GetHit(input::key::ACTION_B))
			{
				if (env::eCells.prop[GetCellX][GetCellY] > 0u)
					--env::eCells.prop[GetCellX][GetCellY];
			}
			else if (input::GetHit(input::key::ACTION_C))
			{
				if (env::eCells.prop[GetCellX][GetCellY] < acv::prop_index)
					++env::eCells.prop[GetCellX][GetCellY];
			}
			else if (input::GetHit(input::key::FUNCTION_7))
			{
				env::Get(GetCellX, GetCellY, env::eflag::EF_INVISIBLE)
					? env::UnSet(GetCellX, GetCellY, env::eflag::EF_INVISIBLE)
					: env::Set(GetCellX, GetCellY, env::eflag::EF_INVISIBLE);
				env::GenerateTerrainMesh();
			}
			else if (input::GetHit(input::key::INV_CYCLE_R))
			{
				if (input::GetHeld(input::key::RUN))
				{
					++env::eCells.terrain_height_ne[GetCellX][GetCellY];
					++env::eCells.terrain_height_nw[GetCellX][GetCellY];
					++env::eCells.terrain_height_se[GetCellX][GetCellY];
					++env::eCells.terrain_height_sw[GetCellX][GetCellY];
				}
				else if (input::GetHeld(input::key::CROUCH))
				{
					if (GetOffsX > 0.f && GetOffsY > 0.f) { // NE
						if (env::eCells.terrain_height_nw[GetCellX + 1][GetCellY] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							++env::eCells.terrain_height_nw[GetCellX + 1][GetCellY];
						if (env::eCells.terrain_height_se[GetCellX][GetCellY + 1] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							++env::eCells.terrain_height_se[GetCellX][GetCellY + 1];
						if (env::eCells.terrain_height_sw[GetCellX + 1][GetCellY + 1] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							++env::eCells.terrain_height_sw[GetCellX + 1][GetCellY + 1];
						++env::eCells.terrain_height_ne[GetCellX][GetCellY];
					}
					else if (GetOffsY > 0.f) { // NW
						if (env::eCells.terrain_height_ne[GetCellX - 1][GetCellY] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							++env::eCells.terrain_height_ne[GetCellX - 1][GetCellY];
						if (env::eCells.terrain_height_sw[GetCellX][GetCellY + 1] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							++env::eCells.terrain_height_sw[GetCellX][GetCellY + 1];
						if (env::eCells.terrain_height_se[GetCellX - 1][GetCellY + 1] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							++env::eCells.terrain_height_se[GetCellX - 1][GetCellY + 1];
						++env::eCells.terrain_height_nw[GetCellX][GetCellY];
					}
					else if (GetOffsX > 0.f) { // SE
						if (env::eCells.terrain_height_sw[GetCellX + 1][GetCellY] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							++env::eCells.terrain_height_sw[GetCellX + 1][GetCellY];
						if (env::eCells.terrain_height_ne[GetCellX][GetCellY - 1] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							++env::eCells.terrain_height_ne[GetCellX][GetCellY - 1];
						if (env::eCells.terrain_height_nw[GetCellX + 1][GetCellY - 1] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							++env::eCells.terrain_height_nw[GetCellX + 1][GetCellY - 1];
						++env::eCells.terrain_height_se[GetCellX][GetCellY];
					}
					else { // SW
						if (env::eCells.terrain_height_se[GetCellX - 1][GetCellY] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							++env::eCells.terrain_height_se[GetCellX - 1][GetCellY];
						if (env::eCells.terrain_height_nw[GetCellX][GetCellY - 1] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							++env::eCells.terrain_height_nw[GetCellX][GetCellY - 1];
						if (env::eCells.terrain_height_ne[GetCellX - 1][GetCellY - 1] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							++env::eCells.terrain_height_ne[GetCellX - 1][GetCellY - 1];
						++env::eCells.terrain_height_sw[GetCellX][GetCellY];
					}
				}
				else
				{
					if (GetOffsX > 0.f && GetOffsY > 0.f)
						++env::eCells.terrain_height_ne[GetCellX][GetCellY];
					else if (GetOffsY > 0.f)
						++env::eCells.terrain_height_nw[GetCellX][GetCellY];
					else if (GetOffsX > 0.f)
						++env::eCells.terrain_height_se[GetCellX][GetCellY];
					else
						++env::eCells.terrain_height_sw[GetCellX][GetCellY];
				}
				t_EnvHeightmap.SetPixelChannelR(GetCellX, GetCellY, env::eCells.terrain_height[GetCellX][GetCellY]);
				t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
				env::GenerateTerrainMeshEditor();
			}
			else if (input::GetHit(input::key::INV_CYCLE_L))
			{
				if (input::GetHeld(input::key::RUN))
				{
					--env::eCells.terrain_height_ne[GetCellX][GetCellY];
					--env::eCells.terrain_height_nw[GetCellX][GetCellY];
					--env::eCells.terrain_height_se[GetCellX][GetCellY];
					--env::eCells.terrain_height_sw[GetCellX][GetCellY];
				}
				else if (input::GetHeld(input::key::CROUCH))
				{
					if (GetOffsX > 0.f && GetOffsY > 0.f) { // NE
						if (env::eCells.terrain_height_nw[GetCellX + 1][GetCellY] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							--env::eCells.terrain_height_nw[GetCellX + 1][GetCellY];
						if (env::eCells.terrain_height_se[GetCellX][GetCellY + 1] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							--env::eCells.terrain_height_se[GetCellX][GetCellY + 1];
						if (env::eCells.terrain_height_sw[GetCellX + 1][GetCellY + 1] == env::eCells.terrain_height_ne[GetCellX][GetCellY])
							--env::eCells.terrain_height_sw[GetCellX + 1][GetCellY + 1];
						--env::eCells.terrain_height_ne[GetCellX][GetCellY];
					}
					else if (GetOffsY > 0.f) { // NW
						if (env::eCells.terrain_height_ne[GetCellX - 1][GetCellY] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							--env::eCells.terrain_height_ne[GetCellX - 1][GetCellY];
						if (env::eCells.terrain_height_sw[GetCellX][GetCellY + 1] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							--env::eCells.terrain_height_sw[GetCellX][GetCellY + 1];
						if (env::eCells.terrain_height_se[GetCellX - 1][GetCellY + 1] == env::eCells.terrain_height_nw[GetCellX][GetCellY])
							--env::eCells.terrain_height_se[GetCellX - 1][GetCellY + 1];
						--env::eCells.terrain_height_nw[GetCellX][GetCellY];
					}
					else if (GetOffsX > 0.f) { // SE
						if (env::eCells.terrain_height_sw[GetCellX + 1][GetCellY] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							--env::eCells.terrain_height_sw[GetCellX + 1][GetCellY];
						if (env::eCells.terrain_height_ne[GetCellX][GetCellY - 1] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							--env::eCells.terrain_height_ne[GetCellX][GetCellY - 1];
						if (env::eCells.terrain_height_nw[GetCellX + 1][GetCellY - 1] == env::eCells.terrain_height_se[GetCellX][GetCellY])
							--env::eCells.terrain_height_nw[GetCellX + 1][GetCellY - 1];
						--env::eCells.terrain_height_se[GetCellX][GetCellY];
					}
					else { // SW
						if (env::eCells.terrain_height_se[GetCellX - 1][GetCellY] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							--env::eCells.terrain_height_se[GetCellX - 1][GetCellY];
						if (env::eCells.terrain_height_nw[GetCellX][GetCellY - 1] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							--env::eCells.terrain_height_nw[GetCellX][GetCellY - 1];
						if (env::eCells.terrain_height_ne[GetCellX - 1][GetCellY - 1] == env::eCells.terrain_height_sw[GetCellX][GetCellY])
							--env::eCells.terrain_height_ne[GetCellX - 1][GetCellY - 1];
						--env::eCells.terrain_height_sw[GetCellX][GetCellY];
					}
				}
				else
				{
					if (GetOffsX > 0.f && GetOffsY > 0.f)
						--env::eCells.terrain_height_ne[GetCellX][GetCellY];
					else if (GetOffsY > 0.f)
						--env::eCells.terrain_height_nw[GetCellX][GetCellY];
					else if (GetOffsX > 0.f)
						--env::eCells.terrain_height_se[GetCellX][GetCellY];
					else
						--env::eCells.terrain_height_sw[GetCellX][GetCellY];
				}
				t_EnvHeightmap.SetPixelChannelR(GetCellX, GetCellY, env::eCells.terrain_height[GetCellX][GetCellY]);
				t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
				env::GenerateTerrainMeshEditor();
			}

			#undef GetCellX
			#undef GetCellY
			#undef GetOffsX
			#undef GetOffsY
		}
		else
		#endif
		{
			#ifdef DEF_NMP
			for (btui32 i = 0; i < NUM_PLAYERS; ++i)
			{
				if (input::GetHit(i, input::key::FUNCTION_5)) // SAVE
				{
					SaveState();
					break;
				}
				else if (input::GetHit(i, input::key::FUNCTION_9)) // LOAD
				{
					if (SaveExists())
						LoadState();
					break;
				}
			}
			#else
			if (input::GetHit(input::key::FUNCTION_5)) // SAVE
			{
				SaveState();
			}
			else if (input::GetHit(input::key::FUNCTION_9)) // LOAD
			{
				if (SaveExists())
					LoadState();
			}
			#endif
		}

		//-------------------------------- Modify camera

		if (cfg::bEditMode) {

			GetCellSpaceInfo(editor_cursor, editor_cursorCS);

			btf32 height;
			env::GetNearestSurfaceHeight(height, editor_cursorCS, 1000000.f);

			editor_cursor_height = m::Lerp(editor_cursor_height, height, 1.5f * dt);

			viewTarget[0] = m::Vector3(editor_cursor.x, editor_cursor_height, editor_cursor.y);

			btf32 r = 3.5f;
			btf32 pitch_x = cos(editor_cam_pitch.Rad());
			btf32 pitch_y = sin(editor_cam_pitch.Rad());
			btf32 yaw_x = cos(editor_cam_yaw.Rad());
			btf32 yaw_y = sin(editor_cam_yaw.Rad());
			viewPosition[0] = viewTarget[0] + r * m::Vector3(-yaw_y * pitch_x, pitch_y, -yaw_x * pitch_x);
		}
		else {

			#ifdef DEF_3PP

			m::Vector3 target_a = m::Vector3(ENTITY(players[0])->t.position.x, ENTITY(players[0])->t.height + 0.8f, ENTITY(players[0])->t.position.y);
			m::Vector3 target_b = m::Vector3(ENTITY(players[1])->t.position.x, ENTITY(players[1])->t.height + 0.8f, ENTITY(players[1])->t.position.y);

			m::Vector2 dir_a = (m::AngToVec2(ENTITY(players[0])->t.yaw.Rad()) + m::AngToVec2(ACTOR(players[0])->viewYaw.Rad())) * 0.5f;
			m::Vector2 dir_b = (m::AngToVec2(ENTITY(players[1])->t.yaw.Rad()) + m::AngToVec2(ACTOR(players[1])->viewYaw.Rad())) * 0.5f;

			m::Vector3 position_a = target_a - (m::Vector3(dir_a.x, -0.2f, dir_a.y)) * 3.f;
			m::Vector3 position_b = target_b - (m::Vector3(dir_b.x, -0.2f, dir_b.y)) * 3.f;

			if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y))
			{
				position_a = m::Lerp(position_a, target_a, 0.5f);
				if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y))
				{
					position_a = m::Lerp(position_a, target_a, 0.75f);
				}
			}

			// Snap if the difference between now and the target is too great
			if (m::Length(viewTarget[0] - target_a) > 10.f || m::Length(viewPosition[0] - position_a) > 10.f) {
				viewTarget[0] = target_a;
				viewPosition[0] = position_a;
			}
			// Move camera gradually
			else {
				viewTarget[0] = m::Lerp(viewTarget[0], target_a, 0.15f);
				viewPosition[0] = m::Lerp(viewPosition[0], position_a, 0.05f);
			}
			// Snap if the difference between now and the target is too great
			if (m::Length(viewTarget[1] - target_b) > 10.f || m::Length(viewPosition[1] - position_b) > 10.f) {
				viewTarget[1] = target_b;
				viewPosition[1] = position_b;
			}
			// Move camera gradually
			else {
				viewTarget[1] = m::Lerp(viewTarget[1], target_b, 0.15f);
				viewPosition[1] = m::Lerp(viewPosition[1], position_b, 0.05f);
			}

			#else

			viewPosition[0] = ACTOR(0)->t_head.GetPosition() + m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), ACTOR(0)->t_head.GetRotation());
			viewPosition[1] = ACTOR(1)->t_head.GetPosition() + m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), ACTOR(1)->t_head.GetRotation());

			viewTarget[0] = viewPosition[0] + ACTOR(0)->t_head.GetForward();
			viewTarget[1] = viewPosition[1] + ACTOR(1)->t_head.GetForward();

			#endif // !DEF_3PP
		}

		//-------------------------------- Stuff

		++tickCount;
	}

	void Draw(bool oob)
	{
		btf32 time2 = ((btf32)tickCount / 30.f) * 0.02f + 0.2f;
		//btf32 time2 = 0.27f;
		//btf32 time2 = 0.22f;

		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 16.f) / 16.f) + 180.f));
		//m::Vector2 sunrot = m::AngToVec2(glm::radians(time2 * 360.f + 180.f));
		//glm::vec3 sunrot2 = glm::vec3(sunrot.x, sunrot.y, 0.f);
		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(sunrot.x, sunrot.y, sunrot.y * 1.2f));

		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(sunrot.x * 0.25f, 1.f, sunrot.x * 0.25f));
		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(0.25f, 1.f, 0.25f));
		m::Vector3 sunVec = m::Normalize(m::Vector3(0.5f, 1.f, 0.5f));
		//m::Vector3 sunVec = m::Vector3(0.f, 1.f, 0.f);

		graphics::Matrix4x4 matrix; // Matrix used for rendering env. props (so far...)

		graphics::SetFrontFace(); // reset front face (is set wrong in other places because i'm an idiota)

		if (oob)
		{
			//-------------------------------- UPDATE SHADERS

			UpdateOtherShaderParams(time2, sunVec);

			//-------------------------------- DRAW SKY

			graphics::SetMatProj();

			//-------------------------------- DRAW OOB TERRAIN

			//graphics::SetMatProj(128.f); // Set projection matrix for long-distance rendering

			// draw any OOB stuff here

			//graphics::SetMatProj(); // Reset projection matrix

			//-------------------------------- DRAW ENTITIES

			glClear(GL_DEPTH_BUFFER_BIT); // Clear for foreground rendering

			// ...
		}
		else
		{
			m::Vector3 lightPos(
				ENTITY(players[activePlayer])->t.position.x,
				ENTITY(players[activePlayer])->t.height,
				-ENTITY(players[activePlayer])->t.position.y);
			m::Vector3 lightVecForw(-sunVec.x, -sunVec.y, -sunVec.z);
			//m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 1.f, 0.f)));
			m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 0.f, 1.f)));
			m::Vector3 LightVecUp = m::Normalize(m::Cross(lightVecForw, LightVecSide));

			#define LIGHT_RND ((SHADOW_RESOLUTION / SHADOW_WIDTH) / 2)

			btf32 moveF = roundf(m::Dot(lightVecForw, lightPos));
			btf32 moveS = roundf(m::Dot(LightVecSide, lightPos) * LIGHT_RND) / LIGHT_RND;
			btf32 moveU = roundf(m::Dot(LightVecUp, lightPos) * LIGHT_RND) / LIGHT_RND;

			#undef LIGHT_RND

			lightPos = lightVecForw * moveF + LightVecSide * moveS + LightVecUp * moveU;

			// Set light matrices
			graphics::SetMatProjLight(); graphics::SetMatViewLight(
				lightPos.x, lightPos.y, lightPos.z,
				-sunVec.x, -sunVec.y, -sunVec.z);
			shadowmat_temp = graphics::GetMatProj() * graphics::GetMatView();
		}

		if (!cfg::bEditMode) if (oob) ProjectileDraw(); // Draw projectiles

		//-------------------------------- DRAW TERRAIN

		matrix = graphics::Matrix4x4();
		if (oob)
		{
			//graphics::MatrixTransform(matrix, m::Vector3(roundf(ENTITY(activePlayer)->t.position.x / 8) * 8, 0.f, roundf(ENTITY(activePlayer)->t.position.y / 8) * 8));
			//DrawMesh(ID_NULL, acv::GetM(acv::m_terrain_near), acv::GetT(acv::t_terrain_sanddirt), SS_TERRAIN, matrix);
		}

		//-------------------------------- DRAW ENTITIES AND PROPS

		#ifdef DEF_DRAW_WIREFRAME
		graphics::SetRenderWire();
		#endif // DEF_DRAW_WIREFRAME

		if (cfg::bEditMode)
		{
			bti32 editor_cursor_x = roundf(editor_cursor.x);
			bti32 editor_cursor_y = roundf(editor_cursor.y);
			graphics::MatrixTransform(matrix, m::Vector3(editor_cursor_x, env::eCells.terrain_height[editor_cursor_x][editor_cursor_y] / TERRAIN_HEIGHT_DIVISION, editor_cursor_y));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debugcell), acv::GetT(acv::t_gui_bar_red), SS_NORMAL, matrix);

			btui32 drawrange = 16u; // Create min/max draw coordinates
			bti32 minx = editor_cursor_x - drawrange; if (minx < 0) minx = 0;
			bti32 maxx = editor_cursor_x + drawrange; if (maxx > WORLD_SIZE_MAXINT) maxx = WORLD_SIZE_MAXINT;
			bti32 miny = editor_cursor_y - drawrange; if (miny < 0) miny = 0;
			bti32 maxy = editor_cursor_y + drawrange; if (maxy > WORLD_SIZE_MAXINT) maxy = WORLD_SIZE_MAXINT;
			for (bti32 x = minx; x <= maxx; x++)
			{
				for (bti32 y = miny; y < maxy; y++)
				{
					btf32 rotation_by_enum[]
					{
						0.f, 180.f, 90.f, 270.f,
					};
					matrix = graphics::Matrix4x4();

					if (env::Get(x, y, env::eflag::EF_IMPASSABLE))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_debug_bb), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_LIGHTSRC))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_default), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_SPAWN_TEST))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(ID_NULL, acv::GetM(acv::m_debug_monkey), acv::GetT(acv::t_col_red), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_SPAWN_ITEM_TEST))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(ID_NULL, acv::GetM(acv::items[env::eCells.spawn_id[x][y]]->id_mesh),
							acv::GetT(acv::items[env::eCells.spawn_id[x][y]]->id_tex), SS_NORMAL, matrix);
					}
					if (env::eCells.prop[x][y] == ID_NULL) env::eCells.prop[x][y] = 0u;
					//-------------------------------- DRAW ENVIRONMENT PROP ON THIS CELL
					if (env::eCells.prop[x][y] != ID_NULL && env::eCells.prop[x][y] > 0u)
					{
						graphics::MatrixTransform(matrix,
							m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, y),
							glm::radians(rotation_by_enum[env::eCells.prop_dir[x][y]]));
						DrawMesh(ID_NULL,
							acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
							acv::GetT(acv::props[env::eCells.prop[x][y]].idTxtr),
							SS_NORMAL, matrix);
					}
					/*if (env::eCells[x][y].prop != ID_NULL && env::eCells[x][y].prop > 0u)
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL,
							acv::GetM(acv::props[env::eCells[x][y].prop].idMesh),
							acv::GetT(acv::props[env::eCells[x][y].prop].idTxtr),
							SS_NORMAL, matrix);
					}*/
				}
			}
			env::DrawTerrainDebug();
		}
		else
		{
			// Draw debug ce;; display
			CellSpace* cs = &ENTITY(players[activePlayer])->t.csi;

			//graphics::MatrixTransform(matrix, m::Vector3(cs->c[eCELL_I].x,
			//	ENTITY(players[activePlayer])->t.height, cs->c[eCELL_I].y));
			//DrawMesh(ID_NULL, acv::GetM(acv::m_debugcell), acv::GetT(acv::t_gui_bar_red), SS_NORMAL, matrix);

			env::DrawDebugGizmos(cs);

			//*
			for (btID i = 0; i <= GetLastEntity(); i++) // For every entity
			{
				#ifdef DEF_FPP_INVISIBLE
				if (i != players[activePlayer] && block_entity.used[i])
				#else
				if (GetEntityExists(i))
				#endif
				{
					fpDraw[GetEntityType(i)](i, ENTITY(i)); // Call draw on entity
				}
			}
			//*/

			#define DRAWRANGE 16u
			/*
			Entity* entity = ENTITY(players[activePlayer]);
			// Set min/max draw coordinates
			bti32 minx = entity->t.csi.c[0].x - DRAWRANGE; if (minx < 0) minx = 0;
			bti32 maxx = entity->t.csi.c[0].x + DRAWRANGE; if (maxx > WORLD_SIZE_MAXINT) maxx = WORLD_SIZE_MAXINT;
			bti32 miny = entity->t.csi.c[0].y - DRAWRANGE; if (miny < 0) miny = 0;
			bti32 maxy = entity->t.csi.c[0].y + DRAWRANGE; if (maxy > WORLD_SIZE_MAXINT) maxy = WORLD_SIZE_MAXINT;
			for (bti32 x = minx; x <= maxx; x++) {
				for (bti32 y = miny; y < maxy; y++) {
					//-------------------------------- DRAW ENTITIES ON THIS CELL
					for (int e = 0; e <= refCells[x][y].ref_ents.end(); e++)
						if (refCells[x][y].ref_ents[e] != ID_NULL && GetEntityExists(refCells[x][y].ref_ents[e]))
							fpDraw[GetEntityType(refCells[x][y].ref_ents[e])](refCells[x][y].ref_ents[e], ENTITY(refCells[x][y].ref_ents[e])); // Call draw on entity
					//-------------------------------- DRAW ACTIVATOR ON THIS CELL
					//if (oob)
					{
						if (refCells[x][y].ref_activator != ID_NULL)
						{
							graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, y));
							DrawMesh(ID_NULL, acv::GetM(acv::m_debug_monkey), acv::GetT(acv::t_default), SS_NORMAL, matrix);
						}
					}
				}
			}
			//*/
			#ifndef DEF_SHADOW_ALL_OBJECTS
			if (oob)
			#endif
			{
				env::DrawProps();
				env::DrawTerrain();
			}
		}

		#ifdef DEF_DRAW_WIREFRAME
		graphics::SetRenderSolid();
		#endif // DEF_DRAW_WIREFRAME
	}

	void TickGUI()
	{
		// TODO: make this less shit
		#ifdef DEF_NMP
		for (btID i = 0u; i < NUM_PLAYERS; ++i)
		{
			if (input::GetHit(i, input::key::INV_CYCLE_L))
				ActorDecrEquipSlot(players[i]);
			if (input::GetHit(i, input::key::INV_CYCLE_R))
				ActorIncrEquipSlot(players[i]);
			if (input::GetHit(i, input::key::ACTIVATE)) // Pick up items
				if (viewtarget[i] != ID_NULL && GetEntityType(viewtarget[i]) == ENTITY_TYPE_RESTING_ITEM) {
					ActorTakeItem(players[i], viewtarget[i]);
				}
				else if (viewtarget[i] != ID_NULL && GetEntityType(viewtarget[i]) == ENTITY_TYPE_ACTOR)
				{
					// hold hand
					ActorTryHoldHand(players[i], viewtarget[i]);

					// SOUL TRANSFER
					// if we are allied
					/*
					if (fac::GetAllegiance(ENTITY(players[activePlayer])->faction, ENTITY(viewtarget[activePlayer])->faction) == fac::allied)
					{
					// SOUL TRANSFER
					if (players[0] != players[1]) // If both player's arent using the same entity
					{
					ACTOR(players[activePlayer])->aiControlled = true; // Let the AI take over
					ACTOR(players[activePlayer])->ai_target_ent = ID_NULL; // Let the AI take over
					ACTOR(players[activePlayer])->ai_ally_ent = ID_NULL; // Let the AI take over
					}
					players[activePlayer] = viewtarget[activePlayer]; // Set player to control player's view target
					ACTOR(players[activePlayer])->aiControlled = false;
					}
					*/
				}
			if (input::GetHit(i, input::key::DROP_HELD))
			{
				ActorDropItem(players[i], ACTOR(players[i])->inv_active_slot);
				//network::SendCharaInv(ACTOR(players[i]), players[i]); // TEMP FAILSAFE
			}
		}
		#else
		if (activePlayer == 0u)
		{
			if (input::GetHit(input::key::INV_CYCLE_L))
				ActorDecrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::INV_CYCLE_R))
				ActorIncrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::ACTIVATE)) // Pick up items
				if (viewtarget[activePlayer] != ID_NULL && GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_RESTING_ITEM)
					ActorTakeItem(players[activePlayer], viewtarget[activePlayer]);
				else if (viewtarget[activePlayer] != ID_NULL && GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_ACTOR)
				{
					// hold hand
					ActorTryHoldHand(players[activePlayer], viewtarget[activePlayer]);

					// SOUL TRANSFER
					// if we are allied
					/*
					if (fac::GetAllegiance(ENTITY(players[activePlayer])->faction, ENTITY(viewtarget[activePlayer])->faction) == fac::allied)
					{
						// SOUL TRANSFER
						if (players[0] != players[1]) // If both player's arent using the same entity
						{
							ACTOR(players[activePlayer])->aiControlled = true; // Let the AI take over
							ACTOR(players[activePlayer])->ai_target_ent = ID_NULL; // Let the AI take over
							ACTOR(players[activePlayer])->ai_ally_ent = ID_NULL; // Let the AI take over
						}
						players[activePlayer] = viewtarget[activePlayer]; // Set player to control player's view target
						ACTOR(players[activePlayer])->aiControlled = false;
					}
					*/
				}
			if (input::GetHit(input::key::DROP_HELD))
				ActorDropItem(players[activePlayer], ACTOR(players[activePlayer])->inv_active_slot);
		}
		else
		{
			if (input::GetHit(input::key::C_INV_CYCLE_L))
				ActorDecrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::C_INV_CYCLE_R))
				ActorIncrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::C_ACTIVATE)) // Pick up items
				if (viewtarget[activePlayer] != ID_NULL && GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_RESTING_ITEM)
					ActorTakeItem(players[activePlayer], viewtarget[activePlayer]);
				else if (viewtarget[activePlayer] != ID_NULL && GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_ACTOR)
				{
					// SOUL TRANSFER
					// Possession, actually
					// if we are allied
					if (fac::GetAllegiance(ENTITY(players[activePlayer])->faction, ENTITY(viewtarget[activePlayer])->faction) == fac::allied)
					{
						// begin soul transfer
						// If both players aren't controlling the same entity, let the AI take over
						if (players[0] != players[1])
							ACTOR(players[activePlayer])->aiControlled = true;
						// Set the player to possess their view target entity
						players[activePlayer] = viewtarget[activePlayer];
						// Disable the AI on the possessed entity
						ACTOR(players[activePlayer])->aiControlled = false;
					}
				}
			if (input::GetHit(input::key::C_DROP_HELD))
				ActorDropItem(players[activePlayer], ACTOR(players[activePlayer])->inv_active_slot);
		}
		#endif // DEF_NMP
	}

	void GUISetMessag(int player, char* string)
	{
		// Player 1, X Start
		int p1_x_start = -(int)graphics::FrameSizeX() / 2;
		int p1_y_start = -(int)graphics::FrameSizeY() / 2;
		text_message[player].ReGen(string, -52, 52, -32);
		message_time[player] = tickCount + 90u;
	}

	void DrawGUI()
	{
		// Set GL properties for transparant rendering
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Player 1, X Start
		int p1_x_start = -(int)graphics::FrameSizeX() / 2;
		int p1_y_start = -(int)graphics::FrameSizeY() / 2;

		//draw ui
		graphics::GetShader(graphics::S_GUI).Use();

		//TODO: use 'actor'?
		Actor* chara = ACTOR(players[activePlayer]);

		//text_message[activePlayer].ReGen("teststr", 0, -p1_x_start, 0);
		if (message_time[activePlayer] > tickCount)
			text_message[activePlayer].Draw(&acv::GetT(acv::t_gui_font));

		// hurt effect
		//graphics::DrawGUITexture(&acv::GetT(acv::t_gui_hurt), 0, 0, cfg::iWinX, cfg::iWinY);
		if (ENTITY(players[activePlayer])->state.damagestate < player_hp[activePlayer])
		{
			graphics::DrawGUITexture(&acv::GetT(acv::t_gui_hurt), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(),
				(btf32)(player_hp[activePlayer] - ENTITY(players[activePlayer])->state.damagestate) * (10.f / 1000.f));
			player_hp[activePlayer] -= 5u;
		}
		else
		{
			player_hp[activePlayer] = ENTITY(players[activePlayer])->state.damagestate;
		}
		#ifndef DEF_OLDSKOOL
		#ifndef DEF_3PP
		// croshair
		if (cfg::bCrossHairs) graphics::DrawGUITexture(&acv::GetT(acv::t_gui_crosshair), 0, 0, 32, 32);
		#endif
		#endif
		// hp
		graphics::DrawGUITexture(&acv::GetT(acv::t_gui_bar_red), p1_x_start + 32, p1_y_start + 8, (int)(((btf32)core::GetHP(players[activePlayer]) / 1000.f) * (64.f)), 16);
		char stuff[32];
		_itoa(ENTITY(players[activePlayer])->state.damagestate, stuff, 10);
		text_hp.ReGen(stuff, p1_x_start + 8, p1_x_start + 128, p1_y_start + 16);
		text_hp.Draw(&acv::GetT(acv::t_gui_font));

		// enemy hp
		if (viewtarget[activePlayer] != ID_NULL && viewtarget[activePlayer] != core::players[activePlayer]) // If not null or player
		{
			int textboxX = p1_x_start + 16;
			int textboxY = p1_y_start + 64;
			if (viewtarget[activePlayer] != viewtarget_last_tick[activePlayer]) // if target has changed
			{
				//text_temp.ReGen(ENTITY(viewtarget[activePlayer])->GetDisplayName(), textboxX, textboxX + 512, textboxY);
				//text_temp.ReGen(ENTITY(viewtarget[activePlayer])->fpName(ENT_VOID(viewtarget[activePlayer])), textboxX, textboxX + 512, textboxY);
				text_temp.ReGen(fpName[GetEntityType(viewtarget[activePlayer])](ENT_VOID(viewtarget[activePlayer])), textboxX, textboxX + 512, textboxY);
				guibox.ReGen(textboxX, textboxX + text_temp.sizex, textboxY - text_temp.sizey, textboxY, 4, 10);
			}
			if (GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_ACTOR)
				graphics::DrawGUITexture(&acv::GetT(acv::t_gui_bar_yellow), p1_x_start + 32, p1_y_start + 24, (int)(((btf32)core::GetHP(viewtarget[activePlayer]) / 1000.f) * (64.f)), 16);
			guibox.Draw(&acv::GetT(acv::t_gui_box));
			text_temp.Draw(&acv::GetT(acv::t_gui_font));
			if (GetEntityType(viewtarget[activePlayer]) == ENTITY_TYPE_RESTING_ITEM)
				graphics::DrawGUITexture(&acv::GetT(acv::t_gui_icon_pick_up), textboxX + 16, textboxY + 32, 32, 32);
		}
		// inventory
		chara->inventory.Draw(chara->inv_active_slot);
	}

	void DrawPostDraw(btf64 delta)
	{
		text_version.Draw(&acv::GetT(acv::t_gui_font));
		char buffer[16];
		int i = snprintf(buffer, 16, "%f", 1.f / delta);
		text_fps.ReGen(buffer, cfg::iWinX * -0.25f, cfg::iWinX * 0.25f, cfg::iWinY * 0.5f - 16.f);
		text_fps.Draw(&acv::GetT(acv::t_gui_font));

		//graphics::DrawGUITexture(&acv::GetT(acv::t_cursor), cursor_x, cursor_y, 128, 128);

		// draw archiver loaded list
		for (int i2 = 0; i2 < acv::AssetCount(); ++i2)
			if (acv::IsLoaded(i2))
				graphics::DrawGUITexture(&acv::GetT(acv::t_col_red), i2 * 2 - ((bti32)cfg::iWinX / 4), ((bti32)cfg::iWinY / 2) - 32, 2, 8);
			else
				graphics::DrawGUITexture(&acv::GetT(acv::t_col_black), i2 * 2 - ((bti32)cfg::iWinX / 4), ((bti32)cfg::iWinY / 2) - 32, 2, 8);
	}

	void SetPlayerInput(btID playerIndex, m::Vector2 input, btf32 rot_x, btf32 rot_y,
		bool use, bool use_hit, bool use_alt,
		bool run, bool aim, bool ACTION_A, bool ACTION_B, bool ACTION_C,
		bool crouch, bool jump)
	{
		Actor* actor = ACTOR(players[playerIndex]);
		actor->input = m::Rotate(input, actor->viewYaw.Rad()) * m::Vector2(-1.f, 1.f);
		actor->viewYaw.Rotate(rot_x);
		actor->viewPitch.RotateClamped(rot_y, -80.f, 70.f);
		actor->inputBV.setto(Actor::IN_USE, use);
		actor->inputBV.setto(Actor::IN_USE_HIT, use_hit);
		actor->inputBV.setto(Actor::IN_USE_ALT, use_alt);
		actor->inputBV.setto(Actor::IN_RUN, run);
		actor->inputBV.setto(Actor::IN_AIM, aim);
		actor->inputBV.setto(Actor::IN_ACTN_A, ACTION_A);
		actor->inputBV.setto(Actor::IN_ACTN_B, ACTION_B);
		actor->inputBV.setto(Actor::IN_ACTN_C, ACTION_C);
		actor->inputBV.setto(Actor::IN_CROUCH, crouch);
		actor->inputBV.setto(Actor::IN_JUMP, jump);
	}

	void AddEntityCell(btui32 x, btui32 y, btID e)
	{
		refCells[x][y].ref_ents.Add(e);
	}

	void RemoveEntityCell(btui32 x, btui32 y, btID e)
	{
		refCells[x][y].ref_ents.Remove(e);
	}

	//________________________________________________________________________________________________________________________________
	// SPAWN FUNCTIONS ---------------------------------------------------------------------------------------------------------------

	btID SpawnEntity(btui8 type, m::Vector2 pos, float dir)
	{
		btID id = InitEntity(ENTITY_TYPE_ACTOR);
		if (id != ID_NULL)
		{
			PrefabEntity[type](id, pos, dir);
		}
		else
		{
			std::cout << "Could not spawn entity, ran out of space" << std::endl;
		}
		return id;
	}
	btID SpawnNewEntityItem(btID item_template, m::Vector2 pos, btf32 dir)
	{
		btID id = InitEntity(ENTITY_TYPE_RESTING_ITEM);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::none;
		ENTITY(id)->properties.set(Entity::ePREFAB_ITEM);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ITEM(id)->item_instance = SpawnItem(item_template);
		ENTITY(id)->radius = acv::items[((HeldItem*)GetItemInstance(ITEM(id)->item_instance))->id_item_template]->f_radius;
		ENTITY(id)->height = 0.5f;
		return id;
	}
	btID SpawnEntityItem(btID itemid, m::Vector2 pos, btf32 height, btf32 dir)
	{
		btID id = InitEntity(ENTITY_TYPE_RESTING_ITEM);

		//spawn_setup_t(id, pos, dir);

		ENTITY(id)->t.position = pos;
		ENTITY(id)->t.velocity = 0.f;
		ENTITY(id)->t.height_velocity = 0.f;
		ENTITY(id)->t.yaw.Set(dir);
		GetCellSpaceInfo(ENTITY(id)->t.position, ENTITY(id)->t.csi);
		//env::GetHeight(ENTITY(id)->t.height, ENTITY(id)->t.csi);
		//ENTITY(id)->t.height += 1.f; // temp
		ENTITY(id)->t.height = height;
		AddEntityCell(ENTITY(id)->t.csi.c[eCELL_I].x, ENTITY(id)->t.csi.c[eCELL_I].y, id);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ENTITY(id)->state.damagestate = STATE_DAMAGE_MAX;
		ENTITY(id)->faction = fac::faction::none;
		ENTITY(id)->properties.set(Entity::ePREFAB_ITEM);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ITEM(id)->item_instance = itemid;
		ENTITY(id)->radius = acv::items[((HeldItem*)GetItemInstance(itemid))->id_item_template]->f_radius;
		ENTITY(id)->height = 0.5f;
		return id;
	}
	void DestroyEntity(btID id)
	{
		RemoveAllReferences(id);
		// A special case has to be made for items, which contain their own instance
		// which must also be destroyed
		//if (block_entity_data[id].type == ENTITY_TYPE_RESTING_ITEM)
			//DestroyItem(ITEM(id)->item_instance);
		IndexFreeEntity(id);
		std::cout << "Destroyed entity " << id << std::endl;
	}

	btID SpawnItem(btID item_template)
	{
		btID id = InitItemInstance(acv::item_types[item_template]);
		if (id != BUF_NULL)
		{
			GETITEMINST(id)->id_item_template = item_template;
			ItemInit(id);
			std::cout << "Created item " << id << std::endl;
		}
		else
		{
			std::cout << "Failed to create item!" << std::endl;
		}
		return id;
	}
	void DestroyItem(btID id)
	{
		FreeItemInstance(id);
		std::cout << "Destroyed item " << id << std::endl;
	}

	btID SpawnActivator(btui32 x, btui32 y)
	{
		btID id = InitActivator(ACTIVATOR_TYPE_JUNK);
		StaticActivator* act = (StaticActivator*)GetActivatorPtr(id);
		act->homePosition.x = x;
		act->homePosition.y = y;
		refCells[x][y].ref_activator = id;
		return id;
	}
	void DestroyActivator(btID id)
	{
		StaticActivator* act = (StaticActivator*)GetActivatorPtr(id);
		refCells[act->homePosition.x][act->homePosition.y].ref_activator = ID_NULL;
		FreeActivator(id);
	}

	void SpawnProjectile(fac::faction faction, btID type, m::Vector2 pos, btf32 height,
		float yaw, float pitch)
	{
		PrjID id = IndexSpawnProjectile();
		Projectile* proj = GetProj(id);

		proj->t.position_x = pos.x;
		proj->t.position_y = pos.y;
		proj->t.position_h = height;

		//
		/*
		btf32 r = 5.f;
		btf32 pitch_x = cos(editor_cam_pitch.Rad());
		btf32 pitch_y = sin(editor_cam_pitch.Rad());
		btf32 yaw_x = cos(editor_cam_yaw.Rad());
		btf32 yaw_y = sin(editor_cam_yaw.Rad());
		viewPosition[0] = viewTarget[0] + r * m::Vector3(-yaw_y * pitch_x, pitch_y, -yaw_x * pitch_x);
		*/

		proj->t.velocity_h = -sin(pitch);
		proj->t.velocity_x = (m::AngToVec2(yaw) * cos(pitch)).x; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj->t.velocity_y = (m::AngToVec2(yaw) * cos(pitch)).y; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj->ttd = tickCount + 60u;
		proj->faction = faction;
		proj->type = type;

		// save check
		if (acv::projectiles[type].saveOnHit)
		{
			SaveState();
			printf("Projectile Save\n");
		}
	}
	void SpawnProjectileSpread(fac::faction faction, btID type, m::Vector2 pos, btf32 height,
		float yaw, float pitch, float spread)
	{
		yaw += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
		pitch += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

		SpawnProjectile(faction, type, pos, height, yaw, pitch);
	}

	void DestroyProjectile(btID id)
	{
		IndexDestroyProjectileC(MakePrjID(id));
	}

	//________________________________________________________________________________________________________________________________
	// SOMETHING ---------------------------------------------------------------------------------------------------------------------

	void ActorCastProj(btID i)
	{
		SpawnProjectileSpread(ENTITY(i)->faction, 0, ENTITY(i)->t.position + (m::AngToVec2(ENTITY(i)->t.yaw.Rad()) * 0.55f), ENTITY(i)->t.height, ACTOR(i)->viewYaw.Rad(), ACTOR(i)->viewPitch.Rad(), 1.f);
	}

	//________________________________________________________________________________________________________________________________
	// PROJECTILES -------------------------------------------------------------------------------------------------------------------

	bool ProjectileCollideEnv(btID index)
	{
		Projectile* proj = GetProj(MakePrjID(index));

		int x = (int)roundf(proj->t.position_x);
		int y = (int)roundf(proj->t.position_y);
		if (env::Get(x, y, env::eflag::EF_IMPASSABLE)) // if hit an impassable tile
			return true;

		CellSpace csi;
		GetCellSpaceInfo(m::Vector2(proj->t.position_x, proj->t.position_y), csi);
		btf32 height;
		env::GetNearestSurfaceHeight(height, csi, proj->t.position_h);
		if (proj->t.position_h < height) // if below the ground surface
			return true;

		return false;
	}

	//-------------------------------- PROJECTILE FUNCTIONS

	int cater_loop_index(int i)
	{
		i++;
		if (i == BUF_SIZE)
			i = 0; // loop around
		return i;
	}

	void ProjectileTick(btf32 dt) // Projectile tick
	{
		int index = block_proj.index_first;
		bool stop = false;
		while (true)
		{
			if (index == block_proj.index_last)
				stop = true;
			if (block_proj.used[index])
			{
				// out of bounds check
				if (proj[index].t.position_x <= 1.f || proj[index].t.position_x >= WORLD_SIZE_MAXINT ||
					proj[index].t.position_y <= 1.f || proj[index].t.position_y >= WORLD_SIZE_MAXINT)
					DestroyProjectile(index);
				// If it's time to die, or collided
				if (tickCount >= proj[index].ttd || ProjectileCollideEnv(index))
					DestroyProjectile(index);
				else // Otherwise
				{
					proj[index].t.velocity_h -= 0.001f;
					proj[index].t.position_x += proj[index].t.velocity_x * dt * 24.f; // Use speed variable
					proj[index].t.position_y += proj[index].t.velocity_y * dt * 24.f; // Use speed variable
					proj[index].t.position_h += proj[index].t.velocity_h * dt * 24.f;

					//proj[index].t.position += proj[index].t.velocity * dt * 2.f; // Use speed variable
					//proj[index].t.height += proj[index].t.height_velocity * dt * 2.f;
					//proj[index].distance_travelled += m::Length(m::Vector3(proj[index].t.velocity.x * dt * 24.f, proj[index].t.velocity.y * dt * 24.f, proj[index].t.height_velocity * dt * 24.f));
					
					ProjectileHitCheck();
				}
			}
			if (stop)
				break;
			else
				index = cater_loop_index(index);
		}
	}

	void ProjectileDraw() // Projectile draw
	{
		int index = block_proj.index_first;
		bool stop = false;
		while (true)
		{
			if (index == block_proj.index_last)
				stop = true;
			if (block_proj.used[index])
			{
				glm::vec3 pos(proj[index].t.position_x, proj[index].t.position_h, proj[index].t.position_y);
				glm::vec3 dir(proj[index].t.velocity_x, proj[index].t.velocity_h, proj[index].t.velocity_y);

				//m::Vector3 velocity3d = m::Normalize(m::Vector3(proj[index].t.velocity.x, proj[index].t.height_velocity, proj[index].t.velocity.y));

				graphics::Matrix4x4 model; // Create identity matrix
				// All-in-one transform
				graphics::MatrixTransform(model, pos, m::Normalize(m::Vector3(proj[index].t.velocity_x, proj[index].t.velocity_h, proj[index].t.velocity_y)), m::Vector3(0, 1, 0));

				// Draw projectile mesh
				//DrawMesh(ID_NULL, acv::GetM(acv::m_proj), acv::GetT(acv::t_proj), SS_NORMAL, model);
				DrawMesh(ID_NULL, acv::GetM(acv::projectiles[proj[index].type].mesh),
					acv::GetT(acv::projectiles[proj[index].type].texture), SS_NORMAL, model);
			}
			if (stop)
				break;
			else
				index = cater_loop_index(index);
		}
	}

	// Iterates through all active projectiles and handles their collision
	void ProjectileHitCheck()
	{
		int index = block_proj.index_first;
		bool stop = false;
		while (true)
		{
			if (index == block_proj.index_last)
				stop = true;
			if (block_proj.used[index])
			{
				//if (tickCount_temp >= proj[index].ttd || ProjectileDoesIntersectEnv(index)) // TTD already checked in Projectile Tick
				if (ProjectileCollideEnv(index)) // If it's time to die
				{
					DestroyProjectile(index);
					btui16 x = (btui16)roundf(proj[index].t.position_x);
					btui16 y = (btui16)roundf(proj[index].t.position_y);
					m::Vector2 vec = m::Normalize(m::Vector2(proj[index].t.velocity_x, proj[index].t.velocity_y));
					btui16 x2 = (btui16)((bti16)x + (bti16)roundf(vec.x));
					btui16 y2 = (btui16)((bti16)y + (bti16)roundf(vec.y));
					//t_EnvHeightmap.SetPixelChannelG((btui16)roundf(proj[index].t.position.x), (btui16)roundf(proj[index].t.position.y), 255ui8);

					//if (!env::Get(x, y, env::eflag::eIMPASSABLE) && !env::Get(x2, y2, env::eflag::eIMPASSABLE))
					//{
					//	if (env::eCells[x2][y2].height < env::eCells[x][y].height + 3ui8) // If the height difference is low
					//	{
					//		--env::eCells[x][y].height;
					//		t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);
					//		++env::eCells[x2][y2].height;
					//		t_EnvHeightmap.SetPixelChannelR(x2, y2, env::eCells[x2][y2].height);
					//	}
					//}

					//t_EnvHeightmap.SetPixelChannelG(x, y, 255ui8);
					//t_EnvHeightmap.SetPixelChannelG(x + 1, y, 255ui8);
					//t_EnvHeightmap.SetPixelChannelG(x - 1, y, 255ui8);
					//t_EnvHeightmap.SetPixelChannelG(x, y + 1, 255ui8);
					//t_EnvHeightmap.SetPixelChannelG(x, y - 1, 255ui8);
					//t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
				}
				else // Otherwise
				{
					// LOOP THROUGH ALL ENTITIES METHOD, SHIT BUT ONLY WAY THAT WORKS ATM
					///*
					for (int i = 0; i <= GetLastEntity(); i++)
					{
						if (GetEntityExists(i) && ENTITY(i)->properties.get(Entity::eCOLLIDE_PRJ))
						{
							if (fac::GetAllegiance(ENTITY(i)->faction, (fac::faction)proj[index].faction) != fac::allied)
							{
								//check height difference
								if (proj[index].t.position_h > ENTITY(i)->t.height && proj[index].t.position_h < ENTITY(i)->t.height + ENTITY(i)->height)
								{
									//get difference between positions
									m::Vector2 vec = m::Vector2(proj[index].t.position_x, proj[index].t.position_y) - ENTITY(i)->t.position;
									//get distance
									float dist = m::Length(vec);
									if (dist < 0.5f)
									{
										// TODO: pass angle to damage fn
										ENTITY(i)->state.Damage(150u, glm::degrees(m::Vec2ToAng(vec)));
										DestroyProjectile(index); // Destroy the projectile
									}
								}
							}
						}
					}
					//*/
					// CELL GROUP METHOD (STILL UNRELIABLE
					/*
					CellGroup cg;
					GetCellGroup(proj[index].t.position, cg);
					for (int i = 0; i < 4; i++)
					{
						#define X cg.c[i].x
						#define Y cg.c[i].y
						// For all entities in this cell
						for (int e = 0; e < cells[X][Y].ents.last(); e++)
						{
							#define ID cells[X][Y].ents[e]
							if (cells[X][Y].ents[e] != ID_NULL)
								if (block_entity.used[ID] && ENTITY(ID)->properties.get(Entity::eCOLLIDE_PRJ))
								{
									if (fac::GetAllegiance(proj[index].faction, ENTITY(ID)->faction) != fac::allied)
									{
										//check height difference
										if (proj[index].t.height > ENTITY(ID)->t.height && proj[index].t.height < ENTITY(ID)->t.height + ENTITY(ID)->height)
										{
											//get difference between positions
											m::Vector2 vec = proj[index].t.position - ENTITY(ID)->t.position;
											//get distance
											float dist = m::Length(vec);
											if (dist < 0.5f)
											{
												// kill
												ENTITY(ID)->state.hp -= 0.5f;
												if (ENTITY(ID)->state.hp <= 0.f)
												{
													ENTITY(ID)->state.properties.unset(ActiveState::eALIVE);
													ENTITY(ID)->state.hp = 0.f;
												}
												DestroyProjectile(index); // Destroy the projectile
											}
										}
									}
								}
							#undef ID
						}
						#undef X
						#undef Y
					}
					//*/
				}
			}
			if (stop)
				break;
			else
				index = cater_loop_index(index);
		}
	}
}
