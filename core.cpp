#include "core.h"
#include "core_func.cpp"
#include "weather.h"

#include "index.h"

#include "core_save_load.h"

unsigned int activePlayer = 0u;

namespace index
{
	btID viewtarget[2]{ ID_NULL,ID_NULL };
	btID viewtarget_last_tick[2]{ ID_NULL,ID_NULL };

	btf32 GetHP(btID id)
	{
		return ENTITY(id)->state.hp;
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
		viewpos = ENTITY(players[activePlayer])->t.position * -1.f;

		Chara* chara = CHARA(players[activePlayer]);
		#define h 1.6f + m::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.aniTime / resAniStep.time)
		cfg::bEditMode ?
			graphics::SetMatViewEditor(&chara->t_head) :
			graphics::SetMatView(&chara->t_head);
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

	void bhm_line(int x1, int y1, int x2, int y2)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = fabs(dx);
		dy1 = fabs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			if (env::Get(x, y, env::eflag::eIMPASSABLE)) return;
			t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}
				if (env::Get(x, y, env::eflag::eIMPASSABLE)) return;
				t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			if (env::Get(x, y, env::eflag::eIMPASSABLE)) return;
			t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}
				if (env::Get(x, y, env::eflag::eIMPASSABLE)) return;
				t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			}
		}
	}

	void newline(int x1, int y1, int x2, int y2)
	{
		btf32 fx = (btf32)x1;
		btf32 fy = (btf32)y1;

		for (btui32 i = 0u; i < 32u; ++i)
		{
			bti32 x = (bti32)roundf(fx);
			bti32 y = (bti32)roundf(fy);
			t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			//if (t_EnvLightmap.GetPixel(x, y).g < 255ui8 - 8ui8)
			//	t_EnvLightmap.SetPixelChannelG(x, y, t_EnvLightmap.GetPixel(x, y).g + 8ui8);
			if (env::Get(x, y, env::eflag::eIMPASSABLE)) return;
			fx = m::Lerp((btf32)x1, (btf32)x2, (btf32)i * (1.f / 32.f));
			fy = m::Lerp((btf32)y1, (btf32)y2, (btf32)i * (1.f / 32.f));
		}
	}

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
			graphics::GetShader((graphics::eShader)i).SetTexture(graphics::Shader::texSkyMap, res::GetT(res::t_sky).glID, graphics::Shader::TXTR_SKY);
		}
	}

	void UpdateOtherShaderParams(btf32 time2, m::Vector3 sunVec)
	{
		#define PCAM (glm::vec3)graphics::GetViewPos()
		for (int i = graphics::S_UTIL_FIRST_LIT; i <= graphics::S_UTIL_LAST_LIT; ++i)
		{
			graphics::GetShader((graphics::eShader)i).Use();
			graphics::GetShader((graphics::eShader)i).setMat4(graphics::Shader::matLightProj, *(graphics::Matrix4x4*)&shadowmat_temp);
			graphics::GetShader((graphics::eShader)i).SetFloat(graphics::Shader::fTime, (float)time2);
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::vecPCam, PCAM);
			graphics::GetShader((graphics::eShader)i).setVec3(graphics::Shader::vecVSun, (glm::vec3)sunVec);
			graphics::GetShader((graphics::eShader)i).SetTexture(graphics::Shader::texShadowMap, shadowtex, graphics::Shader::TXTR_SHADOWMAP);
		}
		#undef PCAM
	}

	void DoSpawn()
	{
		// temp
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			for (int y = 0; y < WORLD_SIZE; ++y)
			{
				if (env::Get(x, y, env::eflag::EF_SPAWN_TEST))
				{
					//env::UnSet(x, y, env::eflag::EF_SPAWN_TEST);
					std::cout << "spawn flag" << std::endl;
					//for (int i = 0; i < 8; ++i)
					btf32 dist;
					m::Vector2 pos1 = ENTITY(players[0])->t.position;
					m::Vector2 pos2 = ENTITY(players[1])->t.position;
					m::Vector2 pos3 = m::Vector2(x, y);
					if (m::Length(pos1 - pos3) > 8.f && m::Length(pos2 - pos3) > 8.f) // Don't spawn too close to either player
					{
						if (m::Length(pos1 - pos3) < 16.f && m::Length(pos2 - pos3) < 16.f) // ..or too far away
						{
							btf32 random = m::Random(0.f, 10.f);
							btui32 rand_rnd = (btui32)floor(random);
							if (rand_rnd < 2u)
								SpawnEntity(prefab::prefab_ai_player, m::Vector2(x, y), 0.f);
							else if (rand_rnd < 4u)
								SpawnEntity(prefab::prefab_npc, m::Vector2(x, y), 0.f);
							else
								SpawnEntity(prefab::prefab_zombie, m::Vector2(x, y), 0.f);
						}
					}
				}
			}
		}
	}

	void Init()
	{
		IndexInitialize();

		char buffinal[32] = "TSOA R";
		char buffer[4];
		char buffer2[4];
		//char buffer3[4];
		_itoa(VERSION_MAJOR, buffer, 10);
		_itoa(VERSION_BUILD, buffer2, 10);
		//_itoa(VERSION_BUILD, buffer3, 10);
		strcat(buffinal, buffer);
		strcat(buffinal, "-");
		strcat(buffinal, buffer2);
		//strcat(buffinal, " b");
		//strcat(buffinal, buffer3);
		strcat(buffinal, " ");
		strcat(buffinal, VERSION_COMMENT);

		text_version.ReGen(buffinal, cfg::iWinX * -0.125f, cfg::iWinX * 0.125f, cfg::iWinY * 0.25f);

		env::LoadBin();

		/*
		for (int x = 0; x < WORLD_SIZE; x++)
		{
			for (int y = 0; y < WORLD_SIZE; y++)
			{
				env::eCells[x][y].height = 128ui8;
			}
		}
		env::GeneratePhysicsSurfaces();
		env::SaveBin();
		//*/

		// Lightmap

		t_EnvLightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(255ui8, 0ui8, 0ui8, 255ui8));

		for (btui32 x = 0u; x < WORLD_SIZE; ++x)
			for (btui32 y = 0u; y < WORLD_SIZE; ++y)
				if (env::Get(x, y, env::eflag::EF_LIGHTSRC))
					flood_fill_temp(x, y, graphics::colour(0ui8, 255ui8, 0ui8, 0ui8));


		t_EnvLightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		// Heightmap

		t_EnvHeightmap.Init(WORLD_SIZE, WORLD_SIZE, graphics::colour(0ui8, 0ui8, 0ui8, 0ui8));

		for (btui16 x = 0; x < WORLD_SIZE; x++)
			for (btui16 y = 0; y < WORLD_SIZE; y++)
				t_EnvHeightmap.SetPixelChannelR(x, y, env::eCells[x][y].height);

		t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);

		// Spawnz
		if (cfg::bEditMode)
		{
			players[0] = SpawnEntity(prefab::PREFAB_EDITORPAWN, m::Vector2(1024.f, 1024.f), 0.f);
		}
		else
		{
			if (SaveExists())
			{
				LoadState();
				//SpawnNewEntityItem(6ui16, m::Vector2(m::Random(1023.f, 1026.f), m::Random(1023.f, 1026.f)), m::Random(0.f, 365.f));
			}
			else
			{
				players[0] = SpawnEntity(prefab::prefab_player, m::Vector2(1024.f, 1024.f), 0.f);
				players[1] = SpawnEntity(prefab::prefab_player, m::Vector2(1023.f, 1022.f), 0.f);

				SpawnNewEntityItem(0ui16, m::Vector2(1025.1f, 1022.6f), 15.f);
				SpawnNewEntityItem(1ui16, m::Vector2(1025.5f, 1024.0f), 120.f);
				SpawnNewEntityItem(2ui16, m::Vector2(1025.5f, 1023.3f), 15.f);
				SpawnNewEntityItem(3ui16, m::Vector2(1025.8f, 1024.f), 15.f);
				SpawnNewEntityItem(4ui16, m::Vector2(1026.8f, 1026.f), 15.f);
				SpawnNewEntityItem(5ui16, m::Vector2(1023.5f, 1023.3f), 15.f);
				SpawnNewEntityItem(6ui16, m::Vector2(1023.8f, 1023.4f), 15.f);
				SpawnNewEntityItem(7ui16, m::Vector2(1023.6f, 1023.2f), 15.f);

				DoSpawn();
			}
		}
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
	}

	//destroy all data
	void ClearBuffers()
	{
		for (int i = 0; i < BUF_SIZE; i++)
			//for (int i = 0; i <= block_entity.index_end; i++)
		{
			if (block_entity.used[i])
			{
				IndexFreeEntity(i);
				//block_entity.remove(i);
			}
			if (block_item.used[i])
			{
				IndexFreeItem(i);
				//ObjBuf_remove(&block_item, i);
			}
		}
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			for (int y = 0; y < WORLD_SIZE; ++y)
			{
				cells[x][y].ents.clear();
			}
		}
	}

	btf64 spawnz_time_temp = 0.;

	void Tick(btf32 dt)
	{
		/*if (!cfg::bEditMode && spawnz_time_temp < Time::time)
		{
			spawnz_time_temp = Time::time + 30.f;
			DoSpawn();
		}*/

		//temporary destroy dead entities
		///*
		for (int i = 0; i <= block_entity.index_end; i++)
			if (block_entity.used[i])
				if (!ACTOR(i)->state.stateFlags.get(ActiveState::eALIVE)
					&& !ACTOR(i)->state.stateFlags.get(ActiveState::eDIED_REPORT)
					&& ACTOR(i)->aiControlled)
					index::DestroyEntity(i);
		//*/

		//-------------------------------- ITERATE THROUGH ENTITIES

		// slow fast version
		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
		{
			if (block_entity.used[i])
			{
				ENTITY(i)->state.TickEffects(dt);
				fpTick[block_entity_data[i].type](ENTITY(i), dt); // Call tick on entity
			}
		}
		/*
		btui32 index_buf, index_ent;
		// fast slow version
		// start at one to skip the editor pawn buffer pointer, which points to the chara buffer
		for (index_buf = 1; index_buf < ENTITY_TYPE_COUNT; ++index_buf)
		{
			for (index_ent = 0; index_ent <= BufPtr[index_buf]->index_end; ++index_ent)
			{
				if (BufPtr[index_buf]->used[index_ent])
				{
					fpTick[index_buf]((Entity*)(((btui8*)(BufDataPtr[index_buf]) + index_ent * BufDataSize[index_buf])), dt);
				}
			}
		}
		*/

		SetViewTargetID(GetClosestActivator(players[0u]), 0u);
		SetViewTargetID(GetClosestActivator(players[1u]), 1u);

		ProjectileTick(dt);

		if (cfg::bEditMode)
		{
			Entity* entity = ENTITY(0);

			#define GetCellY entity->t.csi.c[0].y
			#define GetCellX entity->t.csi.c[0].x

			if (input::GetHit(input::key::USE))
			{
				//env::GeneratePaths();
			}
			else if (input::GetHit(input::key::ACTIVATE))
			{
				env::GeneratePhysicsSurfaces();
				env::SaveBin();
			}
			else if (input::GetHit(input::key::FUNCTION_1)) // COPY
			{
				editor_node_copy = env::eCells[GetCellX][GetCellY];
			}
			else if (input::GetHit(input::key::FUNCTION_2)) // PASTE
			{
				env::eCells[GetCellX][GetCellY] = editor_node_copy;
			}
			else if (input::GetHit(input::key::FUNCTION_3)) // TOGGLE LIGHT
			{
				env::Get(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC)
					? env::UnSet(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC) : env::Set(GetCellX, GetCellY, env::eflag::EF_LIGHTSRC);
			}
			else if (input::GetHit(input::key::FUNCTION_4)) // TOGGLE LIGHT
			{
				env::Get(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST)
					? env::UnSet(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST) : env::Set(GetCellX, GetCellY, env::eflag::EF_SPAWN_TEST);
			}
			else if (input::GetHit(input::key::ACTION_A))
			{
				env::Get(GetCellX, GetCellY, env::eflag::eIMPASSABLE) ? env::UnSet(GetCellX, GetCellY, env::eflag::eIMPASSABLE) : env::Set(GetCellX, GetCellY, env::eflag::eIMPASSABLE);
			}
			else if (input::GetHit(input::key::ACTION_B))
			{
				if (env::eCells[GetCellX][GetCellY].prop > 0u)
					--env::eCells[GetCellX][GetCellY].prop;
			}
			else if (input::GetHit(input::key::ACTION_C))
			{
				if (env::eCells[GetCellX][GetCellY].prop < acv::prop_index)
					++env::eCells[GetCellX][GetCellY].prop;
			}
			else if (input::GetHit(input::key::INV_CYCLE_L))
			{
				if (env::eCells[GetCellX][GetCellY].height > 0u)
					++env::eCells[GetCellX][GetCellY].height;
				t_EnvHeightmap.SetPixelChannelR(GetCellX, GetCellY, env::eCells[GetCellX][GetCellY].height);
				t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
			}
			else if (input::GetHit(input::key::INV_CYCLE_R))
			{
				if (env::eCells[GetCellX][GetCellY].height < 255)
					--env::eCells[GetCellX][GetCellY].height;
				t_EnvHeightmap.SetPixelChannelR(GetCellX, GetCellY, env::eCells[GetCellX][GetCellY].height);
				t_EnvHeightmap.ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
			}

			#undef GetCellX
			#undef GetCellY
		}
		else
		{
			if (input::GetHit(input::key::FUNCTION_5)) // SAVE
			{
				SaveState();
			}
			else if (input::GetHit(input::key::FUNCTION_9)) // LOAD
			{
				if (SaveExists())
					LoadState();
			}
		}
	}

	void Draw(bool oob)
	{
		btf32 time2 = Time::time * 0.02f + 0.2f;
		//btf32 time2 = 0.27f;
		//btf32 time2 = 0.22f;

		m::Vector2 sunrot = m::AngToVec2(glm::radians((floor(time2 * 360.f * 16.f) / 16.f) + 180.f));
		//m::Vector2 sunrot = m::AngToVec2(glm::radians(time2 * 360.f + 180.f));
		//glm::vec3 sunrot2 = glm::vec3(sunrot.x, sunrot.y, 0.f);
		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(sunrot.x, sunrot.y, sunrot.y * 1.2f));

		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(sunrot.x * 0.25f, 1.f, sunrot.x * 0.25f));
		//glm::vec3 sunrot2 = (glm::vec3)m::Normalize(m::Vector3(0.25f, 1.f, 0.25f));
		m::Vector3 sunVec = m::Vector3(0.f, 1.f, 0.f);

		graphics::Matrix4x4 matrix; // Matrix used for rendering env. props (so far...)

		if (oob)
		{
			//-------------------------------- UPDATE SHADERS

			UpdateOtherShaderParams(time2, sunVec);

			//-------------------------------- DRAW SKY

			graphics::SetFrontFace();
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
			m::Vector3 lightPos(ENTITY(activePlayer)->t.position.x, ENTITY(activePlayer)->t.height, -ENTITY(activePlayer)->t.position.y);
			m::Vector3 lightVecForw(-sunVec.x, -sunVec.y, -sunVec.z);
			//m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 1.f, 0.f)));
			m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 0.f, 1.f)));
			m::Vector3 LightVecUp = m::Normalize(m::Cross(lightVecForw, LightVecSide));

			#define LIGHT_RND ((SHADOW_WIDTH / LIGHT_WIDTH) / 2)

			btf32 moveF = roundf(m::Dot(lightVecForw, lightPos));
			btf32 moveS = roundf(m::Dot(LightVecSide, lightPos) * LIGHT_RND) / LIGHT_RND;
			btf32 moveU = roundf(m::Dot(LightVecUp, lightPos) * LIGHT_RND) / LIGHT_RND;

			#undef LIGHT_RND

			lightPos = lightVecForw * moveF + LightVecSide * moveS + LightVecUp * moveU;

			// Set light matrices
			graphics::SetMatProjLight(); graphics::SetMatViewLight(
				lightPos.x,
				lightPos.y,
				lightPos.z,
				-sunVec.x, -sunVec.y, -sunVec.z);//*/
			shadowmat_temp = graphics::GetMatProj() * graphics::GetMatView();
		}

		if (!cfg::bEditMode) if (oob) ProjectileDraw(); // Draw projectiles

		//-------------------------------- DRAW TERRAIN

		matrix = graphics::Matrix4x4();
		if (oob)
		{
			//graphics::MatrixTransform(matrix, m::Vector3(roundf(ENTITY(activePlayer)->t.position.x / 8) * 8, 0.f, roundf(ENTITY(activePlayer)->t.position.y / 8) * 8));
			//DrawMesh(ID_NULL, res::GetM(res::m_terrain_near), res::GetT(res::t_terrain_sanddirt), SS_TERRAIN, matrix);
		}

		//-------------------------------- DRAW ENTITIES AND PROPS

		#ifdef DEF_DRAW_WIREFRAME
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		#endif // DEF_DRAW_WIREFRAME

		if (cfg::bEditMode)
		{
			btui32 drawrange = 8u; // Create min/max draw coordinates
			bti32 minx = ENTITY(players[activePlayer])->t.csi.c[0].x - drawrange; if (minx < 0) minx = 0;
			bti32 maxx = ENTITY(players[activePlayer])->t.csi.c[0].x + drawrange; if (maxx > WORLD_SIZE - 1) maxx = WORLD_SIZE - 1;
			bti32 miny = ENTITY(players[activePlayer])->t.csi.c[0].y - drawrange; if (miny < 0) miny = 0;
			bti32 maxy = ENTITY(players[activePlayer])->t.csi.c[0].y + drawrange; if (maxy > WORLD_SIZE - 1) maxy = WORLD_SIZE - 1;
			for (bti32 x = minx; x <= maxx; x++)
			{
				for (bti32 y = miny; y < maxy; y++)
				{
					if (env::Get(x, y, env::eflag::eIMPASSABLE))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL, res::GetM(res::m_debug_bb), res::GetT(res::t_debug_bb), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_LIGHTSRC))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL, res::GetM(res::m_debug_bb), res::GetT(res::t_default), SS_NORMAL, matrix);
					}
					if (env::Get(x, y, env::eflag::EF_SPAWN_TEST))
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL, res::GetM(res::m_equip_head_pickers), res::GetT(res::t_default), SS_NORMAL, matrix);
					}
					if (env::eCells[x][y].prop == ID_NULL) env::eCells[x][y].prop = 0u;
					//-------------------------------- DRAW ENVIRONMENT PROP ON THIS CELL
					if (env::eCells[x][y].prop != ID_NULL && env::eCells[x][y].prop > 0u)
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL, res::GetM(acv::props[env::eCells[x][y].prop].idMesh), res::GetT(acv::props[env::eCells[x][y].prop].idTxtr), SS_NORMAL, matrix);
					}
					/*if (env::eCells[x][y].prop != ID_NULL && env::eCells[x][y].prop > 0u)
					{
						graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
						DrawMesh(ID_NULL,
							res::GetM(acv::props[env::eCells[x][y].prop].idMesh),
							res::GetT(acv::props[env::eCells[x][y].prop].idTxtr),
							SS_NORMAL, matrix);
					}*/
				}
			}
		}
		else
		{
			#define DRAWRANGE 16u
			Entity* entity = ENTITY(players[activePlayer]);
			// Set min/max draw coordinates
			bti32 minx = entity->t.csi.c[0].x - DRAWRANGE; if (minx < 0) minx = 0;
			bti32 maxx = entity->t.csi.c[0].x + DRAWRANGE; if (maxx > WORLD_SIZE - 1) maxx = WORLD_SIZE - 1;
			bti32 miny = entity->t.csi.c[0].y - DRAWRANGE; if (miny < 0) miny = 0;
			bti32 maxy = entity->t.csi.c[0].y + DRAWRANGE; if (maxy > WORLD_SIZE - 1) maxy = WORLD_SIZE - 1;
			for (bti32 x = minx; x <= maxx; x++) {
				for (bti32 y = miny; y < maxy; y++) {
					//-------------------------------- DRAW ENTITIES ON THIS CELL
					for (int e = 0; e <= cells[x][y].ents.end(); e++)
						if (cells[x][y].ents[e] != ID_NULL && block_entity.used[cells[x][y].ents[e]])
							//ENTITY(cells[x][y].ents[e])->fpDraw(ENT_VOID(cells[x][y].ents[e]));
							fpDraw[block_entity_data[cells[x][y].ents[e]].type](ENT_VOID(cells[x][y].ents[e]));
							//ENTITY(cells[x][y].ents[e])->Draw(cells[x][y].ents[e]);
					if (oob)
					{
						//-------------------------------- DRAW ENVIRONMENT PROP ON THIS CELL
						if (env::eCells[x][y].prop != ID_NULL && env::eCells[x][y].prop > 0u)
						{
							//graphics::MatrixTransform(matrix, m::Vector3(x, env::eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, y));
							//DrawMesh(ID_NULL, res::GetM(acv::props[env::eCells[x][y].prop].idMesh), res::GetT(acv::props[env::eCells[x][y].prop].idTxtr), SS_NORMAL, matrix);
						}
					}
				}
			}
			if (oob) env::Draw();
		}

		#ifdef DEF_DRAW_WIREFRAME
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		#endif // DEF_DRAW_WIREFRAME

		/*
		m::Vector3 lightPos(ENTITY(activePlayer)->t.position.x, ENTITY(activePlayer)->t.height, ENTITY(activePlayer)->t.position.y);
		m::Vector3 lightVecForw (-sunrot2.x, -sunrot2.y, -sunrot2.z);
		m::Vector3 LightVecSide = m::Normalize(m::Cross(lightVecForw, m::Vector3(0.f, 1.f, 0.f)));
		m::Vector3 LightVecUp = m::Normalize(m::Cross(lightVecForw, LightVecSide));

		btf32 moveF = roundf(m::Dot(lightVecForw, lightPos));
		btf32 moveS = roundf(m::Dot(LightVecSide, lightPos));
		btf32 moveU = roundf(m::Dot(LightVecUp, lightPos));
		//btf32 moveF = (m::Dot(lightVecForw, lightPos));
		//btf32 moveS = (m::Dot(LightVecSide, lightPos));
		//btf32 moveU = (m::Dot(LightVecUp, lightPos));

		lightPos = lightVecForw * moveF + LightVecSide * moveS + LightVecUp * moveU;

		graphics::MatrixTransform(matrix, lightPos);
		DrawMesh(ID_NULL, res::GetM(res::m_debug_bb), SS_NORMAL, matrix);
		//*/

		// GRASS DRAWING TEST
		/*
		if (oob)
		{
			graphics::MatrixTransform(matrix, m::Vector3(1024, 128 / TERRAIN_HEIGHT_DIVISION, 1024.5));

			//DrawMesh(0u, res::GetM(res::m_debug_sphere), res::GetT(res::t_meat_test), SS_NORMAL, matrix);
			graphics::Shader* shd = &graphics::GetShader(graphics::S_MEAT);
			// Enable the shader
			shd->Use();
			// Set matrices on shader
			shd->setMat4("matp", graphics::GetMatProj());
			shd->setMat4("matv", graphics::GetMatView());
			shd->setMat4("matm", matrix);
			// Render the mesh
			res::GetM(res::m_debug_sphere).Draw(res::GetT(res::t_meat_test).glID, shd->ID);
			graphics::MatrixTransform(matrix, m::Vector3(1024, 128 / TERRAIN_HEIGHT_DIVISION, 1023.5));
			shd->setMat4("matm", matrix);
			res::GetM(res::m_debug_sphere).Draw(res::GetT(res::t_meat_test).glID, shd->ID);
		}
		else
		{
			graphics::MatrixTransform(matrix, m::Vector3(1024, 21.75, 1024.5));

			//DrawMesh(0u, res::GetM(res::m_debug_sphere), res::GetT(res::t_meat_test), SS_NORMAL, matrix);
			graphics::Shader* shd = &graphics::GetShader(graphics::S_SOLID);
			// Enable the shader
			shd->Use();
			// Set matrices on shader
			shd->setMat4("matp", graphics::GetMatProj());
			shd->setMat4("matv", graphics::GetMatView());
			shd->setMat4("matm", matrix);
			// Render the mesh
			res::GetM(res::m_debug_sphere).Draw(res::GetT(res::t_meat_test).glID, shd->ID);
			graphics::MatrixTransform(matrix, m::Vector3(1024, 21.6, 1023.5));
			shd->setMat4("matm", matrix);
			res::GetM(res::m_debug_sphere).Draw(res::GetT(res::t_meat_test).glID, shd->ID);
		}
		*/
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
				if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_RESTING_ITEM)
					ACTOR(players[activePlayer])->PickUpItem(viewtarget[activePlayer]);
				else if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_CHARA)
				{
					// SOUL TRANSFER
					if (players[0] != players[1]) // If both player's arent using the same entity
						ACTOR(players[activePlayer])->aiControlled = true; // Let the AI take over
					players[activePlayer] = viewtarget[activePlayer]; // Set player to control player's view target
					ACTOR(players[activePlayer])->aiControlled = false;
				}
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
				if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_RESTING_ITEM)
					ACTOR(players[activePlayer])->PickUpItem(viewtarget[activePlayer]);
				else if (viewtarget[activePlayer] != ID_NULL && ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_CHARA)
				{
					// SOUL TRANSFER
					// Possession, actually
					// If both players aren't controlling the same entity, let the AI take over
					if (players[0] != players[1])
						ACTOR(players[activePlayer])->aiControlled = true;
					// Set the player to possess their view target entity
					players[activePlayer] = viewtarget[activePlayer];
					// Disable the AI on the possessed entity
					ACTOR(players[activePlayer])->aiControlled = false;
				}
			if (input::GetHit(input::key::C_DROP_HELD))
				ACTOR(players[activePlayer])->DropItem(ACTOR(players[activePlayer])->inv_active_slot);
		}
	}

	void GUISetMessag(int player, char* string)
	{
		// Player 1, X Start
		int p1_x_start = -(int)graphics::FrameSizeX() / 2;
		int p1_y_start = -(int)graphics::FrameSizeY() / 2;
		text_message[player].ReGen(string, -52, 52, -32);
		message_time[player] = Time::time + 3.0;
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
		Chara* chara = CHARA(players[activePlayer]);

		//text_message[activePlayer].ReGen("teststr", 0, -p1_x_start, 0);
		if (message_time[activePlayer] > Time::time)
			text_message[activePlayer].Draw(&res::GetT(res::t_gui_font));

		// croshair
		//graphics::DrawGUITexture(&res::GetT(res::t_gui_crosshair), &graphics::GetShader(graphics::S_GUI), 0, 0, 32, 32);
		// hp
		graphics::DrawGUITexture(&res::GetT(res::t_gui_bar_red), p1_x_start + 32, p1_y_start + 8, (int)(index::GetHP(index::players[activePlayer]) * 64.f), 16);
		// enemy hp
		if (viewtarget[activePlayer] != ID_NULL && viewtarget[activePlayer] != index::players[activePlayer]) // If not null or player
		{
			int textboxX = p1_x_start + 16;
			int textboxY = p1_y_start + 64;
			if (viewtarget[activePlayer] != viewtarget_last_tick[activePlayer]) // if target has changed
			{
				//text_temp.ReGen(ENTITY(viewtarget[activePlayer])->GetDisplayName(), textboxX, textboxX + 512, textboxY);
				//text_temp.ReGen(ENTITY(viewtarget[activePlayer])->fpName(ENT_VOID(viewtarget[activePlayer])), textboxX, textboxX + 512, textboxY);
				text_temp.ReGen(fpName[block_entity_data[viewtarget[activePlayer]].type](ENT_VOID(viewtarget[activePlayer])), textboxX, textboxX + 512, textboxY);
				guibox.ReGen(textboxX, textboxX + text_temp.sizex, textboxY - text_temp.sizey, textboxY, 4, 10);
			}
			if (ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_CHARA)
				graphics::DrawGUITexture(&res::GetT(res::t_gui_bar_yellow), p1_x_start + 32, p1_y_start + 24, (int)(index::GetHP(viewtarget[activePlayer]) * 64.f), 16);
			guibox.Draw(&res::GetT(res::t_gui_box));
			text_temp.Draw(&res::GetT(res::t_gui_font));
			if (ENTITY(viewtarget[activePlayer])->type == ENTITY_TYPE_RESTING_ITEM)
				graphics::DrawGUITexture(&res::GetT(res::t_gui_icon_pick_up), textboxX + 16, textboxY + 32, 32, 32);
		}
		// inventory
		chara->inventory.Draw(chara->inv_active_slot);
	}

	void DrawPostDraw()
	{
		text_version.Draw(&res::GetT(res::t_gui_font));
		char buffer[16];
		int i = snprintf(buffer, 16, "%f", 1.f / Time::deltaTick);
		text_fps.ReGen(buffer, cfg::iWinX * -0.125f, cfg::iWinX * 0.125f, cfg::iWinY * 0.25f - 16.f);
		text_fps.Draw(&res::GetT(res::t_gui_font));
	}

	void SetInput(btID index, m::Vector2 input, btf32 rot_x, btf32 rot_y, bool atk, bool atk_hit, bool atk2, bool run, bool aim, bool ACTION_A, bool ACTION_B, bool ACTION_C)
	{
		ACTOR(players[index])->input = input;
		ACTOR(players[index])->viewYaw.Rotate(rot_x);
		ACTOR(players[index])->viewPitch.RotateClamped(rot_y, -80.f, 70.f);
		ACTOR(players[index])->inputBV.setto(Actor::IN_USE, atk);
		ACTOR(players[index])->inputBV.setto(Actor::IN_USE_HIT, atk_hit);
		ACTOR(players[index])->inputBV.setto(Actor::IN_USE_ALT, atk2);
		ACTOR(players[index])->inputBV.setto(Actor::IN_RUN, run);
		ACTOR(players[index])->inputBV.setto(Actor::IN_AIM, aim);
		ACTOR(players[index])->inputBV.setto(Actor::IN_ACTN_A, ACTION_A);
		ACTOR(players[index])->inputBV.setto(Actor::IN_ACTN_B, ACTION_B);
		ACTOR(players[index])->inputBV.setto(Actor::IN_ACTN_C, ACTION_C);
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

		btID id = IndexSpawnProjectile();
		proj[id].t.position_x = pos.x;
		proj[id].t.position_y = pos.y;
		//proj[id].t.height = 0.9f + height;
		//proj[id].t.height = 1.3f + height;
		proj[id].t.position_h = height;

		proj[id].t.velocity_h = -sin(pitch);
		proj[id].t.velocity_x = (m::AngToVec2(yaw) * cos(pitch)).x; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj[id].t.velocity_y = (m::AngToVec2(yaw) * cos(pitch)).y; // '* cos(pitch)' makes it move less horizontally if shot upwards
		proj[id].ttd = Time::time + 2.0;

		proj[id].faction = faction;
	}

	void DestroyProjectile(btID id)
	{
		IndexDestroyProjectileC(id);
	}

	btID SpawnNewEntityItem(btID item_template, m::Vector2 pos, btf32 dir)
	{
		btID id = block_entity.add();
		IndexInitEntity(id, ENTITY_TYPE_RESTING_ITEM);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::none;
		ENTITY(id)->properties.set(Entity::ePREFAB_ITEM);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ITEM(id)->item_instance = SpawnItem(item_template);
		return id;
	}
	btID SpawnEntityItem(btID itemid, m::Vector2 pos, btf32 dir)
	{
		btID id = block_entity.add();
		IndexInitEntity(id, ENTITY_TYPE_RESTING_ITEM);
		spawn_setup_t(id, pos, dir);
		ENTITY(id)->faction = fac::faction::none;
		ENTITY(id)->properties.set(Entity::ePREFAB_ITEM);
		ENTITY(id)->state.stateFlags.set(ActiveState::eALIVE);
		ITEM(id)->item_instance = itemid;
		ENTITY(id)->radius = acv::items[((HeldItem*)GetItemPtr(itemid))->item_template]->f_radius;
		return id;
	}

	btID SpawnEntity(btui8 type, m::Vector2 pos, float dir)
	{
		btID id = block_entity.add();
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
	void DestroyEntity(btID id)
	{
		RemoveAllReferences(id);
		IndexFreeEntity(id);
		std::cout << "Destroyed entity " << id << std::endl;
	}

	btID SpawnItem(btID item_template)
	{
		btID id = ObjBuf_add(&block_item);
		if (id != BUF_NULL)
		{
			IndexInitItem(id, acv::item_types[item_template]);
			GETITEM_MISC(id)->item_template = item_template;
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
		IndexFreeItem(id);
		std::cout << "Destroyed item " << id << std::endl;
	}

	void ActorCastProj(btID i)
	{
		SpawnProjectile(ENTITY(i)->faction, ENTITY(i)->t.position + (m::AngToVec2(ENTITY(i)->t.yaw.Rad()) * 0.55f), ENTITY(i)->t.height, ACTOR(i)->viewYaw.Rad(), ACTOR(i)->viewPitch.Rad(), 1.f);
	}

	int cater_loop_index(int i)
	{
		i++;
		if (i == BUF_SIZE)
			i = 0; // loop around
		return i;
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- PROJECTILES

	//-------------------------------- PROJECTILE FORWARD DECLARATION

	bool ProjectileDoesIntersectEnv(btID id);

	//-------------------------------- PROJECTILE FUNCTIONS

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
				if (proj[index].t.position_x <= 1.f || proj[index].t.position_x >= WORLD_SIZE - 1 ||
					proj[index].t.position_y <= 1.f || proj[index].t.position_y >= WORLD_SIZE - 1)
					DestroyProjectile(index);
				// If it's time to die, or collided
				if (Time::time > proj[index].ttd || ProjectileDoesIntersectEnv(index))
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
				DrawMesh(ID_NULL, res::GetM(res::m_proj), res::GetT(res::t_proj), SS_NORMAL, model);
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
					for (int i = 0; i <= block_entity.index_end; i++)
					{
						if (block_entity.used[i] && ENTITY(i)->properties.get(Entity::eCOLLIDE_PRJ))
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
										// to do: pass angle to damage fn
										ENTITY(i)->state.Damage(0.15f, glm::degrees(m::Vec2ToAng(vec)));
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

	bool ProjectileDoesIntersectEnv(btID index)
	{
		int x = (int)roundf(proj[index].t.position_x);
		int y = (int)roundf(proj[index].t.position_y);
		if (env::Get(x, y, env::eflag::eIMPASSABLE)) // if hit an impassable tile
			return true;

		CellSpace csi;
		GetCellSpaceInfo(m::Vector2(proj[index].t.position_x, proj[index].t.position_y), csi);
		btf32 height;
		env::GetHeight(height, csi);
		if (proj[index].t.position_h < height) // if below the ground surface
			return true;

		return false;
	}
}