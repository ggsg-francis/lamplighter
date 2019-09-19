#include "index.h"
#include "index_util.h"
#include "index_fn.cpp"
#include "weather.h"

unsigned int active_player_view = 0u;

namespace index
{
	btID viewtarget = ID_NULL;
	btID viewtarget_last_tick = ID_NULL;

	#ifdef DEF_EDITOR
	btui32 GetCellX()
	{
		return ENTITY(0)->t.cellx;
	}
	btui32 GetCellY()
	{
		return ENTITY(0)->t.celly;
	}
	#endif // DEF_EDITOR

	btf32 GetHP(btID id)
	{
		return ENTITY(id)->state.hp;
	}

	void SetViewTargetID(btID id)
	{
		viewtarget_last_tick = viewtarget;
		viewtarget = id;
		//std::cout << "looking at id " << viewtarget << std::endl;
	}
	btID GetViewTargetID()
	{
		return viewtarget;
	}

	void* GetEntity(btID id)
	{
		return _entities[id];
	}

	void SetViewFocus(btID index)
	{
		active_player_view = index;
		viewpos = ENTITY(active_player_view)->t.position * -1.f;

		#define h 1.6f + m::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.aniTime / resAniStep.time)
		graphics::SetMatView(&CHARA(active_player_view)->t_head);
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
		t_EnvLightmap.SetPixelChannelG(x, y, col.g);
		//if (!env::Get(x, y, env::eflag::eIMPASSABLE) && t_EnvLightmap.GetPixel(x, y).g < col.g)
		//{
		//	t_EnvLightmap.SetPixelChannel(x, y, 1ui8, col.g);
		//	if (col.g > 31ui8)
		//	{
		//		col.g -= 32ui8;
		//		flood_fill_temp(x + 1, y, col);
		//		flood_fill_temp(x - 1, y, col);
		//		flood_fill_temp(x, y + 1, col);
		//		flood_fill_temp(x, y - 1, col);
		//	}
		//}
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
		glBindTexture(GL_TEXTURE_2D, res::GetBTT(res::t_sky).glID); // Bind the texture
		glActiveTexture(GL_TEXTURE4); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "ttsm"), 4);
		glBindTexture(GL_TEXTURE_2D, t_envShadowMap.glID); // Bind the texture

		// terrain textures
		glActiveTexture(GL_TEXTURE6); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "tt1"), 6);
		glBindTexture(GL_TEXTURE_2D, res::GetBTT(res::t_terrain_scorch).glID); // Bind the texture

		graphics::shader_solid.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetBTT(res::t_sky).glID); // Bind the texture
		glActiveTexture(GL_TEXTURE4); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "ttsm"), 4);
		glBindTexture(GL_TEXTURE_2D, t_envShadowMap.glID); // Bind the texture

		graphics::shader_blend.Use();
		glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_blend.ID, "tlm"), 1);
		glBindTexture(GL_TEXTURE_2D, t_EnvLightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_blend.ID, "thm"), 2);
		glBindTexture(GL_TEXTURE_2D, t_EnvHeightmap.glID); // Bind the texture
		glActiveTexture(GL_TEXTURE3); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_blend.ID, "ts"), 3);
		glBindTexture(GL_TEXTURE_2D, res::GetBTT(res::t_sky).glID); // Bind the texture
		glActiveTexture(GL_TEXTURE4); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(graphics::shader_blend.ID, "ttsm"), 4);
		glBindTexture(GL_TEXTURE_2D, t_envShadowMap.glID); // Bind the texture
	}
	void InitShadowMap()
	{
		// Generate shadowmap v2
		int shadow_map_div = 8;

		t_envShadowMap.Init(WORLD_SIZE / shadow_map_div, WORLD_SIZE / shadow_map_div, graphics::colour(0ui8, 255ui8, 255ui8, 255ui8));
	
		for (btui16 x = 0; x < WORLD_SIZE / shadow_map_div; x++) // Create smaller texture
			for (btui16 y = 0; y < WORLD_SIZE / shadow_map_div; y++)
			{
				// Method A: Average
				///*
				float average = 0.f;
				for (int x2 = 0; x2 < shadow_map_div; x2++) // Add every pixel in this block
					for (int y2 = 0; y2 < shadow_map_div; y2++)
						average += (float)env::eCells[x * shadow_map_div + x2][y * shadow_map_div + y2].height;
				average /= (shadow_map_div * shadow_map_div); // Divide back to normalized
				btui8 averageRnd = (btui8)ceilf(average);
				t_envShadowMap.SetPixelChannelB(x, y, averageRnd);
				//*/

				// Method B: Max
				/*
				btui8 max = 0ui8;
				for (int x2 = 0; x2 < shadow_map_div; x2++) // Add every pixel in this block
				for (int y2 = 0; y2 < shadow_map_div; y2++)
				if (env::eCells[x * shadow_map_div + x2][y * shadow_map_div + y2].height > max) // If higher than already measured
				max = env::eCells[x * shadow_map_div + x2][y * shadow_map_div + y2].height; // Then add
				if (max > 0ui8)
				t_envShadowMap.SetPixelChannelB(x, y, max);
				//*/
			}
	}
	btui16 shadow_map_x;
	void UpdateShadowMapPartial(btf32 time2)
	{
		// Generate shadowmap v2
		int shadow_map_div = 8;

		// iterators
		btui16 y = 0u;

		// Reset heights if new texture
		if (shadow_map_x == 0u)
			for (btui16 x = 0u; x < WORLD_SIZE / shadow_map_div; x++)
				for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
				{
					t_envShadowMap.SetPixelChannelR(x, y, t_envShadowMap.GetPixel(x, y).b);
					t_envShadowMap.SetPixelChannelG(x, y, 255ui8); // Reset bounce map
				}

		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 8.f) / 8.f) + 180.f));

		float rise_mult = fabsf(sunrot.y) / fabsf(sunrot.x);

		// Update texture with shadow crests
		for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
		{
			// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
			int x2 = shadow_map_x;
			int run = 0;
			btui8 origin_height = t_envShadowMap.GetPixel(shadow_map_x, y).r;
			btui8 current_height = origin_height;
			while (true)
			{
				++x2; ++run;
				if (x2 >= WORLD_SIZE / shadow_map_div)
					break;
				else
				{
					//current_height = origin_height - ((x2 - x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION); // get rise from distance * aspect
					//current_height = (btui8)floorf(origin_height - ((x2 - x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION)); // get rise from distance * aspect
					current_height = (btui8)ceilf(origin_height - ((x2 - shadow_map_x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION)); // get rise from distance * aspect
					if (t_envShadowMap.GetPixel(x2, y).r < current_height && current_height > 0ui8)
					{
						t_envShadowMap.SetPixelChannelR(x2, y, current_height);
						t_envShadowMap.SetPixelChannelG(x2, y, 0ui8);
					}
					else break;
				}
			}
		}

		// if last line, lower other pixels & bind texture
		if (shadow_map_x == (WORLD_SIZE / shadow_map_div) - 1u)
		{
			for (btui16 x = 0u; x < WORLD_SIZE / shadow_map_div; x++) // Lower all non-crest shadow heights
				for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
					if (t_envShadowMap.GetPixel(x, y).g > 0ui8) // if not part of crest
						if (t_envShadowMap.GetPixel(x, y).r > 3ui8) // If height above zero
							t_envShadowMap.SetPixelChannelR(x, y, t_envShadowMap.GetPixel(x, y).r - 4ui8); // lower
						else t_envShadowMap.SetPixelChannelR(x, y, 0ui8); // set to zero

			t_envShadowMap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
		}

		++shadow_map_x;
		if (shadow_map_x == t_envShadowMap.width)
			shadow_map_x = 0u;
	}
	void UpdateShadowMap(btf32 time2)
	{
		// Generate shadowmap v2
		int shadow_map_div = 8;

		// iterators
		btui16 x = 0u, y = 0u;

		// Reset heights
		for (x = 0u; x < WORLD_SIZE / shadow_map_div; x++)
			for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
			{
				t_envShadowMap.SetPixelChannelR(x, y, t_envShadowMap.GetPixel(x, y).b);
				t_envShadowMap.SetPixelChannelG(x, y, 255ui8); // Reset bounce map
			}

		// try to add shadow crest generation
		// http://www.andrewwillmott.com/tech-notes#TOC-Height-Map-Shadows
		// https://github.com/andrewwillmott/height-map-shadow
		// new idea: 4 channels, 2 for dawn, 2 for dusk, can interpolate towards zero for midday, towards 1 for night

		//btf32 time2 = 0.28f;
		//btf32 time2 = 0.35f;
		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 8.f) / 8.f) + 180.f));

		//float rise_mult = 1.f;
		//float rise_mult = fabsf(sunrot.x) / fabsf(sunrot.y);
		float rise_mult = fabsf(sunrot.y) / fabsf(sunrot.x);

		for (x = 0u; x < WORLD_SIZE / shadow_map_div; x++) // Update texture with shadow crests
			for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
			{
				// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
				int x2 = x;
				int run = 0;
				btui8 origin_height = t_envShadowMap.GetPixel(x, y).r;
				btui8 current_height = origin_height;
				while (true)
				{
					++x2; ++run;
					if (x2 >= WORLD_SIZE / shadow_map_div)
						break;
					else
					{
						//current_height = origin_height - ((x2 - x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION); // get rise from distance * aspect
						//current_height = (btui8)floorf(origin_height - ((x2 - x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION)); // get rise from distance * aspect
						current_height = (btui8)ceilf(origin_height - ((x2 - x) * rise_mult * shadow_map_div * TERRAIN_HEIGHT_DIVISION)); // get rise from distance * aspect
						if (t_envShadowMap.GetPixel(x2, y).r < current_height && current_height > 0ui8)
						{
							t_envShadowMap.SetPixelChannelR(x2, y, current_height);
							t_envShadowMap.SetPixelChannelG(x2, y, 0ui8);
						}
						else break;
					}
				}
			}

		for (x = 0u; x < WORLD_SIZE / shadow_map_div; x++) // Lower all non-crest shadow heights
			for (y = 0u; y < WORLD_SIZE / shadow_map_div; y++)
				if (t_envShadowMap.GetPixel(x, y).g > 0ui8) // if not part of crest
					if (t_envShadowMap.GetPixel(x, y).r > 1ui8) // If height above zero
						t_envShadowMap.SetPixelChannelR(x, y, t_envShadowMap.GetPixel(x, y).r - 2ui8); // lower by 1
					else t_envShadowMap.SetPixelChannelR(x, y, 0ui8); // set to zero

					t_envShadowMap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
	}

	void Init()
	{
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

		// Lightmap

		t_EnvLightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(255ui8, 0ui8, 0ui8, 255ui8));

		flood_fill_temp(5, 3, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));
		flood_fill_temp(16, 16, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));

		t_EnvLightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		// Heightmap

		t_EnvHeightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(0ui8, 0ui8, 0ui8, 0ui8));

		for (btui16 x = 0; x < WORLD_SIZE; x++)
			for (btui16 y = 0; y < WORLD_SIZE; y++)
				t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);
		
		t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		InitShadowMap();
		//UpdateShadowMap(0.28f);

		// Spawnz

		//players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(10.f, 10.f), 0.f);
		players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
		players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(1020.f, 1020.f), 0.f);

		SpawnItem(0ui16, m::Vector2(1025.1f, 1022.6f), 15.f);
		SpawnItem(0ui16, m::Vector2(1025.5f, 1024.0f), 120.f);
		SpawnItem(1ui16, m::Vector2(1025.5f, 1023.3f), 15.f);
		SpawnItem(2ui16, m::Vector2(1025.8f, 1024.f), 15.f);

		#ifndef DEF_EDITOR
		//SpawnEntity(prefab::prefab_npc, m::Vector2(7.f, 6.f), 0.f);
		//SpawnEntity(prefab::prefab_zombie, m::Vector2(9.f, 4.f), 0.f);
		//SpawnEntity(prefab::prefab_zombie, m::Vector2(9.f, 8.f), 0.f);
		//SpawnEntity(prefab::prefab_ai_player, m::Vector2(9.f, 10.f), 0.f);

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

		#endif

		graphics::SetMatProj(); // does not need to be continually called
		UpdateGlobalShaderParams();
	}
	//destroy all data
	void End()
	{
		for (int i = 0; i < BUF_SIZE; i++)
		{
			if (block_entity.used[i])
				DestroyEntity(i);
		}
	}

	void Tick(btf32 dt)
	{
		t_moon.Rotate(-0.01f * dt, m::Vector3(1.f, 0.f, 0.f));

		//******************************** ITERATE THROUGH ENTITIES

		#ifndef DEF_EDITOR

		EntCheckDeath(0, block_entity.index_end); // Check hp of every entity

		#endif

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
			if (block_entity.used[i])
				tick[ENTITY(i)->Type()](dt, i);

		ProjectileTick(dt);

		//temporary destroy dead entities
		///*
		for (int i = 0; i <= block_entity.index_end; i++)
			if (block_entity.used[i])
				if (ACTOR(i)->state.hp == 0.f && ACTOR(i)->aiControlled)
					//temp
					index::DestroyEntity(i);
		//*/

		#ifdef DEF_EDITOR

		if (input::Get(input::key::atk_hit))
		{
			env::GeneratePhysicsSurfaces();
			//env::GeneratePaths();
		}
		if (input::Get(input::key::activate_hit))
		{
			env::SaveBin();
		}
		if (input::Get(input::key::action_a_hit))
		{
			if (env::Get(index::GetCellX(), index::GetCellY(), env::eflag::eIMPASSABLE))
			{
				env::UnSet(index::GetCellX(), index::GetCellY(), env::eflag::eIMPASSABLE);
			}
			else
			{
				env::Set(index::GetCellX(), index::GetCellY(), env::eflag::eIMPASSABLE);
			}
		}
		if (input::Get(input::key::action_b_hit))
		{
			++env::eCells[index::GetCellX()][index::GetCellY()].height;
			t_EnvHeightmap.SetPixel(GetCellX(), GetCellY(), graphics::colour(env::eCells[index::GetCellX()][index::GetCellY()].height, 0ui8, 0ui8, 0ui8));
			t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
		}
		if (input::Get(input::key::action_c_hit))
		{
			--env::eCells[index::GetCellX()][index::GetCellY()].height;
			t_EnvHeightmap.SetPixel(GetCellX(), GetCellY(), graphics::colour(env::eCells[index::GetCellX()][index::GetCellY()].height, 0ui8, 0ui8, 0ui8));
			t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
		}

		#endif // DEF_EDITOR
	}

	btui32 drawticker_temp = 0;

	void Draw(bool oob)
	{
		//btf32 time2 = time * 0.0001f + 0.3f; // decent timescale for now
		//btf32 time2 = time * 0.0015f + 0.7f;
		btf32 time2 = time * 0.0002f + 0.26f;

		//btf32 sunang = atan(0.5f / 1.f);

		/*
		drawticker_temp++;
		if (drawticker_temp == 16u)
		{
			drawticker_temp = 0u;
			//UpdateShadowMap(time2);
			UpdateShadowMapPartial(time2);
		}*/
		UpdateShadowMapPartial(time2);


		//btf32 time2 = 0.28f;
		//btf32 time2 = 0.35f;
		//btf32 time2 = glm::degrees(sunang) / 360.f + 0.5f;
		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 8.f) / 8.f) + 180.f));

		graphics::Matrix4x4 matrix; // Matrix used for rendering env. props (so far...)

		if (oob)
		{
			//******************************** UPDATE SHADERS

			graphics::shader_terrain.Use();
			graphics::shader_terrain.setMat4("lightProj", shadowmat_temp);
			graphics::shader_terrain.SetFloat("ft", (float)time2);
			graphics::shader_terrain.setVec3("pcam", glm::vec3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, -ENTITY(active_player_view)->t.position.y));
			graphics::shader_terrain.setVec3("vsun", glm::vec3(sunrot.x, sunrot.y, 0.f));
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_terrain.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_solid.Use();
			graphics::shader_solid.setMat4("lightProj", shadowmat_temp);
			graphics::shader_solid.SetFloat("ft", (float)time2);
			graphics::shader_solid.setVec3("pcam", glm::vec3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, -ENTITY(active_player_view)->t.position.y));
			graphics::shader_solid.setVec3("vsun", glm::vec3(sunrot.x, sunrot.y, 0.f));
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_solid.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			graphics::shader_blend.Use();
			graphics::shader_blend.setMat4("lightProj", shadowmat_temp);
			graphics::shader_blend.SetFloat("ft", (float)time2);
			graphics::shader_blend.setVec3("pcam", glm::vec3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, -ENTITY(active_player_view)->t.position.y));
			graphics::shader_blend.setVec3("vsun", glm::vec3(sunrot.x, sunrot.y, 0.f));
			glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(graphics::shader_blend.ID, "tshadow"), 5);
			glBindTexture(GL_TEXTURE_2D, shadowtex); // Bind the texture

			/*
			graphics::shader_scroll.Use();
			graphics::shader_scroll.setVec3("csun", *(glm::vec3*)weather::SunColour());
			graphics::shader_scroll.setVec3("camb", *(glm::vec3*)weather::AmbientColour());
			graphics::shader_scroll.setVec3("cfog", *(glm::vec3*)weather::FogColour());
			graphics::shader_scroll.SetFloat("ffog", *(float*)weather::FogDensity());
			graphics::shader_scroll.SetFloat("ft", (float)time);
			graphics::shader_scroll.setVec3("pcam", glm::vec3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, -ENTITY(active_player_view)->t.position.y));
			*/

			graphics::shader_sky.Use();
			graphics::shader_sky.SetFloat("ft", (float)time2);

			//******************************** DRAW SKY

			graphics::SetFrontFace();
			graphics::SetMatProj();


			t_moon.SetPosition(m::Vector3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, ENTITY(active_player_view)->t.position.y));


			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// Setup sky matrix
			graphics::MatrixTransform(matrix, m::Vector3(ENTITY(active_player_view)->t.position.x, ENTITY(active_player_view)->t.height + 1.f, ENTITY(active_player_view)->t.position.y));
			// Draw stars
			glDisable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			DrawMesh(ID_NULL, res::m_skystars, res::t_sky, graphics::shader_sky, matrix);
			// Draw Moon
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			graphics::shader_solid.Use();
			graphics::shader_solid.SetBool("lit", false);
			DrawMeshAtTransform(ID_NULL, res::m_skymoon, res::t_skymoon, graphics::shader_solid, t_moon);
			graphics::shader_solid.Use();
			graphics::shader_solid.SetBool("lit", true);
			// Draw Sky
			glBlendFunc(GL_ONE, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			DrawMesh(ID_NULL, res::m_skydome, res::t_sky, graphics::shader_sky, matrix);
			// Reset
			glBlendFunc(GL_ONE, GL_ZERO);
			glDisable(GL_BLEND);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//******************************** DRAW OOB TERRAIN

			

			

			//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			graphics::SetMatProj(128.f); // Set projection matrix for long-distance rendering
			graphics::MatrixTransform(matrix, m::Vector3(0.f, 0.f, 0.f));
			DrawMesh(ID_NULL, res::m_terrain_oob, res::t_terrain_sanddirt, graphics::shader_terrain, matrix);
			graphics::SetMatProj(); // Reset projection matrix

			//******************************** DRAW ENTITIES

			glClear(GL_DEPTH_BUFFER_BIT); // Clear for foreground rendering

			// ...
		}
		else
		{
			// Set light matrices
			graphics::SetMatProjLight(); graphics::SetMatViewLight(
				ENTITY(active_player_view)->t.position.x,
				ENTITY(active_player_view)->t.height,
				-ENTITY(active_player_view)->t.position.y,
				-sunrot.x, -sunrot.y, 0.f);
			shadowmat_temp = graphics::GetMatProj() * graphics::GetMatView();
		}

		#ifndef DEF_EDITOR
		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
			if (block_entity.used[i]) draw[ENTITY(i)->Type()](i); // Draw entities
		if (oob) ProjectileDraw(); // Draw projectiles
		#endif

		//******************************** DRAW ENVIRONMENT

		if (oob)
		{
			graphics::MatrixTransform(matrix, m::Vector3(roundf(ENTITY(active_player_view)->t.position.x / 8) * 8, 0.f, roundf(ENTITY(active_player_view)->t.position.y / 8) * 8));
			DrawMesh(ID_NULL, res::m_terrain_near, res::t_terrain_sanddirt, graphics::shader_terrain, matrix);
		}

		line_transform.SetScale(m::Vector3(1, 1, 1));

		#ifdef DEF_EDITOR

		btui32 drawrange = 20u; // Create min/max draw coordinates
		bti32 cxs = ENTITY(active_player_view)->t.cellx - drawrange;
		if (cxs < 0)
			cxs = 0;
		bti32 cxe = ENTITY(active_player_view)->t.cellx + drawrange;
		if (cxe > WORLD_SIZE - 1)
			cxe = WORLD_SIZE - 1;
		bti32 cys = ENTITY(active_player_view)->t.celly - drawrange;
		if (cys < 0)
			cys = 0;
		bti32 cye = ENTITY(active_player_view)->t.celly + drawrange;
		if (cye > WORLD_SIZE - 1)
			cye = WORLD_SIZE - 1;

		for (int x = cxs; x <= cxe; x++)
		{
			for (int y = cys; y < cye; y++)
			{
				if (!env::Get(x, y, env::eflag::eIMPASSABLE))
				{
					line_transform.SetPosition(m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
					env::nbit::node_bit bit = env::get_node_bit(GetCellX(), GetCellY(), x, y);
					switch (bit)
					{
					case env::nbit::N:
						line_transform.SetRotation(glm::radians(0.f));
						break;
					case env::nbit::S:
						line_transform.SetRotation(glm::radians(180.f));
						break;
					case env::nbit::E:
						line_transform.SetRotation(glm::radians(90.f));
						break;
					case env::nbit::W:
						line_transform.SetRotation(glm::radians(270.f));
						break;
					default:
						break;
					}
					DrawMeshAtTransform(ID_NULL, res::m_debugcell, res::t_gui_bar_red, graphics::shader_solid, line_transform);
				}
			}
		}

		#endif // DEF_EDITOR

		//for (int x = cxs; x <= cxe; x++)
		//	for (int y = cys; y < cye; y++)
		//		if (!env::Get(x, y, env::eflag::eIMPASSABLE))
		//		{
		//			line_transform.SetPosition(m::Vector3((btf32)x, 0.f, (btf32)y));
		//			DrawMeshAtTransform(ID_NULL, res::m_kitfloor, res::t_marshmud, graphics::shader_terrain, line_transform);
		//		}

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
		#ifndef DEF_EDITOR
		if (input::Get(input::key::action_b_hit))
		{
			if (inv_active_slot > 0u)
				--inv_active_slot;
		}
		if (input::Get(input::key::action_c_hit))
		{
			if (inv_active_slot < 10u)
			++inv_active_slot;
		}
		if (input::Get(input::key::activate_hit)) // Pick up items
			if (viewtarget != ID_NULL)
				if (ENTITY(viewtarget)->Type() == etype::eitem)
					ACTOR(players[0])->PickUpItem(viewtarget);
		if (input::Get(input::key::action_a_hit))
		{
			ACTOR(players[0])->DropItem(inv_active_slot);
		}
		#endif
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
		graphics::DrawGUITexture(&res::GetBTT(res::t_gui_crosshair), &graphics::shader_gui, 0, 0, 32, 32);
		// hp
		graphics::DrawGUITexture(&res::GetBTT(res::t_gui_bar_red), &graphics::shader_gui, p1_x_start + 32, p1_y_start + 8, (int)(index::GetHP(index::players[0ui16]) * 64.f), 16);
		// enemy hp
		if (viewtarget != ID_NULL && viewtarget != index::players[0ui16]) // If not null or player
		{
			if (viewtarget != viewtarget_last_tick) // if target has changed
			{
				if (ENTITY(viewtarget)->Type() == etype::eitem)
				{
					/*text.ReGen((char*)archive::items[ITEM(viewtarget)->itemid]->name, strlen((char*)archive::items[ITEM(viewtarget)->itemid]->name), 20, 160, -15);
					guibox.ReGen(20, 20 + text.sizex, -15 - text.sizey, -15, 4, 8);*/
					text.ReGen((char*)archive::items[ITEM(viewtarget)->itemid]->name, strlen((char*)archive::items[ITEM(viewtarget)->itemid]->name), -310, -110, -200);
					guibox.ReGen(-310, -310 + text.sizex, -200 - text.sizey, -200, 4, 10);
				}
			}
			if (ENTITY(viewtarget)->Type() == etype::chara)
				graphics::DrawGUITexture(&res::GetBTT(res::t_gui_bar_yellow), &graphics::shader_gui, p1_x_start + 32, p1_y_start + 24, (int)(index::GetHP(viewtarget) * 64.f), 16);
			else if (ENTITY(viewtarget)->Type() == etype::eitem)
			{
				guibox.Draw(&graphics::shader_gui, &res::GetBTT(res::t_gui_box));
				text.Draw(&graphics::shader_gui, &res::GetBTT(res::t_gui_font));
			}
		}

		DrawInv();
	}

	void SetInput(btID index, m::Vector2 input, btf32 rot_x, btf32 rot_y, bool atk, bool run, bool aim)
	{
		CHARA(index)->input = input;
		CHARA(index)->viewYaw.Rotate(rot_x);
		CHARA(index)->viewPitch.RotateClamped(rot_y, -90.f, 90.f);
		CHARA(index)->inputbv.setto(Actor::in_atk, atk);
		CHARA(index)->inputbv.setto(Actor::in_run, run);
		CHARA(index)->inputbv.setto(Actor::in_aim, aim);
	}

	void AddEntityCell(int x, int y, btID e)
	{
		cells[x][y].ents.add(e);
	}

	void RemoveEntityCell(int x, int y, btID e)
	{
		cells[x][y].ents.remove(e);
	}

	void SpawnProjectile(int type, m::Vector2 pos, btf32 height, float yaw, float pitch, float spread)
	{
		yaw += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
		pitch += glm::radians(m::Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

		btID id = block_proj.add();
		proj[id].t.position = pos;
		proj[id].t.height = 0.9f + height;

		proj[id].t.height_velocity = -sin(pitch);
		proj[id].t.velocity = m::AngToVec2(yaw) * cos(pitch); // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj[id].ttd = time + 60;

		proj[id].distance_travelled = 0.f;
		proj[id].smokePointIndex = 0ui8;

		for (int i = 0; i < PROJ_TRAIL_NUM; i++) // Set all smoke points to start pos (temporary, maybe)
			graphics::MatrixTransform(proj[id].smokePoints[i], m::Vector3(pos.x, pos.y, height));
	}

	void DestroyProjectile(btID id)
	{
		block_proj.remove(id);
	}

	#define DEF_INDEX
	#ifdef DEF_INDEX

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

	inline void ActorCastProj(btID i)
	{
		//SpawnProjectile(0, ENTITY(i)->t.position + (m::AngToVec2(ENTITY(i)->yaw.Rad()) * 0.55f), ENTITY(i)->t.height, ACTOR(i)->viewYaw.Rad(), ACTOR(i)->viewPitch.Rad(), 10.f);
		SpawnProjectile(0, ENTITY(i)->t.position + (m::AngToVec2(ENTITY(i)->yaw.Rad()) * 0.55f), ENTITY(i)->t.height, ACTOR(i)->viewYaw.Rad(), ACTOR(i)->viewPitch.Rad(), 0.5f);
	}

	inline void EntityReceiveDamage(btID i)
	{

	}

	void TickEntity(btf32 dt, btID index)
	{
		// do nothing at the moment
	}

	void TickChara(btf32 dt, btID index)
	{
		//#ifndef DEF_EDITOR

		if (ENTITY(index)->state.properties.get(ActiveState::eALIVE))
		{
			#ifdef DEF_EDITOR

			//ENTITY[index]->t.velocity = fw::Lerp(ENTITY[index]->t.velocity, fw::Rotate(f2Input, aViewYaw.Rad()) * fw::Vector2(-1.f, 1.f) * dt * fSpeed, 0.3f);
			f2Input.x = -f2Input.x;
			ENTITY(index)->t.velocity = m::Rotate(f2Input, aViewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * 5.f;

			#else

			//f2Input.x = -f2Input.x;
			//ENTITY(index)->t.velocity = m::Rotate(f2Input, aViewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt;

			#define DEF_USE_ANIM

			#ifdef DEF_USE_ANIM

			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//------------- ANIMATION SET ------------------------------------

			switch (aniLower.AdvanceAnim(dt * fSpeed)) // Advance animation, get result
			{
				//******************************** NOEVENT
			case actor::AnimPlayer::eNOEVENT:
				if (iAniID == res::a_knockback)
				{
					ENTITY(index)->t.velocity = ENTITY(index)->t.velocity * 0.8f;
					if (m::Length(ENTITY(index)->t.velocity) < 0.01f) // If not sliding anymore
						aniLower.setAnim(res::a_idle); // Reset to idle
				}
				else if (resAniStep.Flag(res::eCAN_CANCEL))
				{
					if (CHARA(index)->inputbv.get(Actor::in_atk))
						aniLower.setAnim(res::a_castmgc);
					if (aniLower.aniID == res::a_idle && m::Length(f2Input) > 0.1f) // if moving forwards and idle
					{
						bAniStepR = !bAniStepR;
						if (f2Input.y < -0.01f) // If backwards
							bAniStepR ? aniLower.setAnim(res::a_step_back_r) : aniLower.setAnim(res::a_step_back_l);
						else // Else forwards
							bvInRun ? bAniStepR ? aniLower.setAnim(res::a_run_r) : aniLower.setAnim(res::a_run_l)
							: bAniStepR ? aniLower.setAnim(res::a_step_r) : aniLower.setAnim(res::a_step_l);
					}
				}
				break;
				//******************************** ADVSTEP
			case actor::AnimPlayer::eADVSTEP:
				if (resAniStep.Flag(res::eCAST_PROJECTILE))
					ActorCastProj(index);
				break;
				//******************************** ENDOFANIM
			case actor::AnimPlayer::eENDOFANIM:
				if (resAniStep.Flag(res::eRETURN_TO_IDLE))
				{
					if (m::Length(f2Input) > 0.1f) // if wants to move, then skip idle animation
					{
						bAniStepR = !bAniStepR;
						if (f2Input.y < -0.01f) // If backwards
							bAniStepR ? aniLower.setAnim(res::a_step_back_r) : aniLower.setAnim(res::a_step_back_l);
						else // Else forwards
							bvInRun ? bAniStepR ? aniLower.setAnim(res::a_run_r) : aniLower.setAnim(res::a_run_l)
							: bAniStepR ? aniLower.setAnim(res::a_step_r) : aniLower.setAnim(res::a_step_l);
					}
					else aniLower.setAnim(res::a_idle); // Return to idle animation
				}
				else if (aniLower.aniID == res::a_knockback) // If it's the knockback animation
				{
					ENTITY(index)->t.velocity = ENTITY(index)->t.velocity * 0.8f;
					if (m::Length(ENTITY(index)->t.velocity) < 0.01f) // If not sliding anymore
						aniLower.setAnim(res::a_idle); // Reset to idle
				}
			}

			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//------------- WIP RADIOACTIVE TAR PIT !!!DO NOT EXPOSE!!! ------

			if (aniLower.aniID != res::a_knockback)
			{
				//******************************** CALCULATE DIRECTION TO WALK IN

				// super shitty
				m::Angle movang = aViewYaw.Deg(); // Start at global head rotation
				if (f2Input.y < -0.01f) // Backwards
					movang.Rotate(glm::degrees(m::Vec2ToAng(m::Normalize(f2Input * -1.f)))); // Add reverse local angle
				else if (f2Input.y > 0.01f) // Forwards
					movang.Rotate(glm::degrees(m::Vec2ToAng(m::Normalize(f2Input)))); // Add local angle
				else if (f2Input.x > 0.01f || f2Input.x < -0.01f) // Left/right
					movang.Rotate(glm::degrees(m::Vec2ToAng(m::Normalize(f2Input)))); // Add local angle

				//******************************** PIVOT

				if (resAniStep.Flag(res::eCAN_TURN) && ENTITY(index)->freeTurn) // If can turn
					eYaw2.RotateTowards(movang.Deg(), 5.f); // Rotate body towards the target direction

				//******************************** SET ENTITY VELOCITY

				ENTITY(index)->t.velocity = (m::AngToVec2(eYaw2.Rad()) * resAniStep.mov_offs * dt / resAniStep.time) * fSpeed;
			}

			#endif
			#endif // DEF_EDITOR

			//******************************** APPLY MOVEMENT

			bMoving = (m::Length(ENTITY(index)->t.velocity) > 0.016f);
			m::Vector2 oldpos = ENTITY(index)->t.position;
			ENTITY(index)->t.position += ENTITY(index)->t.velocity; // Apply velocity

			//bti32 x = (bti32)roundf(ENTITY(index)->t.position.x);
			//if (x < 0 || x >= WORLD_SIZE) x = 0;
			//bti32 y = (bti32)roundf(ENTITY(index)->t.position.y);
			//if (y < 0 || y >= WORLD_SIZE) y = 0;

			////I don't want this to be here
			//if (x != eCellX || y != eCellY)
			//{
			//	// Old way ( one reference )
			//	//index::RemoveEntityCell(cellx, celly, id);
			//	//iCellX = x; iCellY = y;
			//	//index::AddEntityCell(cellx, celly, id);

			//	// New way ( four references )
			//	eCellX = x; eCellY = y;
			//	// optimize this $hit
			//	for (int i = 0; i < 4; i++)
			//		index::RemoveEntityCell(ENTITY(index)->csi.c[i].x, ENTITY(index)->csi.c[i].y, i);
			//	GetCellSpaceInfo(ePos, ENTITY(index)->csi);
			//	for (int i = 0; i < 4; i++)
			//		index::AddEntityCell(ENTITY(index)->csi.c[i].x, ENTITY(index)->csi.c[i].y, i);
			//}

			env::GetHeight(ENTITY(index)->t.height, ENTITY(index)->csi);

			//******************************** RUN COLLISION

			#ifndef DEF_EDITOR
			EntCheckDeath(0, block_entity.index_end); // Check hp of every entity
			GetCellSpaceInfo(ePos, ENTITY(index)->csi);
			EntDeintersect(ENTITY(index), ENTITY(index)->csi, aViewYaw.Deg(), resAniStep.Flag(res::eINFLICT_KNOCKBACK));
			#endif

			//******************************** POST-PHYSICS

			#ifndef DEF_EDITOR
			if (CHARA(index)->aiControlled) ActorRunAI(index); // Run AI
			//CHARA(index)->musket.Tick(); // Tick the item we are holding
			#endif
		} // End if alive

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

		Chara* c = (Chara*)_entities[index];

		// Reset transforms
		c->t_body = Transform3D();
		c->t_head = Transform3D();

		c->t_body.SetPosition(m::Vector3(ENTITY(index)->t.position.x, ENTITY(index)->t.height + m::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.GetCurve()), ENTITY(index)->t.position.y));
		c->t_body.Rotate(eYaw2.Rad(), m::Vector3(0, 1, 0));

		if (resAniStep.Flag(res::eFLIP_LR))
		{
			c->state.properties.get(ActiveState::eALIVE) ? c->t_body.SetScale(m::Vector3(-1.f, 1.f, 1.f)) : c->t_body.SetScale(m::Vector3(-1.f, 0.1f, 1.f));
		}
		else
		{
			c->state.properties.get(ActiveState::eALIVE) ? c->t_body.SetScale(m::Vector3(1.f, 1.f, 1.f)) : c->t_body.SetScale(m::Vector3(1.f, 0.1f, 1.f));
		}

		// Set head transform
		c->t_head.SetPosition(c->t_body.GetPosition());
		if (c->state.properties.get(ActiveState::eALIVE))
		{
			c->t_head.Rotate(aViewYaw.Rad(), m::Vector3(0, 1, 0));
			c->t_head.Rotate(aViewPitch.Rad(), m::Vector3(1, 0, 0));
			c->t_head.Translate(glm::vec3(0.f, 1.45f, 0.f));
		}
		else
			c->t_head.SetRotation(c->t_body.GetRotation());

		c->t_head.SetScale(m::Vector3(0.8f, 0.8f, 0.8f)); // temporary set scale (will resize mesh later, once I settle on a head size)
	}

	void TickItem(btf32 dt, btID i)
	{
		EItem* ent = (EItem*)_entities[i];
		GetCellSpaceInfo(ent->t.position, ent->csi);
		env::GetHeight(ent->t.height, ent->csi);
		btf32 angx;
		btf32 angy;
		ent->t_item.SetPosition(m::Vector3(ent->t.position.x, ent->t.height, ent->t.position.y));
		ent->t_item.SetRotation(ent->yaw.Rad());
		// terrain rotation
		//env::GetAngles(angx, angy, ent->csi);
		//ent->t_item.SetRotation(0.f);
		//ent->t_item.Rotate(angx, fw::Vector3(0.f, 0.f, 1.f));
		//ent->t_item.Rotate(-angy, fw::Vector3(0.f, 0.f, 1.f));
		//ent->t_item.SetScale(fw::Vector3(0.1f,2.f,0.1f));
	}

	void DrawChara(btID index)
	{
		Chara* ent = (Chara*)_entities[index];
		#ifndef DEF_EDITOR
		resAniStep.Flag(res::eFLIP_LR) ? graphics::SetFrontFaceInverse() : graphics::SetFrontFace();
		/*DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.aniTime / resAniStep.time), ent->t_skin, graphics::shader_blend, ent->t_body);
		DrawBlendMeshAtTransform(index, res::amset_arms.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.aniTime / resAniStep.time), ent->t_skin, graphics::shader_blend, ent->t_body);
		DrawBlendMeshAtTransform(index, res::amset_cloak.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.aniTime / resAniStep.time), res::t_equip_atlas, graphics::shader_blend, ent->t_body);
		*/
		if (resAniStep.Flag(res::eSMEAR))
		{
			DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, 0.2f), ent->t_skin, graphics::shader_blend, ent->t_body);
			DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, 0.4f), ent->t_skin, graphics::shader_blend, ent->t_body);
			DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, 0.6f), ent->t_skin, graphics::shader_blend, ent->t_body);
			DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, 0.8f), ent->t_skin, graphics::shader_blend, ent->t_body);
		}
		else
			DrawBlendMeshAtTransform(index, res::amset_legs.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.GetCurve()), ent->t_skin, graphics::shader_blend, ent->t_body);
		DrawBlendMeshAtTransform(index, res::amset_arms.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.GetCurve()), ent->t_skin, graphics::shader_blend, ent->t_body);
		DrawBlendMeshAtTransform(index, res::amset_cloak.models[iAniID], m::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.GetCurve()), res::t_equip_atlas, graphics::shader_blend, ent->t_body);
		graphics::SetFrontFace();
		// draw head
		DrawBlendMeshAtTransform(index, res::mb_head, 0, ent->t_skin, graphics::shader_blend, ent->t_head);
		DrawMeshAtTransform(index, res::m_equip_head_pickers, res::t_equip_atlas, graphics::shader_solid, ent->t_head);
		// draw item
		//CHARA(index)->musket.Draw(ePos, ENTITY(index)->t.height, eYaw2);
		#endif
	}

	void DrawItem(btID index)
	{
		EItem* ent = (EItem*)_entities[index];
		// Draw the mesh of our item id
		DrawMeshAtTransform(index, (res::assetid)archive::items[ent->itemid]->id_mesh, (res::assetid)archive::items[ent->itemid]->id_tex, graphics::shader_solid, ent->t_item);
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
				if (time > proj[index].ttd || ProjectileDoesIntersectEnv(index))
					DestroyProjectile(index);
				else // Otherwise
				{
					proj[index].t.height_velocity -= 0.001f;
					proj[index].t.position += proj[index].t.velocity * dt * 24.f; // Use speed variable
					proj[index].t.height += proj[index].t.height_velocity * dt * 24.f;
					proj[index].distance_travelled += m::Length(m::Vector3(proj[index].t.velocity.x * dt * 24.f, proj[index].t.velocity.y * dt * 24.f, proj[index].t.height_velocity * dt * 24.f));
					//proj[index].t.position += proj[index].t.velocity * dt * 4.f; // Use speed variable
					//proj[index].t.height += proj[index].t.height_velocity * dt * 4.f;
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
				//all-in-one transform/pitch/yaw
				graphics::MatrixTransform(model, pos,
					m::Vec2ToAng(m::Normalize(proj[index].t.velocity)), //yaw
					//m::Vec2ToAng(m::Normalize(m::Vector2(-proj[index].t.height_velocity, (abs(proj[index].t.velocity.x) + abs(proj[index].t.velocity.y)) / 2)))); // pitch
					m::Vec2ToAng(m::Normalize(m::Vector2(-proj[index].t.height_velocity, m::Length(proj[index].t.velocity))))); // pitch

				// Draw projectile mesh
				DrawMesh(ID_NULL, res::m_proj, res::t_proj, graphics::shader_solid, model);
				// Iterate smokepoint transformation (distance_travelled is way off real scale for some reason)
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
				if (time > proj[index].ttd || ProjectileDoesIntersectEnv(index)) // If it's time to die
				{
					DestroyProjectile(index);
					btui16 x = (btui16)roundf(proj[index].t.position.x);
					btui16 y = (btui16)roundf(proj[index].t.position.y);
					m::Vector2 vec = m::Normalize(proj[index].t.velocity);
					btui16 x2 = x + (btui16)ceilf(vec.x);
					btui16 y2 = y + (btui16)ceilf(vec.y);
					//t_EnvHeightmap.SetPixelChannelG((btui16)roundf(proj[index].t.position.x), (btui16)roundf(proj[index].t.position.y), 255ui8);
					--env::eCells[x][y].height;
					t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);
					if (env::eCells[x2][y2].height < env::eCells[x][y].height + 2ui8)
					{
						++env::eCells[x2][y2].height;
						t_EnvHeightmap.SetPixelChannelR(x2, y2, env::eCells[x2][y2].height);
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
					for (int i = 0; i <= block_entity.index_end; i++)
						if (block_entity.used[i] && ENTITY(i)->properties.get(Entity::eCOLLIDE_PRJ))
						{
							//get difference between positions
							m::Vector2 vec = proj[index].t.position - ENTITY(i)->t.position;
							//get distance
							float dist = m::Length(vec);
							if (dist < 0.5f)
							{
								// kill
								ENTITY(i)->state.hp -= 0.1f;
								if (ENTITY(i)->state.hp <= 0.f)
								{
									ENTITY(i)->state.properties.unset(ActiveState::eALIVE);
									ENTITY(i)->state.hp = 0.f;
								}
								DestroyProjectile(index); // Destroy the projectile
							}
						}
					/*
					CellGroup cg;
					GetCollisionCells(proj[index].t.position, cg);
					for (int i = 0; i < 4; i++)
					{
						#define X cg.c[i].x
						#define Y cg.c[i].y
						// For all entities in this cell
						for (int e = 0; e < cells[X][Y].ents.size(); e++)
						{
							#define ID cells[X][Y].ents[e]
							if (block_entity.used[ID] && ENTITY(ID)->state.alive)
							{
								//get difference between positions
								fw::Vector2 vec = proj[index].t.position - ENTITY(ID)->t.position;
								//get distance
								float dist = fw::Length(vec);
								if (dist < 0.5f)
								{
									// kill
									ENTITY(ID)->state.hp -= 0.1f;
									if (ENTITY(ID)->state.hp <= 0.f)
									{
										ENTITY(ID)->state.alive = false;
										ENTITY(ID)->state.hp = 0.f;
									}
									DestroyProjectile(index); // Destroy the projectile
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

		CellSpaceInfo csi;
		GetCellSpaceInfo(proj[index].t.position, csi);
		btf32 height;
		env::GetHeight(height, csi);
		if (proj[index].t.height < height) // if below the ground surface
			return true;

		return false;
	}

	#endif
}