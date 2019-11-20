#include "index.h"
#include "index_util.h"
#include "index_fn.cpp"
#include "weather.h"

unsigned int activePlayer = 0u;

namespace index
{
	btID viewtarget[2]{ ID_NULL,ID_NULL };
	btID viewtarget_last_tick[2]{ ID_NULL,ID_NULL };

	//// temp
	//btui32 GetCellX()
	//{
	//	return ENTITY(0)->t.cellx;
	//}
	////temp
	//btui32 GetCellY()
	//{
	//	return ENTITY(0)->t.celly;
	//}

	// temp
	#define GetCellX ENTITY(0)->csi.c[0].x
	//temp
	#define GetCellY ENTITY(0)->csi.c[0].y

	btf32 GetHP(btID id)
	{
		return ENTITY(id)->state.hp;
	}

	void SetViewTargetID(btID id)
	{
		viewtarget_last_tick[activePlayer] = viewtarget[activePlayer];
		viewtarget[activePlayer] = id;
		//std::cout << "looking at id " << viewtarget << std::endl;
	}
	btID GetViewTargetID(btui32 player)
	{
		return viewtarget[player];
	}

	void* GetEntity(btID id)
	{
		return _entities[id];
	}

	void SetViewFocus(btID index)
	{
		activePlayer = index;
		viewpos = ENTITY(activePlayer)->t.position * -1.f;

		#define h 1.6f + m::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.aniTime / resAniStep.time)
		cfg::bEditMode ?
			graphics::SetMatViewEditor(&CHARA(activePlayer)->t_head) :
			graphics::SetMatView(&CHARA(activePlayer)->t_head);
		#undef h
	}

	m::Vector2 GetViewOffset()
	{
		return viewpos;
	}

	enum flood_fill_type
	{
		r_ambient,
		g_light,
		b_unused,
		a_unused,
	};

	void flood_fill_temp(btui16 x, btui16 y, graphics::colour col)
	{
		//t_EnvLightmap.SetPixelChannelG(x, y, col.g);
		if (!env::Get(x, y, env::eflag::eIMPASSABLE) && t_EnvLightmap.GetPixel(x, y).g < col.g)
		{
			t_EnvLightmap.SetPixelChannelG(x, y, col.g);
			if (col.g > 31ui8)
			{
				col.g -= 32ui8;
				flood_fill_temp(x + 1, y, col);
				flood_fill_temp(x - 1, y, col);
				flood_fill_temp(x, y + 1, col);
				flood_fill_temp(x, y - 1, col);
			}
		}
	}

	Transform3D t_moon;

	GLuint shadowtex;
	glm::mat4 shadowmat_temp;
	void SetShadowTexture(btui32 id)
	{
		shadowtex = id;
	}

	void UpdateGlobalShaderParams()
	{
		graphics::shader_terrain.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_sky).glID); // Bind the texture

		// terrain textures
		glActiveTexture(GL_TEXTURE6); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "tt1"), 6);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_terrain_scorch).glID); // Bind the texture

		graphics::shader_solid.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_sky).glID); // Bind the texture

		graphics::shader_solidChara.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidChara.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidChara.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidChara.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_sky).glID); // Bind the texture

		graphics::shader_solidBlend.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlend.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlend.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlend.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_sky).glID); // Bind the texture

		graphics::shader_solidBlendChara.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlendChara.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlendChara.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solidBlendChara.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetT(res::t_sky).glID); // Bind the texture
	}

	void Init()
	{
		char buffinal[32] = "TSOA R";
		char buffer[4];
		char buffer2[4];
		//char buffer3[4];
		_itoa(VERSION_MAJOR, buffer, 10);
		_itoa(VERSION_MINOR, buffer2, 10);
		//_itoa(VERSION_BUILD, buffer3, 10);
		strcat(buffinal, buffer);
		strcat(buffinal, "-");
		strcat(buffinal, buffer2);
		//strcat(buffinal, " b");
		//strcat(buffinal, buffer3);
		strcat(buffinal, " ");
		strcat(buffinal, VERSION_COMMENT);

		text_version.ReGen(buffinal, cfg::iWinX * -0.25f, 512, cfg::iWinY * 0.5f);

		//temp
		t_moon.SetRotation(glm::radians(0.f));
		t_moon.SetPosition(m::Vector3(0.f, 0.f, 0.f));
		t_moon.SetScale(m::Vector3(1.f,1.f,1.f));

		env::LoadBin();

		/*
		graphics::ModifiableTexture modt;
		modt.LoadFile("res/TTerrain");

		for (int x = 0; x < WORLD_SIZE; x++)
		{
			for (int y = 0; y < WORLD_SIZE; y++)
			{
				//env::UnSet(x, y, env::eflag::eIMPASSABLE);
				if (x < modt.width && y < modt.height)
					env::eCells[x][y].height = modt.GetPixel(x, y).r;
			}
		}
		//env::GeneratePhysicsSurfaces();

		//env::SaveBin();
		//*/

		/*
		for (btui16 x = 0; x < WORLD_SIZE; x++)
		{
			for (btui16 y = 0; y < WORLD_SIZE; y++)
			{
				env::eCells[x][y].model = ID_NULL;
			}
		}
		env::SaveBin();
		*/

		// Lightmap

		t_EnvLightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(255ui8, 0ui8, 0ui8, 255ui8));

		flood_fill_temp(1024, 1024, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));
		flood_fill_temp(1005, 1003, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));
		flood_fill_temp(1016, 1016, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));

		t_EnvLightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		// Heightmap

		t_EnvHeightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(0ui8, 0ui8, 0ui8, 0ui8));

		for (btui16 x = 0; x < WORLD_SIZE; x++)
			for (btui16 y = 0; y < WORLD_SIZE; y++)
				t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);
		
		t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		// Spawnz

		//players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1.f, 1.f), 0.f);
		//players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(4.f, 4.f), 0.f);
		if (cfg::bEditMode)
			players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
			//players[0] = SpawnEntity(prefab::PREFAB_EDITORPAWN, m::Vector2(1024.f, 1024.f), 0.f);
		else
			players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
		players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1020.f), 0.f);
		CHARA(players[1])->t_skin = 1u;
		CHARA(players[1])->faction = fac::playerhunter;

		SpawnItem(0ui16, m::Vector2(1025.1f, 1022.6f), 15.f);
		SpawnItem(4ui16, m::Vector2(1025.5f, 1024.0f), 120.f);
		SpawnItem(1ui16, m::Vector2(1025.5f, 1023.3f), 15.f);
		SpawnItem(2ui16, m::Vector2(1025.8f, 1024.f), 15.f);
		SpawnItem(3ui16, m::Vector2(1026.8f, 1026.f), 15.f);

		if (!cfg::bEditMode)
		{
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_ai_player, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_ai_player, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			//SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);

			// This is going to blow up extremely fast if I don't automate it somehow

			fac::SetAllegiance(fac::player, fac::player, fac::allied);
			fac::SetAllegiance(fac::player, fac::playerhunter, fac::enemy);
			//fac::SetAllegiance(fac::player, fac::playerhunter, fac::neutral);
			fac::SetAllegiance(fac::player, fac::undead, fac::enemy);

			fac::SetAllegiance(fac::undead, fac::player, fac::enemy);
			fac::SetAllegiance(fac::undead, fac::playerhunter, fac::enemy);
			fac::SetAllegiance(fac::undead, fac::undead, fac::allied);

			fac::SetAllegiance(fac::playerhunter, fac::undead, fac::enemy);
			fac::SetAllegiance(fac::playerhunter, fac::player, fac::enemy);
			//fac::SetAllegiance(fac::playerhunter, fac::player, fac::neutral);
			fac::SetAllegiance(fac::playerhunter, fac::playerhunter, fac::allied);
		}

		graphics::SetMatProj(); // does not need to be continually called
		UpdateGlobalShaderParams();
	}
	//destroy all data
	void End()
	{
		for (int i = 0; i < BUF_SIZE; i++)
		//for (int i = 0; i <= block_entity.index_end; i++)
		{
			if (block_entity.used[i])
				DestroyEntity(i);
		}
	}

	btf64 spawnz_time_temp = 0.;

	void Tick(btf32 dt)
	{
		/*
		if (!cfg::bEditMode)
		{
			if (spawnz_time_temp < time)
			{
				spawnz_time_temp = time + 20.;
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_zombie, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_ai_player, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
				SpawnEntity(prefab::prefab_npc, m::Vector2(m::Random(896, 1152), m::Random(896, 1152)), 0.f);
			}
		} //*/

		t_moon.Rotate(-0.01f * dt, m::Vector3(1.f, 0.f, 0.f));

		//-------------------------------- ITERATE THROUGH ENTITIES

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
			if (block_entity.used[i]) ENTITY(i)->Tick(i, dt);

		ProjectileTick(dt);
		ProjectileTick(dt); // temp

		//temporary destroy dead entities
		///*
		for (int i = 0; i <= block_entity.index_end; i++)
			if (block_entity.used[i])
				if (ACTOR(i)->state.hp == 0.f && ACTOR(i)->aiControlled)
					//temp
					index::DestroyEntity(i);
		//*/

		if (cfg::bEditMode)
		{
			if (input::GetHit(input::key::USE))
			{
				//env::GeneratePaths();
			}
			if (input::GetHit(input::key::ACTIVATE))
			{
				env::GeneratePhysicsSurfaces();
				env::SaveBin();
			}
			if (input::GetHit(input::key::ACTION_A))
			{
				env::Get(GetCellX, GetCellY, env::eflag::eIMPASSABLE) ? env::UnSet(GetCellX, GetCellY, env::eflag::eIMPASSABLE) : env::Set(GetCellX, GetCellY, env::eflag::eIMPASSABLE);
			}
			if (input::GetHit(input::key::ACTION_B))
			{
				--env::eCells[GetCellX][GetCellY].model;
				while(!res::IsMesh(env::eCells[GetCellX][GetCellY].model))
					--env::eCells[GetCellX][GetCellY].model;
			}
			else if (input::GetHit(input::key::ACTION_C))
			{
				++env::eCells[GetCellX][GetCellY].model;
				while (!res::IsMesh(env::eCells[GetCellX][GetCellY].model))
					++env::eCells[GetCellX][GetCellY].model;
			}
		}
	}

	void Draw(bool oob)
	{
		//btf32 time2 = time * 0.0001f + 0.3f; // decent timescale for now
		//btf32 time2 = time * 0.0002f + 0.22f;
		btf32 time2 = Time::time * 0.0006f + 0.25f;
		//btf32 time2 = Time::time * 0.05f + 0.2f;
		//btf32 time2 = time * 0.001f + 0.1f;

		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 8.f) / 8.f) + 180.f));
		//glm::vec3 sunrot2 = glm::vec3(sunrot.x, sunrot.y, 0.f);
		glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(sunrot.x, sunrot.y, sunrot.y * 1.34f));


		graphics::Matrix4x4 matrix; // Matrix used for rendering env. props (so far...)

		if (oob)
		{
			//-------------------------------- UPDATE SHADERS

			#define PCAM (glm::vec3)graphics::GetViewPos()

			graphics::shader_terrain.Use();
			graphics::shader_terrain.setMat4("lightProj", shadowmat_temp);
			graphics::shader_terrain.SetFloat("ft", (float)time2);
			graphics::shader_terrain.setVec3("pcam", PCAM);
			graphics::shader_terrain.setVec3("vsun", sunrot2);
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_solid.Use();
			graphics::shader_solid.setMat4("lightProj", shadowmat_temp);
			graphics::shader_solid.SetFloat("ft", (float)time2);
			graphics::shader_solid.setVec3("pcam", PCAM);
			graphics::shader_solid.setVec3("vsun", sunrot2);
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_solidChara.Use();
			graphics::shader_solidChara.setMat4("lightProj", shadowmat_temp);
			graphics::shader_solidChara.SetFloat("ft", (float)time2);
			graphics::shader_solidChara.setVec3("pcam", PCAM);
			graphics::shader_solidChara.setVec3("vsun", sunrot2);
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_solidChara.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_solidBlend.Use();
			graphics::shader_solidBlend.setMat4("lightProj", shadowmat_temp);
			graphics::shader_solidBlend.SetFloat("ft", (float)time2);
			graphics::shader_solidBlend.setVec3("pcam", PCAM);
			graphics::shader_solidBlend.setVec3("vsun", sunrot2);
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_solidBlend.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_solidBlendChara.Use();
			graphics::shader_solidBlendChara.setMat4("lightProj", shadowmat_temp);
			graphics::shader_solidBlendChara.SetFloat("ft", (float)time2);
			graphics::shader_solidBlendChara.setVec3("pcam", PCAM);
			graphics::shader_solidBlendChara.setVec3("vsun", sunrot2);
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_solidBlendChara.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_sky.Use();
			graphics::shader_sky.SetFloat("ft", (float)time2);
			graphics::shader_sky.setVec3("pcam", PCAM);
			graphics::shader_sky.setVec3("vsun", sunrot2);

			#undef PCAM

			//-------------------------------- DRAW SKY

			graphics::SetFrontFace();
			graphics::SetMatProj();


			t_moon.SetPosition(m::Vector3(ENTITY(activePlayer)->t.position.x, ENTITY(activePlayer)->t.height + 1.f, ENTITY(activePlayer)->t.position.y));


			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// Setup sky matrix
			graphics::MatrixTransform(matrix, m::Vector3(ENTITY(activePlayer)->t.position.x, ENTITY(activePlayer)->t.height + 1.f, ENTITY(activePlayer)->t.position.y));
			// Draw stars
			glDisable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			//DrawMesh(ID_NULL, res::m_skystars, res::t_sky, graphics::shader_sky, matrix);
			// Draw Moon
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			graphics::shader_solid.Use();
			graphics::shader_solid.SetBool("lit", false);
			DrawMeshAtTransform(ID_NULL, res::GetM(res::m_skymoon), res::GetT(res::t_skymoon), graphics::shader_solid, t_moon);
			graphics::shader_solid.Use();
			graphics::shader_solid.SetBool("lit", true);
			// Draw Sky
			glBlendFunc(GL_ONE, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			DrawMesh(ID_NULL, res::GetM(res::m_skydome), res::GetT(res::t_sky), graphics::shader_sky, matrix);
			// Reset
			glBlendFunc(GL_ONE, GL_ZERO);
			glDisable(GL_BLEND);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//-------------------------------- DRAW OOB TERRAIN

			//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			graphics::SetMatProj(128.f); // Set projection matrix for long-distance rendering

			graphics::MatrixTransform(matrix, m::Vector3(0.f, 0.f, 0.f));
			DrawMesh(ID_NULL, res::GetM(res::m_terrain_oob), res::GetT(res::t_terrain_sanddirt), graphics::shader_terrain, matrix);

			graphics::MatrixTransform(matrix, m::Vector3(2048.f, 340.f, 2048.f), 34.f);
			//graphics::MatrixTransform(matrix, m::Vector3(2048.f, 340.f, 2048.f));
			DrawMesh(ID_NULL, res::GetM(res::m_ex1e_air_carrier), res::GetT(res::t_default), graphics::shader_solid, matrix);

			graphics::SetMatProj(); // Reset projection matrix

			//-------------------------------- DRAW ENTITIES

			glClear(GL_DEPTH_BUFFER_BIT); // Clear for foreground rendering

			// ...
		}
		else
		{
			// Set light matrices
			graphics::SetMatProjLight(); graphics::SetMatViewLight(
				ENTITY(activePlayer)->t.position.x,
				ENTITY(activePlayer)->t.height,
				-ENTITY(activePlayer)->t.position.y,
				-sunrot2.x, -sunrot2.y, -sunrot2.z);
			shadowmat_temp = graphics::GetMatProj() * graphics::GetMatView();
		}

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
			if (block_entity.used[i]) ENTITY(i)->Draw(i); // Draw entities
		if (!cfg::bEditMode) if (oob) ProjectileDraw(); // Draw projectiles

		/*for (int i = 0; i < 4; ++i)
		{
			graphics::MatrixTransform(matrix, m::Vector3(ACTOR(players[active_player_view])->csi.c[i].x, 0.f, ACTOR(players[active_player_view])->csi.c[i].y));
			DrawMesh(ID_NULL, res::m_debug_bb, res::t_debug_bb, graphics::shader_terrain, matrix);
		}*/

		//-------------------------------- DRAW ENVIRONMENT

		//if (oob)
		{
			matrix = graphics::Matrix4x4();
			graphics::MatrixTransform(matrix, m::Vector3(roundf(ENTITY(activePlayer)->t.position.x / 8) * 8, 0.f, roundf(ENTITY(activePlayer)->t.position.y / 8) * 8));
			DrawMesh(ID_NULL, res::GetM(res::m_terrain_near), res::GetT(res::t_terrain_sanddirt), graphics::shader_terrain, matrix);

			if (cfg::bEditMode)
			{
				btui32 drawrange = 32u; // Create min/max draw coordinates
				bti32 minx = ENTITY(activePlayer)->csi.c[0].x - drawrange; if (minx < 0) minx = 0;
				bti32 maxx = ENTITY(activePlayer)->csi.c[0].x + drawrange; if (maxx > WORLD_SIZE - 1) maxx = WORLD_SIZE - 1;
				bti32 miny = ENTITY(activePlayer)->csi.c[0].y - drawrange; if (miny < 0) miny = 0;
				bti32 maxy = ENTITY(activePlayer)->csi.c[0].y + drawrange; if (maxy > WORLD_SIZE - 1) maxy = WORLD_SIZE - 1;

				for (bti32 x = minx; x <= maxx; x++)
				{
					for (bti32 y = miny; y < maxy; y++)
					{
						if (env::Get(x, y, env::eflag::eIMPASSABLE))
						{
							graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
							DrawMesh(ID_NULL, res::GetM(res::m_debug_bb), res::GetT(res::t_debug_bb), graphics::shader_solid, matrix);
						}
						if (env::eCells[x][y].model != ID_NULL)
						{
							graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
							DrawMesh(ID_NULL, res::GetM(env::eCells[x][y].model), graphics::shader_solid, matrix);
						}
					}
				}
			}
			else
			{
				btui32 drawrange = 32u; // Create min/max draw coordinates
				bti32 minx = ENTITY(activePlayer)->csi.c[0].x - drawrange; if (minx < 0) minx = 0;
				bti32 maxx = ENTITY(activePlayer)->csi.c[0].x + drawrange; if (maxx > WORLD_SIZE - 1) maxx = WORLD_SIZE - 1;
				bti32 miny = ENTITY(activePlayer)->csi.c[0].y - drawrange; if (miny < 0) miny = 0;
				bti32 maxy = ENTITY(activePlayer)->csi.c[0].y + drawrange; if (maxy > WORLD_SIZE - 1) maxy = WORLD_SIZE - 1;

				for (bti32 x = minx; x <= maxx; x++)
				{
					for (bti32 y = miny; y < maxy; y++)
					{
						if (env::eCells[x][y].model != ID_NULL)
						{
							graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
							DrawMesh(ID_NULL, res::GetM(env::eCells[x][y].model), graphics::shader_solid, matrix);
						}
					}
				}
			}
		}

		#ifdef DEF_DEBUG_DISPLAY

		graphics::SetFrontFaceInverse();

		//draw p1 cell box

		line_transform.SetPosition(fw::Vector3(roundf(player1->t.position.x / 4.f) * 4.f, 0, roundf(player1->t.position.y / 4.f) * 4.f));
		line_transform.SetRotation(0.f);
		line_transform.SetScale(fw::Vector3(1, 1, 1));
		//line_transform.Draw(graphics::mdl_debug_cell, graphics::shader_solid, graphics::tex_yel);
		line_transform.Draw(graphics::mdl_debug_cell, graphics::shader_solid);


		//draw p1 cell lines

		int x_temp = (roundf((player1->t.position.x) / 4.f)) + 8;
		int y_temp = (roundf((player1->t.position.y) / 4.f)) + 8;

		float f = cells[x_temp][y_temp].ents.size() * 0.25f;
		line_transform.SetScale(fw::Vector3(f, 1.f, f));
		line_transform.Draw(graphics::mdl_debug_cell, graphics::shader_solid);

		for (int i = 0; i < cells[x_temp][y_temp].lvec.size(); i++)
		{
			line_transform.SetPosition(fw::Vector3(cells[x_temp][y_temp].lvec[i]->position.x, 0.f, cells[x_temp][y_temp].lvec[i]->position.y));
			line_transform.SetRotation(-cells[x_temp][y_temp].lvec[i]->rotation);
			line_transform.SetScale(fw::Vector3(cells[x_temp][y_temp].lvec[i]->scale.x * 2, 1.f, 1.f));
			//line_transform.Draw(graphics::mdl_debug_line, graphics::shader_solid, graphics::tex_yel);
			line_transform.Draw(graphics::mdl_debug_line, graphics::shader_solid);
		}

		#endif
	}

	void TickGUI()
	{
		if (activePlayer == 0u)
		{
			if (input::GetHit(input::key::INV_CYCLE_L))
				ACTOR(players[activePlayer])->DecrEquipSlot();
			if (input::GetHit(input::key::INV_CYCLE_R))
				ACTOR(players[activePlayer])->IncrEquipSlot();
			if (input::GetHit(input::key::ACTIVATE)) // Pick up items
				if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->Type() == Entity::eITEM)
						ACTOR(players[activePlayer])->PickUpItem(viewtarget[activePlayer]);
			if (input::GetHit(input::key::DROP_HELD))
				ACTOR(players[activePlayer])->DropItem(ACTOR(players[activePlayer])->inv_active_slot);
		}
		else
		{
			if (input::GetHit(input::key::C_INV_CYCLE_L))
				ACTOR(players[activePlayer])->DecrEquipSlot();
			if (input::GetHit(input::key::C_INV_CYCLE_R))
				ACTOR(players[activePlayer])->IncrEquipSlot();
			if (input::GetHit(input::key::C_ACTIVATE)) // Pick up items
				if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->Type() == Entity::eITEM)
						ACTOR(players[activePlayer])->PickUpItem(viewtarget[activePlayer]);
			if (input::GetHit(input::key::C_DROP_HELD))
				ACTOR(players[activePlayer])->DropItem(ACTOR(players[activePlayer])->inv_active_slot);
		}
	}

	void DrawGUI()
	{
		// Set GL properties for transparant rendering
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Player 1, X Start
		int p1_x_start = -(int)cfg::iWinX / 4;
		int p1_y_start = -(int)cfg::iWinY / 2;

		//draw ui
		graphics::shader_gui.Use();

		// croshair
		graphics::DrawGUITexture(&res::GetT(res::t_gui_crosshair), &graphics::shader_gui, 0, 0, 32, 32);
		// inventory
		CHARA(activePlayer)->inventory.Draw(ACTOR(players[activePlayer])->inv_active_slot);
		// hp
		graphics::DrawGUITexture(&res::GetT(res::t_gui_bar_red), &graphics::shader_gui, p1_x_start + 32, p1_y_start + 8, (int)(index::GetHP(index::players[activePlayer]) * 64.f), 16);
		// enemy hp
		if (viewtarget[activePlayer] != ID_NULL && viewtarget[activePlayer] != index::players[activePlayer]) // If not null or player
		{
			if (viewtarget[activePlayer] != viewtarget_last_tick[activePlayer]) // if target has changed
			{
				if (ENTITY(viewtarget[activePlayer])->Type() == Entity::eITEM)
				{
					text_temp.ReGen((char*)archive::items[ITEM(viewtarget[activePlayer])->itemid]->name, -310, -110, -200);
					guibox.ReGen(-310, -310 + text_temp.sizex, -200 - text_temp.sizey, -200, 4, 10);
				}
			}
			if (ENTITY(viewtarget[activePlayer])->Type() == Entity::eCHARA)
				graphics::DrawGUITexture(&res::GetT(res::t_gui_bar_yellow), &graphics::shader_gui, p1_x_start + 32, p1_y_start + 24, (int)(index::GetHP(viewtarget[activePlayer]) * 64.f), 16);
			else if (ENTITY(viewtarget[activePlayer])->Type() == Entity::eITEM)
			{
				guibox.Draw(&graphics::shader_gui, &res::GetT(res::t_gui_box));
				text_temp.Draw(&graphics::shader_gui, &res::GetT(res::t_gui_font));
			}
		}
	}

	void DrawPostDraw()
	{
		text_version.Draw(&graphics::shader_gui, &res::GetT(res::t_gui_font));
		char buffer[16];
		int i = snprintf(buffer, 16, "%f", 1.f / Time::deltaTick);
		text_fps.ReGen(buffer, cfg::iWinX * -0.25f, 512, cfg::iWinY * 0.5f - 16.f);
		text_fps.Draw(&graphics::shader_gui, &res::GetT(res::t_gui_font));
	}

	void SetInput(btID index, m::Vector2 input, btf32 rot_x, btf32 rot_y, bool atk, bool atk_hit, bool atk2, bool run, bool aim, bool ACTION_A, bool ACTION_B, bool ACTION_C)
	{
		CHARA(index)->input = input;
		CHARA(index)->viewYaw.Rotate(rot_x);
		CHARA(index)->viewPitch.RotateClamped(rot_y, -80.f, 70.f);
		CHARA(index)->inputBV.setto(Actor::IN_USE, atk);
		CHARA(index)->inputBV.setto(Actor::IN_USE_HIT, atk_hit);
		CHARA(index)->inputBV.setto(Actor::IN_USE_ALT, atk2);
		CHARA(index)->inputBV.setto(Actor::IN_RUN, run);
		CHARA(index)->inputBV.setto(Actor::IN_AIM, aim);
		CHARA(index)->inputBV.setto(Actor::IN_ACTN_A, ACTION_A);
		CHARA(index)->inputBV.setto(Actor::IN_ACTN_B, ACTION_B);
		CHARA(index)->inputBV.setto(Actor::IN_ACTN_C, ACTION_C);
	}

	void AddEntityCell(btui32 x, btui32 y, btID e)
	{
		cells[x][y].ents.add(e);
	}

	void RemoveEntityCell(btui32 x, btui32 y, btID e)
	{
		cells[x][y].ents.remove(e);
	}

	void SpawnProjectile(fac::faction faction, m::Vector2 pos, btf32 height, float yaw, float pitch, float spread)
	{
		yaw += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
		pitch += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

		btID id = block_proj.add();
		proj[id].t.position = pos;
		//proj[id].t.height = 0.9f + height;
		proj[id].t.height = 1.3f + height;

		proj[id].t.height_velocity = -sin(pitch);
		proj[id].t.velocity = m::AngToVec2(yaw) * cos(pitch); // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj[id].ttd = Time::time + 2.0;

		proj[id].faction = faction;

		proj[id].distance_travelled = 0.f;
		proj[id].smokePointIndex = 0ui8;

		for (int i = 0; i < PROJ_TRAIL_NUM; i++) // Set all smoke points to start pos (temporary, maybe)
			graphics::MatrixTransform(proj[id].smokePoints[i], m::Vector3(pos.x, pos.y, height));
	}

	void DestroyProjectile(btID id)
	{
		block_proj.remove(id);
	}

	btID SpawnItem(btID itemid, m::Vector2 pos, btf32 dir)
	{
		btID id = block_entity.add();

		ENTITY(id) = new EItem();
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::none;
		ENTITY(id)->properties.set(Entity::ePREFAB_ITEM);
		ENTITY(id)->state.properties.set(ActiveState::eALIVE);
		ITEM(id)->itemid = itemid;

		return id;
	}

	btID SpawnEntity(prefab::prefabtype type, m::Vector2 pos, float dir)
	{
		btID id = block_entity.add();
		PrefabEntity[type](id, pos, dir);
		return id;
	}

	void DestroyEntity(btID id)
	{
		RemoveAllReferences(id);
		delete ENTITY(id);
		block_entity.remove(id);
		std::cout << "Destroyed entity " << id << std::endl;
	}

	void ActorCastProj(btID i)
	{
		SpawnProjectile(ENTITY(i)->faction, ENTITY(i)->t.position + (m::AngToVec2(ENTITY(i)->yaw.Rad()) * 0.55f), ENTITY(i)->t.height, ACTOR(i)->viewYaw.Rad(), ACTOR(i)->viewPitch.Rad(), 1.f);
	}

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
				if (proj[index].t.position.x <= 1.f || proj[index].t.position.x >= WORLD_SIZE - 1 ||
					proj[index].t.position.y <= 1.f || proj[index].t.position.y >= WORLD_SIZE - 1)
					DestroyProjectile(index);
				// If it's time to die, or collided
				if (Time::time > proj[index].ttd || ProjectileDoesIntersectEnv(index))
					DestroyProjectile(index);
				else // Otherwise
				{
					proj[index].t.height_velocity -= 0.001f;
					proj[index].t.position += proj[index].t.velocity * dt * 24.f; // Use speed variable
					proj[index].t.height += proj[index].t.height_velocity * dt * 24.f;
					//proj[index].t.position += proj[index].t.velocity * dt * 2.f; // Use speed variable
					//proj[index].t.height += proj[index].t.height_velocity * dt * 2.f;
					proj[index].distance_travelled += m::Length(m::Vector3(proj[index].t.velocity.x * dt * 24.f, proj[index].t.velocity.y * dt * 24.f, proj[index].t.height_velocity * dt * 24.f));
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
				glm::vec3 pos(proj[index].t.position.x, proj[index].t.height, proj[index].t.position.y);
				glm::vec3 dir(proj[index].t.velocity.x, proj[index].t.height_velocity, proj[index].t.velocity.y);

				//m::Vector3 velocity3d = m::Normalize(m::Vector3(proj[index].t.velocity.x, proj[index].t.height_velocity, proj[index].t.velocity.y));

				graphics::Matrix4x4 model; // Create identity matrix
				/* // All-in-one transform/pitch/yaw
				graphics::MatrixTransform(model, pos,
					m::Vec2ToAng(m::Normalize(proj[index].t.velocity)), //yaw
					m::Vec2ToAng(m::Normalize(m::Vector2(-proj[index].t.height_velocity, m::Length(proj[index].t.velocity))))); // pitch
				//*/
				// All-in-one transform
				graphics::MatrixTransform(model, pos, m::Normalize(m::Vector3(proj[index].t.velocity.x, proj[index].t.height_velocity, proj[index].t.velocity.y)), m::Vector3(0, 1, 0));

				// Draw projectile mesh
				DrawMesh(ID_NULL, res::GetM(res::m_proj), res::GetT(res::t_proj), graphics::shader_solid, model);
				// Iterate smokepoint transformation (distance_travelled is way off real scale for some reason)
				/*
				if (proj[index].distance_travelled > 16.f)
				{
					proj[index].distance_travelled -= 16.f;
					++proj[index].smokePointIndex;
					if (proj[index].smokePointIndex >= PROJ_TRAIL_NUM)
						proj[index].smokePointIndex = 0ui8;
					proj[index].smokePoints[proj[index].smokePointIndex] = model; // Set this matrix
					proj[index].smokePointTime[proj[index].smokePointIndex] = 0.f; // Set this smokepoint time
				}
				// Draw smoke trail
				for (int i = 0; i < PROJ_TRAIL_NUM; i++) // For all smoke matrices
				{
					proj[index].smokePointTime[i] += 0.001f;
					//proj[index].smokePointTime[i] = m::Lerp(proj[index].smokePointTime[i], 1.f, 0.01f);
					DrawBlendMesh(ID_NULL, res::mb_smoke_trail_segment, proj[index].smokePointTime[i], res::t_smoke_trail, graphics::shader_blend, proj[index].smokePoints[i]);
					//DrawMesh(ID_NULL, res::mb_smoke_trail_segment, res::t_smoke_trail, graphics::shader_solid, proj[index].smokePoints[i]);
				}
				*/
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
				if (Time::time > proj[index].ttd || ProjectileDoesIntersectEnv(index)) // If it's time to die
				{
					DestroyProjectile(index);
					btui16 x = (btui16)roundf(proj[index].t.position.x);
					btui16 y = (btui16)roundf(proj[index].t.position.y);
					m::Vector2 vec = m::Normalize(proj[index].t.velocity);
					btui16 x2 = (btui16)((bti16)x + (bti16)roundf(vec.x));
					btui16 y2 = (btui16)((bti16)y + (bti16)roundf(vec.y));
					//t_EnvHeightmap.SetPixelChannelG((btui16)roundf(proj[index].t.position.x), (btui16)roundf(proj[index].t.position.y), 255ui8);
					
					if (!env::Get(x, y, env::eflag::eIMPASSABLE) && !env::Get(x2, y2, env::eflag::eIMPASSABLE))
					{
						if (env::eCells[x2][y2].height < env::eCells[x][y].height + 3ui8) // If the height difference is low
						{
							--env::eCells[x][y].height;
							t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);
							++env::eCells[x2][y2].height;
							t_EnvHeightmap.SetPixelChannelR(x2, y2, env::eCells[x2][y2].height);
						}
					}

					t_EnvHeightmap.SetPixelChannelG(x, y, 255ui8);
					t_EnvHeightmap.SetPixelChannelG(x + 1, y, 255ui8);
					t_EnvHeightmap.SetPixelChannelG(x - 1, y, 255ui8);
					t_EnvHeightmap.SetPixelChannelG(x, y + 1, 255ui8);
					t_EnvHeightmap.SetPixelChannelG(x, y - 1, 255ui8);
					t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
				}
				else // Otherwise
				{
					// LOOP THROUGH ALL ENTITIES METHOD, SHIT BUT ONLY WAY THAT WORKS ATM
					///*
					for (int i = 0; i <= block_entity.index_end; i++)
					{
						if (block_entity.used[i] && ENTITY(i)->properties.get(Entity::eCOLLIDE_PRJ))
						{
							if (fac::GetAllegiance(ENTITY(i)->faction, proj[index].faction) != fac::allied)
							{
								//check height difference
								if (proj[index].t.height > ENTITY(i)->t.height && proj[index].t.height < ENTITY(i)->t.height + ENTITY(i)->height)
								{
									//get difference between positions
									m::Vector2 vec = proj[index].t.position - ENTITY(i)->t.position;
									//get distance
									float dist = m::Length(vec);
									if (dist < 0.5f)
									{
										// kill
										ENTITY(i)->state.hp -= 0.5f;
										if (ENTITY(i)->state.hp <= 0.f)
										{
											ENTITY(i)->state.properties.unset(ActiveState::eALIVE);
											ENTITY(i)->state.hp = 0.f;
										}
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
						for (int e = 0; e < IDBUF_SIZE; e++)
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

	bool ProjectileDoesIntersectEnv(btID index)
	{
		int x = (int)roundf(proj[index].t.position.x);
		int y = (int)roundf(proj[index].t.position.y);
		if (env::Get(x, y, env::eflag::eIMPASSABLE)) // if hit an impassable tile
			return true;

		CellSpace csi;
		GetCellSpaceInfo(proj[index].t.position, csi);
		btf32 height;
		env::GetHeight(height, csi);
		if (proj[index].t.height < height) // if below the ground surface
			return true;

		return false;
	}
}