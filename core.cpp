#include "core.h"
#include "core_func.hpp"
#include "weather.h"

#include "index.h"

#include "core_save_load.h"

// TODO: probably dont want this here
#include "SDL2\SDL.h"

lui64 tickCount;
bool saveNextFrame = false;

namespace core
{
	void SetViewFocus(ID16 index) {
		activePlayer = index;
		graphics::SetMatView(&camViewTarget[activePlayer], &camViewPosition[activePlayer], nullptr);
	}

	bool CoreLoad() {
		bool loaded = LoadState();
		if (loaded) {
			gui.guiPlayerHP[0] = 0u;
			gui.guiPlayerHP[1] = 0u;
		}
		return loaded;
	};

	// still useful to have really
	/*
	void flood_fill_temp(lui16 srcx, lui16 srcy, lui16 x, lui16 y, graphics::colour col)
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
	}*/

	GLuint shadowtex;
	glm::mat4 shadowmat_temp;
	void SetShadowTexture(lui32 id)
	{
		shadowtex = id;
	}

	void UpdateOtherShaderParams(lf32 time2, m::Vector3 sunVec)
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
		#if DEF_GRID
		#if DEF_SPAWN_NPC
		// spawn characters
		for (int x = 0; x < WORLD_SIZE; ++x) {
			for (int y = 0; y < WORLD_SIZE; ++y) {
				if (!env::Get(x, y, env::eflag::EF_SPAWN_TEST)) continue;
				SpawnEntity(env::eCells.spawn_id[x][y], m::Vector2(x, y), 0.f);
			}
		}
		#endif

		// spawn items
		for (int x = 0; x < WORLD_SIZE; ++x) {
			for (int y = 0; y < WORLD_SIZE; ++y) {
				if (!env::Get(x, y, env::eflag::EF_SPAWN_ITEM_TEST)) continue;
				SpawnNewEntityItem(env::eCells.spawn_id[x][y], m::Vector2(x,y), 0.f);
			}
		}
		#endif
	}

	void Init()
	{
		IndexInitialize();

		// Use native window size for generating these gui elements
		graphics::SetGUIFrameSize(config.iWinX, config.iWinY);

		// Generate debug version display
		char buffinal[32];
		sprintf(buffinal, "%s v%i.%i.%i", DEF_PROJECTNAME_V, VERSION_MAJOR, VERSION_MINOR, VERSION_PROJECT);
		gui.text_version.ReGen(buffinal, config.iWinX * -0.5f, config.iWinX * 0.5f, config.iWinY * 0.5f - 12);

		#if DEF_PROJECT == PROJECT_BC
		gui.text_guidehelp.ReGen("Press F1 to read the guidebook!", config.iWinX * -0.5f + 2, config.iWinX * 0.5f, config.iWinY * 0.5f - 4);
		#endif

		//-------------------------------- Spawnz

		if (!SaveExists() || !CoreLoad()) {
			//SpawnActivator(1024u, 1024u);

			#ifdef DEF_NMP
			for (int i = 0; i < config.iNumNWPlayers; ++i) {
				players[i] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
			}
			#else
			players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
			players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(1023.f, 1022.f), 0.f);
			// test npc
			SpawnEntity(prefab::prefab_player_ally, m::Vector2(1025.f, 1025.f), 0.f);
			#endif

			#if DEF_PVP
			// PVP - align p2 with player hunter faction
			ENTITY(players[1])->faction = fac::playerhunter;
			#endif
			#if DEF_SPAWN_ON_INIT
			DoSpawn();
			#endif
			#if DEF_AUTOSAVE_ON_START
			SaveState();
			#endif
		}

		CheckPlayerAI();

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
	}
	void InitEditMode()
	{
		// Use native window size for generating these gui elements
		graphics::SetGUIFrameSize(config.iWinX, config.iWinY);

		// Generate debug version display
		char buffinal[32];
		sprintf(buffinal, "%s v%i.%i.%i", DEF_PROJECTNAME_V, VERSION_MAJOR, VERSION_MINOR, VERSION_PROJECT);
		gui.text_version.ReGen(buffinal, config.iWinX * -0.5f, config.iWinX * 0.5f, config.iWinY * 0.5f - 12);

		#if DEF_PROJECT == PROJECT_BC
		gui.text_guidehelp.ReGen("Press F1 to read the guidebook!", config.iWinX * -0.5f + 2, config.iWinX * 0.5f, config.iWinY * 0.5f - 4);
		#endif
	}
	void End()
	{
		ClearBuffers();
		IndexEnd();
	}

	void ClearBuffers()
	{
		IndexClearEntities();
		IndexClearItemInstances();
		for (int x = 0; x < WORLD_SIZE; ++x) {
			for (int y = 0; y < WORLD_SIZE; ++y) {
				refCells[x][y].ref_ents.Clear();
			}
		}
	}

	void RegenCellRefs()
	{
		for (int i = 0; i < GetEntityArraySize(); i++) {
			if (!AnyEntityHere(i)) continue;
			ECCommon* ent = (ECCommon*)GetEntityPtr(i);
			AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, GetEntityID(i));
		}
	}

	bool Tick(lf32 dt)
	{
		#if DEF_PROJECT == PROJECT_BC
		// kill the game if we won :P
		if (ACTOR(players[activePlayer])->inventory.CountItemsOfTemplate(1u) == 12u) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
				"Avertissement",
				"He'll yeah! you the game :) congratulations",
				NULL);
			return false;
		}
		#endif

		// TODO: think this over, there must be a better way to handle this
		#if DEF_AUTO_RELOAD_ON_DEATH
		// check if either player is dead
		if (!ENTITY(players[0])->stateFlags.get(ECCommon::eALIVE) || !ENTITY(players[1])->stateFlags.get(ECCommon::eALIVE)) {
			// Load the last save state
			CoreLoad();
		}
		#endif

		weather::Tick(dt);

		// temporary destroy dead entities
		for (int i = 0; i < GetEntityArraySize(); i++) {
			if (!AnyEntityHere(i)) continue;
			if (GetEntity<ECCommon>(i)->activeFlags.get(ECCommon::eALIVE)
				|| GetEntity<ECCommon>(i)->activeFlags.get(ECCommon::eDIED_REPORT))
				continue;
			//index::DestroyEntity(i);
			GetEntity<ECCommon>(i)->physicsFlags.unset(ECCommon::PhysicsFlags::eCOLLIDE_ENT);
			GetEntity<ECCommon>(i)->physicsFlags.unset(ECCommon::PhysicsFlags::eCOLLIDE_PRJ);
			GetEntity<ECCommon>(i)->physicsFlags.unset(ECCommon::PhysicsFlags::eCOLLIDE_MAG);
		}

		//-------------------------------- ITERATE THROUGH ENTITIES

		for (ID16 i = 0; i < GetEntityArraySize(); i++) { // For every entity
			if (!AnyEntityHere(i)) continue;
			GetEntity<ECCommon>(i)->TickEffects(dt);
			EntityTick(i, dt);
		}

		//-------------------------------- SOME OTHER SHIT

		ProjectileTick(dt);

		if (saveNextFrame) {
			SaveState();
			saveNextFrame = false;
		}
		#if DEF_PROJECT != PROJECT_BC || defined _DEBUG // disable save/load for BC
		#ifdef DEF_NMP
		for (lui32 i = 0; i < config.iNumNWPlayers; ++i) {
			if (input::GetHit(i, input::key::FUNCTION_5)) { // SAVE
				SaveState();
				break;
			}
			else if (input::GetHit(i, input::key::FUNCTION_9)) { // LOAD
				if (SaveExists()) CoreLoad();
				break;
			}
		}
		#else
		if (input::GetHit(input::key::FUNCTION_5)) { // SAVE
			SaveState();
		}
		else if (input::GetHit(input::key::FUNCTION_9)) { // LOAD
			if (SaveExists()) CoreLoad();
		}
		#endif
		#endif

		#if DEF_PROJECT == PROJECT_BC
		if (input::GetHit(input::key::FUNCTION_1)) { // TOGGLE GUIDE
			gui.bShowGuide = !gui.bShowGuide;
			gui.guideW = 0;
		}
		#endif

		//-------------------------------- Modify camera

		if (config.b3PP) {
			#if DEF_NMP
			m::Vector3 target_a = m::Vector3(ENTITY(players[network::nid])->t.position.x, ENTITY(players[network::nid])->t.altitude + 0.8f, ENTITY(players[network::nid])->t.position.y);
			m::Vector2 dir_a = (m::AngToVec2(ENTITY(players[network::nid])->t.yaw.Rad()) + m::AngToVec2(ACTOR(players[network::nid])->viewYaw.Rad())) * 0.5f;
			m::Vector3 position_a = target_a - (m::Vector3(dir_a.x, -0.2f, dir_a.y)) * 3.f;
			#else			
			m::Vector3 target_a = m::Vector3(ENTITY(players[0])->t.position.x, ENTITY(players[0])->t.altitude + 0.8f, ENTITY(players[0])->t.position.y);
			m::Vector2 dir_a = (m::AngToVec2(ENTITY(players[0])->t.yaw.Rad()) + m::AngToVec2(ACTOR(players[0])->viewYaw.Rad())) * 0.5f;
			m::Vector3 position_a = target_a - (m::Vector3(dir_a.x, -0.2f, dir_a.y)) * 3.f;
			m::Vector3 target_b = m::Vector3(ENTITY(players[1])->t.position.x, ENTITY(players[1])->t.altitude + 0.8f, ENTITY(players[1])->t.position.y);
			m::Vector2 dir_b = (m::AngToVec2(ENTITY(players[1])->t.yaw.Rad()) + m::AngToVec2(ACTOR(players[1])->viewYaw.Rad())) * 0.5f;
			m::Vector3 position_b = target_b - (m::Vector3(dir_b.x, -0.2f, dir_b.y)) * 3.f;
			#endif

			#if DEF_GRID
			#if DEF_NMP
			if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y)) {
				position_a = m::Lerp(position_a, target_a, 0.5f);
				if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y)) {
					position_a = m::Lerp(position_a, target_a, 0.75f);
				}
			}
			#else
			if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y)) {
				position_a = m::Lerp(position_a, target_a, 0.5f);
				if (!env::LineTraceBh(roundf(target_a.x), roundf(target_a.z), roundf(position_a.x), roundf(position_a.z), target_a.y, position_a.y)) {
					position_a = m::Lerp(position_a, target_a, 0.75f);
				}
			}
			if (config.bSplitScreen) {
				if (!env::LineTraceBh(roundf(target_b.x), roundf(target_b.z), roundf(position_b.x), roundf(position_b.z), target_b.y, position_b.y)) {
					position_b = m::Lerp(position_b, target_b, 0.5f);
					if (!env::LineTraceBh(roundf(target_b.x), roundf(target_b.z), roundf(position_b.x), roundf(position_b.z), target_b.y, position_b.y)) {
						position_b = m::Lerp(position_b, target_b, 0.75f);
					}
				}
			}
			#endif
			#else
			#if DEF_NMP
			env::LineTraceHit hit;
			if (env::LineTrace(target_a.x, target_a.z, position_a.x, position_a.z, target_a.y, position_a.y, &hit)) {
				position_a.x = hit.pos.x;
				position_a.z = hit.pos.y;
				position_a = position_a + m::Vector3(dir_a.x, 0.f, dir_a.y) * 0.25f; // push the camera away from the wall a bit
				//position_a.y = hit.h;
			}
			#endif
			#endif

			#if DEF_NMP
			// Snap if the difference between now and the target is too great
			if (m::Length(camViewTarget[network::nid] - target_a) > 10.f || m::Length(camViewPosition[network::nid] - position_a) > 10.f) {
				camViewTarget[network::nid] = target_a;
				camViewPosition[network::nid] = position_a;
			}
			// Move camera gradually
			else {
				camViewTarget[network::nid] = m::BlendToward(camViewTarget[network::nid], target_a, 0.05f, dt);
				camViewPosition[network::nid] = m::BlendToward(camViewPosition[network::nid], position_a, 0.15f, dt);
			}
			#else
			// Snap if the difference between now and the target is too great
			if (m::Length(camViewTarget[0] - target_a) > 10.f || m::Length(camViewPosition[0] - position_a) > 10.f) {
				camViewTarget[0] = target_a;
				camViewPosition[0] = position_a;
			}
			// Move camera gradually
			else {
				camViewTarget[0] = m::BlendToward(camViewTarget[0], target_a, 0.05f, dt);
				camViewPosition[0] = m::BlendToward(camViewPosition[0], position_a, 0.15f, dt);
			}
			if (config.bSplitScreen) {
				// Snap if the difference between now and the target is too great
				if (m::Length(camViewTarget[1] - target_b) > 10.f || m::Length(camViewPosition[1] - position_b) > 10.f) {
					camViewTarget[1] = target_b;
					camViewPosition[1] = position_b;
				}
				// Move camera gradually
				else {
					camViewTarget[1] = m::BlendToward(camViewTarget[1], target_b, 0.05f, dt);
					camViewPosition[1] = m::BlendToward(camViewPosition[1], position_b, 0.15f, dt);
				}
			}
			#endif
		}
		else {
			#if DEF_NMP
			camViewPosition[network::nid] = ACTOR(players[network::nid])->t_head.GetPosition() +
				m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), ACTOR(players[network::nid])->t_head.GetRotation());
			camViewTarget[network::nid] = camViewPosition[network::nid] + ACTOR(players[network::nid])->t_head.GetForward();
			#else
			camViewPosition[0] = ACTOR(players[0])->t_head.GetPosition() +
				m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), ACTOR(players[0])->t_head.GetRotation());
			camViewTarget[0] = camViewPosition[0] + ACTOR(players[0])->t_head.GetForward();
			if (config.bSplitScreen) {
				camViewPosition[1] = ACTOR(players[1])->t_head.GetPosition() +
					m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), ACTOR(players[1])->t_head.GetRotation());
				camViewTarget[1] = camViewPosition[1] + ACTOR(players[1])->t_head.GetForward();
			}
			#endif
		}

		#if DEF_PROJECT == PROJECT_BC
		{ // Set 3pp based on tile parameter
			#ifdef DEF_NMP
			config.b3PP = true;
			for (int i = 0; i < config.iNumNWPlayers; ++i) {
				ECCommon* ent = ENTITY(players[i]);
				int x = ent->t.csi.c[eCELL_I].x;
				int y = ent->t.csi.c[eCELL_I].y;
				if (env::Get(x, y, env::eflag::EF_FPP_HERE))
					config.b3PP = false;
			}
			#else
			ECCommon* ent = ENTITY(players[0]);
			int x = ent->t.csi.c[eCELL_I].x;
			int y = ent->t.csi.c[eCELL_I].y;
			if (env::Get(x, y, env::eflag::EF_FPP_HERE))
				config.b3PP = false;
			else
				config.b3PP = true;
			#endif
		}
		#endif

		//-------------------------------- Stuff

		++tickCount;
	
		return true;
	}

	#if DEF_GRID
	void EditorModTerrain(li32 cellX, li32 cellY, lf32 offsX, lf32 offsY, li32 modify) {
		if (input::GetHeld(input::key::RUN))
		{
			env::eCells.terrain_height_ne[cellX][cellY] += modify;
			env::eCells.terrain_height_nw[cellX][cellY] += modify;
			env::eCells.terrain_height_se[cellX][cellY] += modify;
			env::eCells.terrain_height_sw[cellX][cellY] += modify;
		}
		else if (input::GetHeld(input::key::CROUCH))
		{
			if (offsX > 0.f && offsY > 0.f) { // NE
				if (env::eCells.terrain_height_nw[cellX + 1][cellY] == env::eCells.terrain_height_ne[cellX][cellY])
					env::eCells.terrain_height_nw[cellX + 1][cellY] += modify;
				if (env::eCells.terrain_height_se[cellX][cellY + 1] == env::eCells.terrain_height_ne[cellX][cellY])
					env::eCells.terrain_height_se[cellX][cellY + 1] += modify;
				if (env::eCells.terrain_height_sw[cellX + 1][cellY + 1] == env::eCells.terrain_height_ne[cellX][cellY])
					env::eCells.terrain_height_sw[cellX + 1][cellY + 1] += modify;
				env::eCells.terrain_height_ne[cellX][cellY];
			}
			else if (offsY > 0.f) { // NW
				if (env::eCells.terrain_height_ne[cellX - 1][cellY] == env::eCells.terrain_height_nw[cellX][cellY])
					env::eCells.terrain_height_ne[cellX - 1][cellY] += modify;
				if (env::eCells.terrain_height_sw[cellX][cellY + 1] == env::eCells.terrain_height_nw[cellX][cellY])
					env::eCells.terrain_height_sw[cellX][cellY + 1] += modify;
				if (env::eCells.terrain_height_se[cellX - 1][cellY + 1] == env::eCells.terrain_height_nw[cellX][cellY])
					env::eCells.terrain_height_se[cellX - 1][cellY + 1] += modify;
				env::eCells.terrain_height_nw[cellX][cellY] += modify;
			}
			else if (offsX > 0.f) { // SE
				if (env::eCells.terrain_height_sw[cellX + 1][cellY] == env::eCells.terrain_height_se[cellX][cellY])
					env::eCells.terrain_height_sw[cellX + 1][cellY] += modify;
				if (env::eCells.terrain_height_ne[cellX][cellY - 1] == env::eCells.terrain_height_se[cellX][cellY])
					env::eCells.terrain_height_ne[cellX][cellY - 1] += modify;
				if (env::eCells.terrain_height_nw[cellX + 1][cellY - 1] == env::eCells.terrain_height_se[cellX][cellY])
					env::eCells.terrain_height_nw[cellX + 1][cellY - 1] += modify;
				env::eCells.terrain_height_se[cellX][cellY] += modify;
			}
			else { // SW
				if (env::eCells.terrain_height_se[cellX - 1][cellY] == env::eCells.terrain_height_sw[cellX][cellY])
					env::eCells.terrain_height_se[cellX - 1][cellY] += modify;
				if (env::eCells.terrain_height_nw[cellX][cellY - 1] == env::eCells.terrain_height_sw[cellX][cellY])
					env::eCells.terrain_height_nw[cellX][cellY - 1] += modify;
				if (env::eCells.terrain_height_ne[cellX - 1][cellY - 1] == env::eCells.terrain_height_sw[cellX][cellY])
					env::eCells.terrain_height_ne[cellX - 1][cellY - 1] += modify;
				env::eCells.terrain_height_sw[cellX][cellY] += modify;
			}
		}
		else
		{
			if (offsX > 0.f && offsY > 0.f)
				env::eCells.terrain_height_ne[cellX][cellY] += modify;
			else if (offsY > 0.f)
				env::eCells.terrain_height_nw[cellX][cellY] += modify;
			else if (offsX > 0.f)
				env::eCells.terrain_height_se[cellX][cellY] += modify;
			else
				env::eCells.terrain_height_sw[cellX][cellY] += modify;
		}
		env::GenerateTerrainMesh();
	}
	#endif

	bool TickEditor(lf32 dt)
	{
		#ifndef DEF_NMP
		if (input::GetHeld(input::key::RUN)) {
			editor.cam_pitch.RotateClamped(input::input_buffer.mouse_y * 20.f * dt, 5.f, 85.f);
			editor.cam_yaw.Rotate(input::input_buffer.mouse_x * 10.f * dt);
		}
		else {
			m::Vector2 vec = m::AngToVec2(editor.cam_yaw.Rad());
			editor.cursor +=
				((m::Vector2(vec.y, -vec.x) * input::input_buffer.mouse_x)
					+ (vec * -input::input_buffer.mouse_y)) * 0.35f * dt;
		}

		li32 editor_cursor_x = roundf(editor.cursor.x);
		li32 editor_cursor_y = roundf(editor.cursor.y);

		lf32 editor_cursor_offset_x = editor.cursor.x - (lf32)editor_cursor_x;
		lf32 editor_cursor_offset_y = editor.cursor.y - (lf32)editor_cursor_y;

		if (input::GetHit(input::key::FUNCTION_5)) { // SAVE
			env::SaveBin();
		}

		#if DEF_GRID
		if (input::GetHit(input::key::USE)) {
			++env::eCells.terrain_material[editor_cursor_x][editor_cursor_y];
			if (env::eCells.terrain_material[editor_cursor_x][editor_cursor_y] > 7u)
				env::eCells.terrain_material[editor_cursor_x][editor_cursor_y] = 0u;
			env::GenerateTerrainMesh();
		}
		else if (input::GetHeld(input::key::USE_ALT)) {
			env::eCells.terrain_material[editor_cursor_x][editor_cursor_y] = editor.material_copy;
			env::GenerateTerrainMesh();
		}
		else if (input::GetHeld(input::key::DIR_F)) // Forward
			env::eCells.prop_dir[editor_cursor_x][editor_cursor_y] = env::eNORTH;
		else if (input::GetHeld(input::key::DIR_B)) // Back
			env::eCells.prop_dir[editor_cursor_x][editor_cursor_y] = env::eSOUTH;
		else if (input::GetHeld(input::key::DIR_R)) // Right
			env::eCells.prop_dir[editor_cursor_x][editor_cursor_y] = env::eEAST;
		else if (input::GetHeld(input::key::DIR_L)) // Left
			env::eCells.prop_dir[editor_cursor_x][editor_cursor_y] = env::eWEST;
		else if (input::GetHit(input::key::FUNCTION_1)) { // COPY
			editor.flags_copy = env::eCells.flags[editor_cursor_x][editor_cursor_y];
			editor.prop_copy = env::eCells.prop[editor_cursor_x][editor_cursor_y];
			editor.prop_dir_copy = env::eCells.prop_dir[editor_cursor_x][editor_cursor_y];
			editor.height_copy_ne = env::eCells.terrain_height_ne[editor_cursor_x][editor_cursor_y];
			editor.height_copy_nw = env::eCells.terrain_height_nw[editor_cursor_x][editor_cursor_y];
			editor.height_copy_se = env::eCells.terrain_height_se[editor_cursor_x][editor_cursor_y];
			editor.height_copy_sw = env::eCells.terrain_height_sw[editor_cursor_x][editor_cursor_y];
			editor.material_copy = env::eCells.terrain_material[editor_cursor_x][editor_cursor_y];
		}
		else if (input::GetHeld(input::key::FUNCTION_2)) { // PASTE
			env::eCells.flags[editor_cursor_x][editor_cursor_y] = (env::eflag::flag)editor.flags_copy;
			env::eCells.prop[editor_cursor_x][editor_cursor_y] = editor.prop_copy;
			env::eCells.prop_dir[editor_cursor_x][editor_cursor_y] = editor.prop_dir_copy;
			env::eCells.terrain_height_ne[editor_cursor_x][editor_cursor_y] = editor.height_copy_ne;
			env::eCells.terrain_height_nw[editor_cursor_x][editor_cursor_y] = editor.height_copy_nw;
			env::eCells.terrain_height_se[editor_cursor_x][editor_cursor_y] = editor.height_copy_se;
			env::eCells.terrain_height_sw[editor_cursor_x][editor_cursor_y] = editor.height_copy_sw;
			env::eCells.terrain_material[editor_cursor_x][editor_cursor_y] = editor.material_copy;
			env::GenerateTerrainMesh();
		}
		else if (input::GetHit(input::key::FUNCTION_3)) { // TOGGLE LIGHT
			env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_LIGHTSRC)
				? env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_LIGHTSRC)
				: env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_LIGHTSRC);
		}
		else if (input::GetHit(input::key::FUNCTION_4)) { // TOGGLE SPAWN
			if (env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_TEST)) {
				env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_TEST);
				env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_ITEM_TEST);
			}
			else if (env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_ITEM_TEST)) {
				env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_TEST);
				env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_ITEM_TEST);
			}
			else {
				env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_TEST);
			}
		}
		else if (input::GetHit(input::key::FUNCTION_6)) { // TOGGLE SPAWN INDEX
			if (env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_TEST)) {
				++env::eCells.spawn_id[editor_cursor_x][editor_cursor_y];
				// we don't have a proper archive setup for entity templates
				if (env::eCells.spawn_id[editor_cursor_x][editor_cursor_y] >= prefab::prefab_count)
					env::eCells.spawn_id[editor_cursor_x][editor_cursor_y] = 0u;
			}
			else if (env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_SPAWN_ITEM_TEST)) {
				++env::eCells.spawn_id[editor_cursor_x][editor_cursor_y];
				if (env::eCells.spawn_id[editor_cursor_x][editor_cursor_y] >= acv::item_index)
					env::eCells.spawn_id[editor_cursor_x][editor_cursor_y] = 0u;
			}
		}
		else if (input::GetHit(input::key::FUNCTION_7)) { // TOGGLE FPP
			env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_FPP_HERE)
				? env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_FPP_HERE)
				: env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_FPP_HERE);
		}
		else if (input::GetHit(input::key::ACTION_A)) {
			env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_IMPASSABLE)
				? env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_IMPASSABLE)
				: env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_IMPASSABLE);
		}
		else if (input::GetHit(input::key::ACTION_B)) {
			if (env::eCells.prop[editor_cursor_x][editor_cursor_y] > 0u)
				--env::eCells.prop[editor_cursor_x][editor_cursor_y];
		}
		else if (input::GetHit(input::key::ACTION_C)) {
			if (env::eCells.prop[editor_cursor_x][editor_cursor_y] < acv::prop_index)
				++env::eCells.prop[editor_cursor_x][editor_cursor_y];
		}
		else if (input::GetHit(input::key::FUNCTION_7)) {
			env::Get(editor_cursor_x, editor_cursor_y, env::eflag::EF_INVISIBLE)
				? env::UnSet(editor_cursor_x, editor_cursor_y, env::eflag::EF_INVISIBLE)
				: env::Set(editor_cursor_x, editor_cursor_y, env::eflag::EF_INVISIBLE);
			env::GenerateTerrainMesh();
		}
		else if (input::GetHit(input::key::INV_CYCLE_R)) {
			EditorModTerrain(editor_cursor_x, editor_cursor_y, editor_cursor_offset_x, editor_cursor_offset_y, 1);
		}
		else if (input::GetHit(input::key::INV_CYCLE_L)) {
			EditorModTerrain(editor_cursor_x, editor_cursor_y, editor_cursor_offset_x, editor_cursor_offset_y, -1);
		}

		#endif
		#endif

		//-------------------------------- Modify camera

		GetCellSpaceInfo(editor.cursor, editor.cursorCS);

		lf32 height;

		#if DEF_GRID
		env::GetHeight(height, editor.cursorCS);
		#else
		env::GetNearestSurfaceHeight(height, editor.cursorCS, 1000000.f);
		#endif

		editor.cursor_height = m::Lerp(editor.cursor_height, height, 1.5f * dt);

		camViewTarget[0] = m::Vector3(editor.cursor.x, editor.cursor_height, editor.cursor.y);

		lf32 r = 3.5f;
		lf32 pitch_x = cos(editor.cam_pitch.Rad());
		lf32 pitch_y = sin(editor.cam_pitch.Rad());
		lf32 yaw_x = cos(editor.cam_yaw.Rad());
		lf32 yaw_y = sin(editor.cam_yaw.Rad());
		camViewPosition[0] = camViewTarget[0] + r * m::Vector3(-yaw_y * pitch_x, pitch_y, -yaw_x * pitch_x);

		//-------------------------------- Stuff

		++tickCount;

		return true;
	}

	void Draw(bool oob)
	{
		lf32 time2 = ((lf32)tickCount / 30.f) * 0.02f + 0.2f;
		m::Vector3 sunVec = m::Normalize(m::Vector3(0.5f, 1.f, 0.5f));

		graphics::Matrix4x4 matrix; // Matrix used for rendering env. props (so far...)

		graphics::SetFrontFace(); // reset front face (is set wrong in other places because i'm an idiota)

		if (oob)
		{
			//-------------------------------- UPDATE SHADERS

			UpdateOtherShaderParams(time2, sunVec);

			//-------------------------------- DRAW SKY

			#if DEF_PROJECT == PROJECT_BC
			if (config.b3PP) graphics::SetMatProj(0.5f);
			else graphics::SetMatProj();
			#else
			graphics::SetMatProj();
			#endif

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
			m::Vector3 lightPos(0.f, 0.f, 0.f);
			if (!config.bEditMode) lightPos = m::Vector3(
				ENTITY(players[activePlayer])->t.position.x,
				ENTITY(players[activePlayer])->t.altitude,
				-ENTITY(players[activePlayer])->t.position.y);
			m::Vector3 lightVecForw(-sunVec.x, -sunVec.y, -sunVec.z);
			//m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 1.f, 0.f)));
			m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 0.f, 1.f)));
			m::Vector3 LightVecUp = m::Normalize(m::Cross(lightVecForw, LightVecSide));

			#define LIGHT_RND ((SHADOW_RESOLUTION / SHADOW_WIDTH) / 2)

			lf32 moveF = roundf(m::Dot(lightVecForw, lightPos));
			lf32 moveS = roundf(m::Dot(LightVecSide, lightPos) * LIGHT_RND) / LIGHT_RND;
			lf32 moveU = roundf(m::Dot(LightVecUp, lightPos) * LIGHT_RND) / LIGHT_RND;

			#undef LIGHT_RND

			lightPos = lightVecForw * moveF + LightVecSide * moveS + LightVecUp * moveU;

			// Set light matrices
			graphics::SetMatProjLight(); graphics::SetMatViewLight(
				lightPos.x, lightPos.y, lightPos.z,
				-sunVec.x, -sunVec.y, -sunVec.z);
			shadowmat_temp = graphics::GetMatProj() * graphics::GetMatView();
		}

		if (!config.bEditMode) if (oob) ProjectileDraw(); // Draw projectiles

		//-------------------------------- DRAW ENTITIES AND PROPS
		
		matrix = graphics::Matrix4x4();

		#ifdef DEF_DRAW_WIREFRAME
		graphics::SetRenderWire();
		#endif // DEF_DRAW_WIREFRAME

		if (config.bEditMode)
		{
			li32 editor_cursor_x = roundf(editor.cursor.x);
			li32 editor_cursor_y = roundf(editor.cursor.y);
			#if DEF_GRID
			graphics::MatrixTransform(matrix, m::Vector3(editor_cursor_x,
				env::eCells.terrain_height[editor_cursor_x][editor_cursor_y] /
				TERRAIN_HEIGHT_DIVISION, editor_cursor_y));
			DrawMesh(acv::GetM(acv::m_debugcell), acv::GetT(acv::t_gui_bar_red), SS_NORMAL, matrix);
			#endif

			#if DEF_GRID
			lui32 drawrange = 16u; // Create min/max draw coordinates
			li32 minx = editor_cursor_x - drawrange; if (minx < 0) minx = 0;
			li32 maxx = editor_cursor_x + drawrange; if (maxx > WORLD_SIZE_MAXINT) maxx = WORLD_SIZE_MAXINT;
			li32 miny = editor_cursor_y - drawrange; if (miny < 0) miny = 0;
			li32 maxy = editor_cursor_y + drawrange; if (maxy > WORLD_SIZE_MAXINT) maxy = WORLD_SIZE_MAXINT;
			for (li32 x = minx; x <= maxx; x++)
			{
				for (li32 y = miny; y < maxy; y++)
				{
					lf32 rotation_by_enum[]
					{
						0.f, 180.f, 90.f, 270.f,
					};
					matrix = graphics::Matrix4x4();


					graphics::SetRenderWire();

					if (env::Get(x, y, env::eflag::EF_FPP_HERE)) {
					//if (env::Get(x, y, env::eflag::EF_IMPASSABLE)) {
						graphics::MatrixTransform(matrix, m::Vector3(
							x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_debug_bb), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_LIGHTSRC)) {
						graphics::MatrixTransform(matrix, m::Vector3(
							x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_default), SS_NORMAL, matrix);
					}

					graphics::SetRenderSolid();
					
					if (env::Get(x, y, env::eflag::EF_SPAWN_TEST)) {
						graphics::MatrixTransform(matrix, m::Vector3(
							x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(acv::GetM(acv::m_debug_monkey), acv::GetT(acv::t_default), SS_NORMAL, matrix);
						for (int i = 0; i < env::eCells.spawn_id[x][y]; ++i) {
							graphics::MatrixTransform(matrix, m::Vector3(
								x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f + 0.125f + (i * 0.125f), y));
							DrawMesh(acv::GetM(acv::m_debug_monkey),
								acv::GetT(acv::t_col_red), SS_NORMAL, matrix);
						}
					}
					if (env::Get(x, y, env::eflag::EF_SPAWN_ITEM_TEST)) {
						graphics::MatrixTransform(matrix, m::Vector3(
							x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION + 0.5f, y));
						DrawMesh(acv::GetM(acv::items[env::eCells.spawn_id[x][y]]->id_mesh),
							acv::GetT(acv::items[env::eCells.spawn_id[x][y]]->id_tex), SS_NORMAL, matrix);
					}

					if (env::eCells.prop[x][y] == ID_NULL) env::eCells.prop[x][y] = 0u;
					//-------------------------------- DRAW ENVIRONMENT PROP ON THIS CELL
					if (env::eCells.prop[x][y] != ID_NULL && env::eCells.prop[x][y] > 0u) {
						graphics::MatrixTransform(matrix,
							m::Vector3(x, env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, y),
							glm::radians(rotation_by_enum[env::eCells.prop_dir[x][y]]));
						DrawMesh(acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
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
			#endif
			env::DrawTerrainDebug();
		}
		else
		{
			#if !DEF_GRID
			// Draw debug ce;; display
			//CellSpace* cs = &ENTITY(players[activePlayer])->t.csi;
			//env::DrawDebugGizmos(cs);
			#endif

			for (ID16 i = 0; i < GetEntityArraySize(); i++) { // For every entity
				// Early exit if the entity isnt even real
				if (!AnyEntityHere(i)) continue;
				// For distance culling
				m::Vector3 diff = m::Vector3(GetEntity<ECCommon>(i)->t.position.x, GetEntity<ECCommon>(i)->t.altitude, GetEntity<ECCommon>(i)->t.position.y)
					- camViewPosition[activePlayer];
				// Dot against the camera direction, for not drawing behind us
				// Not as good as frustum culling, but I'll live
				lf32 dot = m::Dot(diff,
					m::Normalize(camViewTarget[activePlayer] - camViewPosition[activePlayer]));
				// Culling
				if (m::Length(diff) < 30.f && dot > 0.f) {
					EntityDraw(i);
				}
			}

			#define DRAWRANGE 16u
			/*
			Entity* entity = ENTITY(players[activePlayer]);
			// Set min/max draw coordinates
			li32 minx = entity->t.csi.c[0].x - DRAWRANGE; if (minx < 0) minx = 0;
			li32 maxx = entity->t.csi.c[0].x + DRAWRANGE; if (maxx > WORLD_SIZE_MAXINT) maxx = WORLD_SIZE_MAXINT;
			li32 miny = entity->t.csi.c[0].y - DRAWRANGE; if (miny < 0) miny = 0;
			li32 maxy = entity->t.csi.c[0].y + DRAWRANGE; if (maxy > WORLD_SIZE_MAXINT) maxy = WORLD_SIZE_MAXINT;
			for (li32 x = minx; x <= maxx; x++) {
				for (li32 y = miny; y < maxy; y++) {
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
			#if !DEF_SHADOW_ALL_OBJECTS
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

	// move somewhere else
	void SoulTransferTemp(lui32 player, LtrID from, LtrID to) {
		// if we are allied
		if (fac::GetAllegiance(ENTITY(from)->faction, ENTITY(to)->faction) == fac::allied) {
			// SOUL TRANSFER
			if (!IDCOMPARE(players[0], players[1])) { // If both player's arent using the same entity
				ACTOR(players[player])->aiControlled = true; // Let the AI take over
				ACTOR(players[player])->ai_target_ent = ID2_NULL; // Let the AI take over
				ACTOR(players[player])->ai_ally_ent = ID2_NULL; // Let the AI take over
			}
			players[player] = to; // Set player to control player's view target
			ACTOR(players[player])->aiControlled = false;
		}
	}

	void TickGUI()
	{
		// Not the most elegant of functions
		#ifdef DEF_NMP
		for (ID16 i = 0u; i < config.iNumNWPlayers; ++i) {
			if (input::GetHit(i, input::key::INV_CYCLE_L))
				ActorDecrEquipSlot(players[i]);
			if (input::GetHit(i, input::key::INV_CYCLE_R))
				ActorIncrEquipSlot(players[i]);
			if (input::GetHit(i, input::key::ACTIVATE)) { // Pick up items
				LtrID viewtarget = ACTOR(players[i])->viewtarget;
				if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_RESTING_ITEM) {
					ActorTakeItem(players[i], viewtarget);
				}
				else if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_ACTOR) {
					// hold hand
					ActorTryHoldHand(players[i], viewtarget);
					// soultransfer
					//SoulTransferTemp(i, players[i], viewtarget);
				}
			}
			if (input::GetHit(i, input::key::DROP_HELD))
				ActorDropItem(players[i], ACTOR(players[i])->inv_active_slot);
		}
		#else
		if (activePlayer == 0u) {
			if (input::GetHit(input::key::INV_CYCLE_L))
				ActorDecrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::INV_CYCLE_R))
				ActorIncrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::ACTIVATE)) { // Pick up items
				LtrID viewtarget = ACTOR(players[activePlayer])->viewtarget;
				if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_RESTING_ITEM) {
					ActorTakeItem(players[activePlayer], viewtarget);
				}
				else if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_ACTOR) {
					// hold hand
					ActorTryHoldHand(players[activePlayer], viewtarget);
					// soultransfer
					//SoulTransferTemp(activePlayer, players[activePlayer], viewtarget);
				}
			}
			if (input::GetHit(input::key::DROP_HELD))
				ActorDropItem(players[activePlayer], ACTOR(players[activePlayer])->inv_active_slot);
		}
		else {
			if (input::GetHit(input::key::C_INV_CYCLE_L))
				ActorDecrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::C_INV_CYCLE_R))
				ActorIncrEquipSlot(players[activePlayer]);
			if (input::GetHit(input::key::C_ACTIVATE)) { // Pick up items
				LtrID viewtarget = ACTOR(players[activePlayer])->viewtarget;
				if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_RESTING_ITEM) {
					ActorTakeItem(players[activePlayer], viewtarget);
				}
				else if (GetEntityExists(viewtarget) && GetEntityType(viewtarget.Index()) == ENTITY_TYPE_ACTOR) {
					// hold hand
					ActorTryHoldHand(players[activePlayer], viewtarget);
					// soultransfer
					//SoulTransferTemp(activePlayer, players[activePlayer], viewtarget);
				}
			}
			if (input::GetHit(input::key::C_DROP_HELD))
				ActorDropItem(players[activePlayer], ACTOR(players[activePlayer])->inv_active_slot);
		}
		#endif // DEF_NMP
	}

	void GUISetMessag(int player, char* string)
	{
		graphics::SetGUIFrameSize(graphics::FrameSizeX(), graphics::FrameSizeY());
		int halfw = -(int)graphics::FrameSizeX() / 2;
		gui.message_time[player] = tickCount + 90u;
		gui.text_message[player].ReGen(string, halfw + 16, -halfw - 16, 32);
	}

	void GUIDrawInventory(Inventory* inv, lui16 active_slot)
	{
		int p1_x_start = -(int)graphics::FrameSizeX() / 2;
		int p1_y_start = -(int)graphics::FrameSizeY() / 2;

		const li32 invspace = 38;

		graphics::GUIText text;

		gui.guiInvTimer[activePlayer] = m::StepToward(gui.guiInvTimer[activePlayer], (lf32)active_slot, 0.4f);

		li32 xoffs = p1_x_start + 24;
		li32 yoffs = p1_y_start + 24 + 32; // add hp bar height

		// Calculate loop bounds
		li32 min = (li32)ceilf(gui.guiInvTimer[activePlayer]) - 2;
		if (min < 0) min = 0;
		li32 max = (li32)floorf(gui.guiInvTimer[activePlayer]) + 3;
		if (max >= inv->items.Size()) max = inv->items.Size() - 1;
		// Loop through items
		for (int i = min; i <= max; ++i) {
			if (inv->items.Used(i)) {
				lf32 xoffsf = gui.guiInvTimer[activePlayer] * invspace;
				if ((lf32)i >= gui.guiInvTimer[activePlayer]) {
					lf32 opacity = 1.f - m::Clamp(fabsf(((lf32)i - gui.guiInvTimer[activePlayer]) * (1.f / 3.f)), 0.f, 1.f);
					graphics::DrawGUITexture(&acv::GetT(acv::items[GETITEMINST(inv->items[i])->id_item_template]->id_icon),
						xoffs + (i * invspace) - xoffsf, yoffs, 64, 64, opacity);
				}
				else {
					lf32 opacity = 1.f - m::Clamp(fabsf(((lf32)i - gui.guiInvTimer[activePlayer]) * 0.5f), 0.f, 1.f);
					graphics::DrawGUITexture(&acv::GetT(acv::items[GETITEMINST(inv->items[i])->id_item_template]->id_icon),
						xoffs, yoffs - (i * invspace) + xoffsf, 64, 64, opacity);
				}
			}
		}

		if (inv->items.Used(active_slot)) {
			// draw itemname
			text.ReGen((char*)(acv::items[GETITEMINST(inv->items[active_slot])->id_item_template]->name),
				xoffs + 24, xoffs + 2048, yoffs - 8);
			text.Draw(&acv::GetT(acv::t_gui_font));
			// draw item count
			if (GetItemInstanceType(inv->items[active_slot].Index()) == ITEM_TYPE_CONS) {
				char textbuffer[8];
				_itoa(GETITEMINST(inv->items[active_slot])->uses, textbuffer, 10);
				text.ReGen(textbuffer, xoffs, xoffs + 512, yoffs, graphics::eTEXTALIGN_MID);
				text.Draw(&acv::GetT(acv::t_gui_font));
			}
		}

		int boxoffs = (m::Clamp(fabsf(((lf32)active_slot - gui.guiInvTimer[activePlayer]) * 0.5f), 0.f, 1.f) * 12.f);
		if (boxoffs > 6) boxoffs = 6;

		graphics::DrawGUIBox(&acv::GetT(acv::t_gui_select_box),
			xoffs - 12 - boxoffs,
			xoffs + 12 + boxoffs,
			yoffs - 12 - boxoffs,
			yoffs + 12, 8, 8 + boxoffs);
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
		ECActor* chara = ACTOR(players[activePlayer]);

		// Draw message
		if (gui.message_time[activePlayer] > tickCount) {
			gui.text_message[activePlayer].SetOffset(0, (90u - (gui.message_time[activePlayer] - tickCount)) / 4);
			gui.text_message[activePlayer].Draw(&acv::GetT(acv::t_gui_font),
				(float)(gui.message_time[activePlayer] - tickCount) / 90.f);
		}

		// Draw hurt effect
		if (ENTITY(players[activePlayer])->damagestate < gui.guiPlayerHP[activePlayer]) {
			graphics::DrawGUITexture(&acv::GetT(acv::t_gui_hurt), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(),
				(lf32)(gui.guiPlayerHP[activePlayer] - ENTITY(players[activePlayer])->damagestate) * (10.f / 1000.f));
			gui.guiPlayerHP[activePlayer] =
				m::StepToward(gui.guiPlayerHP[activePlayer],
					ENTITY(players[activePlayer])->damagestate, 5);
		}
		else {
			gui.guiPlayerHP[activePlayer] = ENTITY(players[activePlayer])->damagestate;
		}

		// Draw crosshairs
		if (!config.b3PP) {
			if (config.bCrossHairs) graphics::DrawGUITexture(&acv::GetT(acv::t_gui_crosshair), 0, 0, 32, 32);
		}

		// inventory
		GUIDrawInventory(&chara->inventory, chara->inv_active_slot);

		{
			ECCommon* player = ENTITY(players[activePlayer]);
			// hp
			lf32 hp = (lf32)ENTITY(players[activePlayer])->damagestate / 1000.f;
			lf32 hp2 = (lf32)gui.guiPlayerHP[activePlayer] / 1000.f;
			lf32 hpscale = 128.f;
			graphics::DrawGUITexture(&acv::GetT(acv::t_col_black),
				p1_x_start + (hpscale * 0.5f), p1_y_start + 8, (int)hpscale, 16);
			graphics::DrawGUITexture(&acv::GetT(acv::t_gui_bar_yellow),
				p1_x_start + roundf(hp2 * (hpscale * 0.5f)), p1_y_start + 8,
				(int)ceilf(hp2 * hpscale), 16);
			graphics::DrawGUITexture(&acv::GetT(acv::t_gui_bar_red),
				p1_x_start + roundf(hp * (hpscale * 0.5f)), p1_y_start + 8,
				(int)ceilf(hp * hpscale), 16);
			char stuff[32];
			snprintf(stuff, 32, "%iHP", player->damagestate);
			gui.text_hp.ReGen(stuff, p1_x_start + 4, p1_x_start + 128, p1_y_start + 13);
			gui.text_hp.Draw(&acv::GetT(acv::t_gui_font));
			// effects
			int effoffs = 1;
			for (int i = 0; i < player->effects.Size(); ++i) {
				if (!player->effects.Used(i)) continue;
				graphics::DrawGUITexture(&acv::GetT(player->effects[i].effect_icon), p1_x_start + 128 + effoffs * 16, p1_y_start + 8, 16, 16);
				++effoffs;
			}
		}

		// enemy hp
		LtrID viewtarget = ACTOR(players[activePlayer])->viewtarget;
		// If not null or player and exists
		if (GetEntityExists(viewtarget) && !IDCOMPARE(viewtarget, core::players[activePlayer])) {
			ECCommon* entity = ENTITY(viewtarget);

			//* GET TARGET SCREENSPACE POS
			glm::vec4 glpos = glm::vec4(entity->t.position.x, entity->t.altitude, -entity->t.position.y, 1.f);
			glm::mat4 matv = graphics::GetMatView();
			glm::mat4 matp = graphics::GetMatProj();
			glm::vec4 pr = matp * matv * glpos;
			pr.x /= pr.w;
			pr.y /= pr.w;
			pr.z /= pr.w;
			pr.x *= graphics::FrameSizeX() * 0.5f;
			pr.y *= graphics::FrameSizeY() * 0.5f;
			//*/

			if (pr.w > 0.f) { // If the target is actually in front of us
				int target_x_start = (li32)pr.x;
				int target_y_start = (li32)pr.y;

				int textboxX = target_x_start + 24;
				int textboxY = target_y_start - 24;

				// Clamp textbox
				if (textboxX > (li32)graphics::FrameSizeX() / 2 - 128 - 12)
					textboxX = (li32)graphics::FrameSizeX() / 2 - 128 - 12;
				else if (textboxX < -(li32)graphics::FrameSizeX() / 2 + 12)
					textboxX = -(li32)graphics::FrameSizeX() / 2 + 12;
				if (textboxY > (li32)graphics::FrameSizeY() / 2 - 12)
					textboxY = (li32)graphics::FrameSizeY() / 2 - 12;
				else if (textboxY < -(li32)graphics::FrameSizeY() / 2 + 24)
					textboxY = -(li32)graphics::FrameSizeY() / 2 + 24;

				// Draw its name
				gui.text_temp.ReGen(EntityName(viewtarget.Index()), textboxX, textboxX + 128, textboxY);
				gui.guibox.ReGen(textboxX, textboxX + gui.text_temp.sizex, textboxY - gui.text_temp.sizey, textboxY, 4, 10);
				// draw our enemy's health
				if (GetEntityExists(ACTOR(players[activePlayer])->atk_target)) {
					lf32 hp = (lf32)ACTOR(ACTOR(players[activePlayer])->atk_target)->damagestate / 1000.f;
					lf32 hpscale = 128.f;
					graphics::DrawGUITexture(&acv::GetT(acv::t_col_black),
						p1_x_start + (hpscale * 0.5f), p1_y_start + 24, (int)hpscale, 16);
					graphics::DrawGUITexture(&acv::GetT(acv::t_gui_bar_yellow),
						p1_x_start + roundf(hp * hpscale * 0.5f), p1_y_start + 24,
						(int)ceilf(hp * hpscale), 16);
				}
				// draw the thing
				gui.guibox.Draw(&acv::GetT(acv::t_gui_box), 0.75f);
				gui.text_temp.Draw(&acv::GetT(acv::t_gui_font), 1.f);
				// draw the pick-up icon
				if (GetEntityType(viewtarget.Index()) == ENTITY_TYPE_RESTING_ITEM)
					graphics::DrawGUITexture(&acv::GetT(acv::t_gui_icon_pick_up), target_x_start, target_y_start, 24, 24);
				else if (GetEntityType(viewtarget.Index()) == ENTITY_TYPE_ACTOR)
					graphics::DrawGUITexture(&acv::GetT(acv::t_gui_icon_hold_hand), target_x_start, target_y_start, 24, 24);
			}
		}
	
		// VICTORY CONDITION HANDLED IN GUI DRAW LMAO
		#if DEF_PROJECT == PROJECT_BC
		if (activePlayer == 0) {
			char stuff[32];
			snprintf(stuff, 32, "Mushrooms Collected: %i of 12", ACTOR(players[activePlayer])->inventory.CountItemsOfTemplate(1u));
			// reuse health text 4 fun
			gui.text_hp.ReGen(stuff, p1_x_start + 8, 0, p1_y_start + graphics::FrameSizeY() - 16);
			gui.text_hp.Draw(&acv::GetT(acv::t_gui_font));
		}
		#endif
	}

	void DrawPostDraw(lf64 delta)
	{
		#ifdef _DEBUG
		gui.text_version.Draw(&acv::GetT(acv::t_gui_font));
		char buffer[16];
		int i = snprintf(buffer, 16, "%f", 1.f / delta);
		gui.text_fps.ReGen(buffer, config.iWinX * -0.5f, config.iWinX * 0.5f, config.iWinY * 0.5f - 24.f);
		gui.text_fps.Draw(&acv::GetT(acv::t_gui_font));
		//graphics::DrawGUITexture(&acv::GetT(acv::t_cursor), cursor_x, cursor_y, 128, 128);
		// draw archiver loaded list
		for (int i2 = 0; i2 < acv::AssetCount(); ++i2)
			if (acv::IsLoaded(i2))
				graphics::DrawGUITexture(&acv::GetT(acv::t_debug_loaded_y), 2 + i2 * 4 - ((li32)config.iWinX / 2), ((li32)config.iWinY / 2) - 40, 4, 8);
			else
				graphics::DrawGUITexture(&acv::GetT(acv::t_debug_loaded_n), 2 + i2 * 4 - ((li32)config.iWinX / 2), ((li32)config.iWinY / 2) - 40, 4, 8);
		#endif
		#if DEF_PROJECT == PROJECT_BC
		if (gui.bShowGuide) {
			gui.guideW = m::StepToward(gui.guideW, 512, 12);
			graphics::DrawGUITexture(&acv::GetT(acv::t_gui_guide), 0, 0, gui.guideW, 512, 1.f);
			//graphics::DrawGUITexture(&acv::GetT(acv::t_gui_guide), 0, 0, guideW, guideW, 1.f);
		}
		else {
			gui.text_guidehelp.Draw(&acv::GetT(acv::t_gui_font));
		}
		#endif
	}

	void SetPlayerInput(ID16 playerIndex, m::Vector2 input, lf32 rot_x, lf32 rot_y,
		bool use, bool use_hit, bool use_alt,
		bool run, bool aim, bool ACTION_A, bool ACTION_B, bool ACTION_C,
		bool crouch, bool jump) {
		ECActor* actor = ACTOR(players[playerIndex]);
		actor->input.move = m::Rotate(input, actor->viewYaw.Rad()) * m::Vector2(-1.f, 1.f);
		actor->viewYaw.Rotate(rot_x);
		actor->viewPitch.RotateClamped(rot_y, -80.f, 70.f);
		actor->input.bits.setto(IN_USE, use);
		actor->input.bits.setto(IN_USE_HIT, use_hit);
		actor->input.bits.setto(IN_USE_ALT, use_alt);
		actor->input.bits.setto(IN_RUN, run);
		actor->input.bits.setto(IN_AIM, aim);
		actor->input.bits.setto(IN_ACTN_A, ACTION_A);
		actor->input.bits.setto(IN_ACTN_B, ACTION_B);
		actor->input.bits.setto(IN_ACTN_C, ACTION_C);
		actor->input.bits.setto(IN_CROUCH, crouch);
		actor->input.bits.setto(IN_JUMP, jump);
	}

	void CheckPlayerAI() {
		#if DEF_NMP
		for (int i = 0; i < config.iNumNWPlayers; ++i)
			ACTOR(players[i])->aiControlled = false;
		#else
		ACTOR(players[0])->aiControlled = false;
		if (config.bSplitScreen)
			ACTOR(players[1])->aiControlled = false;
		else
			ACTOR(players[1])->aiControlled = true;
		#endif
	}

	//________________________________________________________________________________________________________________________________
	// SPAWN FUNCTIONS ---------------------------------------------------------------------------------------------------------------

	LtrID SpawnEntity(lui8 type, m::Vector2 pos, float dir)
	{
		LtrID id = IndexSpawnEntity(ENTITY_TYPE_ACTOR);
		if (IDCHECK(id)) {
			PrefabEntity[type](id, pos, dir);
		}
		else {
			std::cout << "Could not spawn entity, ran out of space" << std::endl;
		}
		return id;
	}
	LtrID SpawnNewEntityItem(ID16 item_template, m::Vector2 pos, lf32 dir)
	{
		LtrID id = IndexSpawnEntity(ENTITY_TYPE_RESTING_ITEM);
		if (IDCHECK(id)) {
			PrefabCommon(id, pos, dir);
			ENTITY(id)->faction = fac::faction::none;
			ENTITY(id)->physicsFlags.set(ECCommon::ePREFAB_ITEM);
			ENTITY(id)->activeFlags.set(ECCommon::eALIVE);
			ITEM(id)->item_instance = SpawnItem(item_template);
			ENTITY(id)->radius = acv::items[((HeldItem*)GetItemInstance(ITEM(id)->item_instance.Index()))->id_item_template]->f_radius;
			ENTITY(id)->height = 0.5f;
		}
		return id;
	}
	LtrID SpawnEntityItem(LtrID itemid, m::Vector2 pos, lf32 height, lf32 dir)
	{
		LtrID id = IndexSpawnEntity(ENTITY_TYPE_RESTING_ITEM);
		if (IDCHECK(id)) {
			//spawn_setup_t(id, pos, dir);
			ENTITY(id)->t.position = pos;
			ENTITY(id)->velocity = 0.f;
			ENTITY(id)->altitude_velocity = 0.f;
			ENTITY(id)->t.yaw.Set(dir);
			GetCellSpaceInfo(ENTITY(id)->t.position, ENTITY(id)->t.csi);
			//env::GetHeight(ENTITY(id)->t.height, ENTITY(id)->t.csi);
			//ENTITY(id)->t.height += 1.f; // temp
			ENTITY(id)->t.altitude = height;
			AddEntityCell(ENTITY(id)->t.csi.c[eCELL_I].x, ENTITY(id)->t.csi.c[eCELL_I].y, id);
			ENTITY(id)->activeFlags.set(ECCommon::eALIVE);
			ENTITY(id)->damagestate = STATE_DAMAGE_MAX;
			ENTITY(id)->faction = fac::faction::none;
			ENTITY(id)->physicsFlags.set(ECCommon::ePREFAB_ITEM);
			ENTITY(id)->activeFlags.set(ECCommon::eALIVE);
			ITEM(id)->item_instance = itemid;
			ENTITY(id)->radius = acv::items[((HeldItem*)GetItemInstance(itemid.Index()))->id_item_template]->f_radius;
			ENTITY(id)->height = 0.5f;
		}
		return id;
	}
	void DestroyEntity(LtrID id)
	{
		if (GetEntityExists(id)) {
			// TODO: add ondelete function for entity
			IndexDeleteEntity(id.Index());
			printf( "Destroyed entity %i\n", id);
		}
	}

	LtrID SpawnItem(ID16 item_template)
	{
		LtrID id = InitItemInstance(acv::item_types[item_template]);
		if (IDCHECK(id))
		{
			GETITEMINST(id)->id_item_template = item_template;
			ItemInit(id);
			std::cout << "Created item " << id.GUID() << std::endl;
		}
		else
		{
			std::cout << "Failed to create item!" << std::endl;
		}
		return id;
	}
	void DestroyItem(LtrID id)
	{
		FreeItemInstance(id.GUID());
		std::cout << "Destroyed item " << id.GUID() << std::endl;
	}

	void SpawnProjectile(fac::faction faction, ID16 type, m::Vector2 pos, lf32 height,
		float yaw, float pitch)
	{
		PrjID id = IndexSpawnProjectile();
		Projectile* proj = GetProj(id);

		proj->t.position_x = pos.x;
		proj->t.position_y = pos.y;
		proj->t.position_h = height;

		//
		/*
		lf32 r = 5.f;
		lf32 pitch_x = cos(editor.cam_pitch.Rad());
		lf32 pitch_y = sin(editor.cam_pitch.Rad());
		lf32 yaw_x = cos(editor.cam_yaw.Rad());
		lf32 yaw_y = sin(editor.cam_yaw.Rad());
		viewPosition[0] = viewTarget[0] + r * m::Vector3(-yaw_y * pitch_x, pitch_y, -yaw_x * pitch_x);
		*/

		proj->t.velocity_h = -sin(pitch);
		proj->t.velocity_x = (m::AngToVec2(yaw) * cos(pitch)).x; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj->t.velocity_y = (m::AngToVec2(yaw) * cos(pitch)).y; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj->ttd = tickCount + 60u;
		proj->faction = faction;
		proj->type = type;

		// save check
		if (acv::projectiles[type].saveOnHit) {
			saveNextFrame = true;
			printf("Projectile Save\n");
		}
	}
	void SpawnProjectileSpread(fac::faction faction, ID16 type, m::Vector2 pos, lf32 height,
		float yaw, float pitch, float spread)
	{
		yaw += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
		pitch += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

		SpawnProjectile(faction, type, pos, height, yaw, pitch);
	}

	void DestroyProjectile(ID16 id)
	{
		IndexDestroyProjectileC(MakePrjID(id));
	}

	//________________________________________________________________________________________________________________________________
	// PROJECTILES -------------------------------------------------------------------------------------------------------------------

	bool ProjectileCollideEnv(ID16 index)
	{
		Projectile* proj = GetProj(MakePrjID(index));

		#if DEF_GRID
		int x = (int)roundf(proj->t.position_x);
		int y = (int)roundf(proj->t.position_y);
		if (env::Get(x, y, env::eflag::EF_IMPASSABLE)) // if hit an impassable tile
			return true;
		#endif

		CellSpace csi;
		GetCellSpaceInfo(m::Vector2(proj->t.position_x, proj->t.position_y), csi);
		lf32 height;
		#if DEF_GRID
		env::GetHeight(height, csi);
		#else
		env::GetNearestSurfaceHeight(height, csi, proj->t.position_h);
		#endif
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

	void ProjectileTick(lf32 dt) // Projectile tick
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
				DrawMesh(acv::GetM(acv::projectiles[proj[index].type].mesh),
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
					lui16 x = (lui16)roundf(proj[index].t.position_x);
					lui16 y = (lui16)roundf(proj[index].t.position_y);
					m::Vector2 vec = m::Normalize(m::Vector2(proj[index].t.velocity_x, proj[index].t.velocity_y));
					lui16 x2 = (lui16)((li16)x + (li16)roundf(vec.x));
					lui16 y2 = (lui16)((li16)y + (li16)roundf(vec.y));
					//t_EnvHeightmap.SetPixelChannelG((lui16)roundf(proj[index].t.position.x), (lui16)roundf(proj[index].t.position.y), 255ui8);

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
					for (int i = 0; i < GetEntityArraySize(); i++)
					{
						if (AnyEntityHere(i) && GetEntity<ECCommon>(i)->physicsFlags.get(ECCommon::eCOLLIDE_PRJ))
						{
							if (fac::GetAllegiance(GetEntity<ECCommon>(i)->faction, (fac::faction)proj[index].faction) != fac::allied)
							{
								//check height difference
								if (proj[index].t.position_h > GetEntity<ECCommon>(i)->t.altitude && proj[index].t.position_h < GetEntity<ECCommon>(i)->t.altitude + GetEntity<ECCommon>(i)->height)
								{
									//get difference between positions
									m::Vector2 vec = m::Vector2(proj[index].t.position_x, proj[index].t.position_y) - GetEntity<ECCommon>(i)->t.position;
									//get distance
									float dist = m::Length(vec);
									if (dist < 0.5f)
									{
										// TODO: pass angle to damage fn
										GetEntity<ECCommon>(i)->Damage(acv::projectiles[proj[index].type].damage, glm::degrees(m::Vec2ToAng(vec)));
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
												ENTITY(ID)->hp -= 0.5f;
												if (ENTITY(ID)->hp <= 0.f)
												{
													ENTITY(ID)->properties.unset(ECCommon::eALIVE);
													ENTITY(ID)->hp = 0.f;
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
