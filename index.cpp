#include "index.h"
#include "index_util.h"
#include "index_fn.h"

unsigned int active_player_view = 0u;

namespace index
{
	#ifdef DEF_EDITOR
	btui32 GetCellX()
	{
		return ent::t[0].cellx;
	}
	btui32 GetCellY()
	{
		return ent::t[0].celly;
	}
	#endif // DEF_EDITOR

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- OLD FUNCTIONS (DEFUNCTION ZONE) --------------------------------------------------------------------

	void SetViewFocus(btID index)
	{
		active_player_view = index;
		viewpos = ent::t[active_player_view].position * -1.f;

		//1.6 is normal height
		//#define h ent::t[i].height + fw::Lerp(r_step.height_start, r_step.height_end, fAniTime / r_step.time)
		#define h 1.6f + fw::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.aniTime / resAniStep.time)
		graphics::SetMatProj(viewpos.x, viewpos.y, h, CHARA(active_player_view)->viewYaw.Rad(), CHARA(active_player_view)->viewPitch.Rad());
		#undef h
		graphics::SetMatView(0.f, 0.f, 0.f, 0.f); // eventually we can try and break setmatproj into 2 so setmatproj only has to be called once ever
	}

	fw::Vector2 GetViewOffset()
	{
		return viewpos;
	}

	void Init()
	{
		env::LoadBin();

		players[0] = SpawnEntity(ent::type::prefab_player, fw::Vector2(5.f, 2.f), 0.f);
		players[1] = SpawnEntity(ent::type::prefab_player, fw::Vector2(6.f, 2.f), 0.f);
		#ifndef DEF_EDITOR
		//SpawnEntity(ent::type::prefab_npc, fw::Vector2(4.f, 4.f), 0.f);
		//SpawnEntity(ent::type::npc, fw::Vector2(7.f, 5.f), 0.f);

		// This is going to blow up extremely fast if I don't automate it somehow

		fac::SetAllegiance(fac::undead, fac::player, fac::enemy);
		fac::SetAllegiance(fac::undead, fac::playerhunter, fac::enemy);
		fac::SetAllegiance(fac::undead, fac::undead, fac::allied);

		fac::SetAllegiance(fac::playerhunter, fac::undead, fac::enemy);
		fac::SetAllegiance(fac::playerhunter, fac::player, fac::enemy);
		fac::SetAllegiance(fac::playerhunter, fac::playerhunter, fac::allied);

		#endif

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
	int ticker_temp;

	void Tick(btf32 dt)
	{
		//ticker_temp++;
		//if (ticker_temp == 1000)
		//{
		//	Character* chr = (Character*)SpawnEntity(ENT_ZOMBIE);
		//	chr->SetSkin(&pRes->tex_zpc);
		//	ticker_temp = 0;
		//}

		//******************************** ITERATE THROUGH ENTITIES

		#ifndef DEF_EDITOR
		EntCheckDeath(0, block_entity.index_end); // Check hp of every entity
		#endif

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
		{
			if (block_entity.used[i])
			{
				//tick[0](dt);
				// call the appropriate tick function
				//tick[ent::type22[i]](dt, i);
				tick[_entities[i]->Type()](dt, i);
			}
		}

		#ifndef DEF_EDITOR
		EntDeintersect(0, block_entity.index_end);
		#endif

		//SpawnProjectile(0, fw::Vector2(1.5f, 1.5f), 45.f, 20.f); // creates bug
		//SpawnProjectile(0, fw::Vector2(1.f, 1.f), 45.f, 20.f); // no bug

		ProjectileTick(dt);

		//temporary destroy dead entities
		/*
		for (int i = 0; i <= block_entity.last_filled; i++)
			if (block_entity.filled[i])
				if (GetEnt(i)->hp == 0.f && GetEnt(i)->aiControlled)
					//temp
					index::DestroyEntity(i);
		*/

		//******************************** NETCODE TICK
		///*
		networkTimerTemp += time_delta;
		//put in network file uwu
		if (networkTimerTemp > 1.f / cfg::fSyncTimer)
		{
			while (networkTimerTemp > 1.f / cfg::fSyncTimer)
				networkTimerTemp -= 1.f / cfg::fSyncTimer;
			msg::set_unit_pose ptemp;
			ptemp.id = network::nid;
			//set positions
			//ptemp.px = actor_sphere->getGlobalPose().p.x;
			ptemp.px = ent::t[players[0]].position.x;
			ptemp.py = ent::t[players[0]].position.y;
			//set rotations
			ptemp.yaw = ent::yaw[players[0]].Deg();
			//ptemp.vh = actor::viewYaw[players[0]].Deg();
			//ptemp.vv = actor::viewPitch[players[0]].Deg();
			network::SendMsg(SET_UNIT_POSE, &ptemp);
		}//*/
	}

	void Draw()
	{
		EntityDraw();
		ProjectileDraw();

		btui32 drawrange = 8u; // Create min/max draw coordinates
		bti32 cxs = ent::t[active_player_view].cellx - drawrange;
		if (cxs < 0)
			cxs = 0;
		bti32 cxe = ent::t[active_player_view].cellx + drawrange;
		if (cxe > WORLD_SIZE - 1)
			cxe = WORLD_SIZE - 1;
		bti32 cys = ent::t[active_player_view].celly - drawrange;
		if (cys < 0)
			cys = 0;
		bti32 cye = ent::t[active_player_view].celly + drawrange;
		if (cye > WORLD_SIZE - 1)
			cye = WORLD_SIZE - 1;

		//put in debug display
		///*

		line_transform.SetScale(fw::Vector3(1, 1, 1));
		#ifdef DEF_EDITOR
		for (int x = cxs; x <= cxe; x++)
		{
			for (int y = cys; y < cye; y++)
			{
				if (!env::Get(x, y, env::eflag::eIMPASSABLE))
				{
					line_transform.SetPosition(fw::Vector3(x, 0, y));
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
					DrawMeshAtTransform(res::m_debugcell, res::t_guired, pRes->shader_solid, line_transform);
				}
			}
		}
		#endif // DEF_EDITOR

		line_transform.SetRotation(glm::radians(0.f));

		for (int x = cxs; x <= cxe; x++)
		{
			for (int y = cys; y < cye; y++)
			{
				if (x >= 0 && y >= 0)
				{
					if (!env::Get(x, y, env::eflag::eIMPASSABLE))
					{
						line_transform.SetPosition(fw::Vector3(x, 0, y));
						DrawMeshAtTransform(res::m_kitfloor, res::t_marshmud, pRes->shader_solid, line_transform);
					}
				}
			}
		}

		#ifndef DEF_EDITOR
		//line_transform.SetPosition(fw::Vector3( ai::pathnodes[ai::node].x, 0.f, ai::pathnodes[ai::node].y));
		//DrawMeshAtTransform(pRes->mdl_beachball, pRes->tex_bball, pRes->shader_solid, line_transform);
		#endif

		//*/

		//line_transform.SetPosition(fw::Vector3(players[0]->cellx, 0, players[0]->celly));
		//line_transform.SetScale(fw::Vector3(1.2, 1.2, 1.2));
		//DrawMeshAtTransform(pRes->mdl_debug_cell, pRes->tex_red, pRes->shader_solid, line_transform);

		#ifdef DEF_DEBUG_DISPLAY
		graphics::SetFrontFaceInverse();

		//draw p1 cell box

		line_transform.SetPosition(fw::Vector3(roundf(player1->t.position.x / 4.f) * 4.f, 0, roundf(player1->t.position.y / 4.f) * 4.f));
		line_transform.SetRotation(0.f);
		line_transform.SetScale(fw::Vector3(1, 1, 1));
		//line_transform.Draw(pRes->mdl_debug_cell, pRes->shader_solid, pRes->tex_yel);
		line_transform.Draw(pRes->mdl_debug_cell, pRes->shader_solid);


		//draw p1 cell lines

		int x_temp = (roundf((player1->t.position.x) / 4.f)) + 8;
		int y_temp = (roundf((player1->t.position.y) / 4.f)) + 8;

		float f = cells[x_temp][y_temp].ents.size() * 0.25f;
		line_transform.SetScale(fw::Vector3(f, 1.f, f));
		line_transform.Draw(pRes->mdl_debug_cell, pRes->shader_solid);

		for (int i = 0; i < cells[x_temp][y_temp].lvec.size(); i++)
		{
			line_transform.SetPosition(fw::Vector3(cells[x_temp][y_temp].lvec[i]->position.x, 0.f, cells[x_temp][y_temp].lvec[i]->position.y));
			line_transform.SetRotation(-cells[x_temp][y_temp].lvec[i]->rotation);
			line_transform.SetScale(fw::Vector3(cells[x_temp][y_temp].lvec[i]->scale.x * 2, 1.f, 1.f));
			//line_transform.Draw(pRes->mdl_debug_line, pRes->shader_solid, pRes->tex_yel);
			line_transform.Draw(pRes->mdl_debug_line, pRes->shader_solid);
		}

		#endif
	}

	void SetPose(void* msg)
	{
		msg::set_unit_pose* msg22 = (msg::set_unit_pose*)msg;
		ent::t[players[msg22->id]].position.x = msg22->px;
		ent::t[players[msg22->id]].position.y = msg22->py;
		ent::yaw[players[msg22->id]] = msg22->yaw;
		//actor::viewYaw[players[msg22->id]] = msg22->vh;
		//actor::viewPitch[players[msg22->id]] = msg22->vv;
	}

	void SetInput(btID index, fw::Vector2 input, btf32 rot_x, btf32 rot_y, bool atk, bool run, bool aim)
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

	void SpawnProjectile(int type, fw::Vector2 pos, float dir, float spread)
	{
		//highly un-optimized spread function
		float dir_deg = glm::degrees(dir);
		float LO = -spread / 2; float HI = spread / 2;
		float r3 = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		dir_deg += r3;
		dir = glm::radians(dir_deg);

		//p->id = block_proj.add(p);
		btID id = block_proj.add();
		proj[id].t.position = pos;
		proj[id].t.height = 0.9f;
		proj[id].t.velocity = fw::AngToVec2(dir) * 8.f;
		proj[id].ttd = time + 2.5f;
	}

	void DestroyProjectile(btID id)
	{
		block_proj.remove(id);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- THE NEW DOD FORMAT

	#define DEF_INDEX
	#ifdef DEF_INDEX

	btID SpawnEntity(ent::type::prefabtype type, fw::Vector2 pos, float dir)
	{
		//btID id;
		btID id = block_entity.add();
		CHARA2[id] = new Chara();

		if (type == ent::type::prefab_npc)
			prefab_npc(id, pos, dir);
		else if (type == ent::type::prefab_inanimate_bb)
			prefab_item_bb(id, pos, dir);
		else if (type == ent::type::prefab_player)
			prefab_pc(id, pos, dir);

		bti32 x = roundf(pos.x);
		if (x < 0 || x >= WORLD_SIZE)
			x = 0;
		bti32 y = roundf(pos.y);
		if (y < 0 || y >= WORLD_SIZE)
			y = 0;

		// optimize this $hit
		//index::cell_group group = index::GetCollisionCells(ent::t[i].position);
		//for (int i = 0; i < 4; i++)
		//{
		//	index::AddEntityCell(group.c[i].x, group.c[i].y, i);
		//}
		index::AddEntityCell(x, y, id);

		return id;
	}

	void DestroyEntity(btID id)
	{
		RemoveAllReferences(id);
		delete CHARA2[id];
		block_entity.remove(id);
		std::cout << "Destroyed entity " << id << std::endl;
	}

	inline void EntityCastProj(btID i)
	{
		SpawnProjectile(0, ent::t[i].position + (fw::AngToVec2(ent::yaw[i].Rad()) * 0.55f), ent::yaw[i].Rad(), 10.f);
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
		if (ent::state[index].alive)
		{
			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//------------- ANIMATION SET ------------------------------------

			switch (aniLower.AdvanceAnim(dt * fSpeed)) // Advance animation, get result
			{ // first 2 cases not needed yet
			case actor::anim_player::eNOEVENT:
				if (iAniID == res::knockback)
				{
					ent::t[index].velocity = ent::t[index].velocity * 0.8f;
					if (fw::Length(ent::t[index].velocity) < 0.01f) // If not sliding anymore
						aniLower.setAnim(res::idle); // Reset to idle
				}
				else if (resAniStep.Flag(res::f::can_start_moving))
				{
					if (fw::Length(f2Input) > 0.1f && aniLower.aniID == res::idle) // if moving forwards and idle
					{
						aniLower.aniTime = 0.f; // Reset timer
						bAniStepR = !bAniStepR;
						//CHARA(index)->charastatebv.setto(Chara::ani_right_foot, CHARA(index)->charastatebv.get(Chara::ani_right_foot));

						//if backwards
						if (f2Input.y < -0.01f)
						{
							if (bAniStepR) aniLower.setAnim(res::step_back_r);
							else aniLower.setAnim(res::step_back_l);
						}
						else
						{
							if (bvInRun)
							{
								if (bAniStepR) aniLower.setAnim(res::run_r);
								else aniLower.setAnim(res::run_l);
							}
							else
							{
								if (bAniStepR) aniLower.setAnim(res::step_r);
								else aniLower.setAnim(res::step_l);
							}
						}
					}
				}
				break;

			case actor::anim_player::eADVSTEP:
				if (resAniStep.Flag(res::f::cast_projectile))
				{
					EntityCastProj(index);
				}
				break;

			case actor::anim_player::eENDOFANIM:
				switch (aniLower.aniID)
				{
				case res::idle: // OR check 'can start moving' meta
					if (fw::Length(f2Input) > 0.1f && aniLower.aniID == res::idle) // if moving forwards and idle
					{
						aniLower.aniTime = 0.f; // Reset timer
						bAniStepR = !bAniStepR;
						//CHARA(index)->charastatebv.setto(Chara::ani_right_foot, CHARA(index)->charastatebv.get(Chara::ani_right_foot));

						//if backwards
						if (f2Input.y < -0.01f)
						{
							if (bAniStepR) aniLower.setAnim(res::step_back_r);
							else aniLower.setAnim(res::step_back_l);
						}
						else
						{
							if (bvInRun)
							{
								if (bAniStepR) aniLower.setAnim(res::run_r);
								else aniLower.setAnim(res::run_l);
							}
							else
							{
								if (bAniStepR) aniLower.setAnim(res::step_r);
								else aniLower.setAnim(res::step_l);
							}
						}
					}
					break;

				case res::step_l: case res::step_r: // Either forward steps
				case res::step_back_l: case res::step_back_r: // Either backward steps
				case res::run_l: case res::run_r: // Either run steps
					aniLower.setAnim(res::idle);
					break;

				case res::knockback:
					ent::t[index].velocity = ent::t[index].velocity * 0.8f;
					if (fw::Length(ent::t[index].velocity) < 0.01f) // If not sliding anymore
						aniLower.setAnim(res::idle); // Reset to idle
					break;
				}
				break;
			}

			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//------------- WIP RADIOACTIVE TAR PIT !!!DO NOT EXPOSE!!! ------

			if (aniLower.aniID != res::knockback)
			{
				//******************************** CALCULATE DIRECTION TO WALK IN

				// super shitty
				m::Angle movang = aViewYaw.Deg(); // Start at global head rotation
				if (f2Input.y < -0.01f) // Backwards
					movang.Rotate(glm::degrees(fw::Vec2ToAng(fw::Normalize(f2Input * -1.f)))); // Add reverse local angle
				else if(f2Input.y > 0.01f) // Forwards
					movang.Rotate(glm::degrees(fw::Vec2ToAng(fw::Normalize(f2Input)))); // Add local angle
				else if (f2Input.x > 0.01f || f2Input.x < -0.01f) // Left/right
					movang.Rotate(glm::degrees(fw::Vec2ToAng(fw::Normalize(f2Input)))); // Add local angle

				//******************************** PIVOT

				#ifdef DEF_PIVOT_OFFSET
				fw::Vector2 offset1 = fw::AngToVec2(aYaw.Rad()); // prev offset dir
				fw::Vector2 offset1_lr = fw::Vector2(-offset1.y, offset1.x); // prev offset dir
				#endif

				//if (resAniStep.can_turn && aniLower.aniID != res::anims::idle) // If can turn and is not idle
				if (resAniStep.Flag(res::f::can_turn)) // If can turn and is not idle
					aYaw.RotateTowards(movang.Deg(), 5.f); // Rotate body towards the target direction

				#ifdef DEF_PIVOT_OFFSET
				fw::Vector2 offset2 = fw::AngToVec2(aYaw.Rad()); // new offset dir
				fw::Vector2 offset2_lr = fw::Vector2(-offset2.y, offset2.x); // prev offset dir

				// rot pivot offset fb
				transf.velocity += (offset2 - offset1) * fw::Lerp(r_step.pivot_offset_s, r_step.pivot_offset_e, ani_time / r_step.time); // Add pivot rotation offset to the position
				// rot pivot offset lr
				transf.velocity += (offset2_lr - offset1_lr) * r_step.pivot_offset_lr; // Add pivot rotation offset to the position
				#endif

				//******************************** SET ENTITY VELOCITY

				ent::t[index].velocity = (fw::AngToVec2(aYaw.Rad()) * resAniStep.move_offset * dt / resAniStep.time) * fSpeed;
			}

			//******************************** APPLY MOVEMENT

			bMoving = (fw::Length(ent::t[index].velocity) > 0.016f);
			fw::Vector2 oldpos = ent::t[index].position;
			ent::t[index].position += ent::t[index].velocity; // Apply velocity

			bti32 x = roundf(ent::t[index].position.x);
			if (x < 0 || x >= WORLD_SIZE)
				x = 0;
			bti32 y = roundf(ent::t[index].position.y);
			if (y < 0 || y >= WORLD_SIZE)
				y = 0;

			//I don't want this to be here
			if (x != iCellX || y != iCellY)
			{
				// Old way ( one reference )
				//index::RemoveEntityCell(cellx, celly, id);
				//iCellX = x; iCellY = y;
				//index::AddEntityCell(cellx, celly, id);

				// New way ( four references )

				iCellX = x; iCellY = y;

				// optimize this $hit
				index::cell_group group = index::GetCollisionCells(oldpos);
				for (int i = 0; i < 4; i++)
				{
					index::RemoveEntityCell(group.c[i].x, group.c[i].y, i);
				}
				group = index::GetCollisionCells(ent::t[index].position);
				for (int i = 0; i < 4; i++)
				{
					index::AddEntityCell(group.c[i].x, group.c[i].y, i);
				}
			}

			//******************************** RUN AI

			if (CHARA(index)->aiControlled)
			{
				EntRunAI(index);
			}

			//******************************** TEMP!!!!!!!!!!!!!!

			if (CHARA(index)->musket.eMusketHoldState == Musket::aim) // IF AIMING
			{
				if (input::Get(input::key::action_a_hit))
					CHARA(index)->musket.eMusketHoldState = Musket::inspect_pan;
				if (input::Get(input::key::action_b_hit))
					CHARA(index)->musket.eMusketHoldState = Musket::inspect_barrel;
				CHARA(index)->musket.bvMusketState.setto(Musket::musket_state::latch_pulled, input::Get(input::key::atk_held));
			}
			else if (CHARA(index)->musket.eMusketHoldState == Musket::inspect_pan) // IF PAN
			{
				if (input::Get(input::key::action_a_hit))
					CHARA(index)->musket.eMusketHoldState = Musket::aim;
				if (input::Get(input::key::action_b_hit))
					CHARA(index)->musket.bvMusketState.toggle(Musket::musket_state::fpan_hatch_open);
				if (CHARA(index)->musket.bvMusketState.get(Musket::musket_state::fpan_hatch_open)) // if hatch open
					if (input::Get(input::key::action_c_hit)) // input
						CHARA(index)->musket.bvMusketState.set(Musket::musket_state::fpan_powder_in); // set powder
				CHARA(index)->musket.bvMusketState.setto(Musket::musket_state::latch_pulled, input::Get(input::key::atk_held));
			}
			else if (CHARA(index)->musket.eMusketHoldState == Musket::inspect_barrel) // IF BARREL
			{
				// if the hatch is open powder falls out lol
				if (CHARA(index)->musket.bvMusketState.get(Musket::fpan_hatch_open))
						CHARA(index)->musket.bvMusketState.unset(Musket::fpan_powder_in);

				if (input::Get(input::key::action_a_hit))
					CHARA(index)->musket.eMusketHoldState = Musket::aim;

				if (input::Get(input::key::atk_hit)) // set latch float
					CHARA(index)->musket.bvMusketState.toggle(Musket::musket_state::barrel_rod_in);

				CHARA(index)->musket.rod = fw::Lerp(CHARA(index)->musket.rod, (btf32)CHARA(index)->musket.bvMusketState.get(Musket::musket_state::barrel_rod_in), 0.1f);

				if (CHARA(index)->musket.rod > 0.95f)
					CHARA(index)->musket.bvMusketState.set(Musket::musket_state::barrel_armed); // unset powder
			}

			CHARA(index)->musket.fpan = fw::Lerp(CHARA(index)->musket.fpan, (btf32)CHARA(index)->musket.bvMusketState.get(Musket::musket_state::fpan_hatch_open), 0.3f);
			CHARA(index)->musket.lever = fw::Lerp(CHARA(index)->musket.lever, (btf32)CHARA(index)->musket.bvMusketState.get(Musket::musket_state::latch_pulled), 0.2f);

			if (CHARA(index)->musket.lever > 0.95f 
				//&& CHARA(index)->musket.bvMusketState.get(Musket::get_can_fire))
				&& CHARA(index)->musket.bvMusketState.get(Musket::barrel_armed)
				&& CHARA(index)->musket.bvMusketState.get(Musket::fpan_hatch_open)
				&& CHARA(index)->musket.bvMusketState.get(Musket::fpan_powder_in))
			{
				CHARA(index)->musket.bvMusketState.unset(Musket::unset_fire); // unset
				EntityCastProj(index);
			}
			else if (CHARA(index)->musket.lever > 0.95f
				//&& CHARA(index)->musket.bvMusketState.get(Musket::get_can_fire))
				&& CHARA(index)->musket.bvMusketState.get(Musket::fpan_hatch_open)
				&& CHARA(index)->musket.bvMusketState.get(Musket::fpan_powder_in))
			{
				CHARA(index)->musket.bvMusketState.unset(Musket::unset_fire); // unset
				//EntityCastProj(index);
			}
		} // End if alive
	}

	void TickMusket(btf32 dt, btID i)
	{

	}

	void EntityDraw()
	{
		for (int index = 0; index <= block_entity.index_end; index++)
		{
			if (block_entity.used[index])
			{
				Transform3D t_body, t_head, t_item;

				t_body.SetPosition(fw::Vector3(ent::t[index].position.x, ent::t[index].height + fw::Lerp(resAniStep.height_start, resAniStep.height_end, aniLower.aniTime / resAniStep.time), ent::t[index].position.y));
				t_body.Rotate(aYaw.Rad(), fw::Vector3(0, 1, 0));

				if (resAniStep.Flag(res::f::flip_lr))
				{
					graphics::SetFrontFaceInverse();
					t_body.SetScale(fw::Vector3(-1.f, 1.f, 1.f));
				}
				else
				{
					t_body.SetScale(fw::Vector3(1.f, 1.f, 1.f));
				}

				DrawBlendMeshAtTransform(pRes->mesh_chara_legs.models[iAniID], fw::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.aniTime / resAniStep.time), res::t_skin1, pRes->shader_blend, t_body);
				DrawBlendMeshAtTransform(pRes->mesh_cloak.models[iAniID], fw::Lerp(resAniStep.blend_start, resAniStep.blend_end, aniLower.aniTime / resAniStep.time), res::t_equip_atlas, pRes->shader_blend, t_body);

				graphics::SetFrontFace();

				// draw arms
				t_body.SetScale(fw::Vector3(1.f, 1.f, 1.f));
				DrawBlendMeshAtTransform(res::m_armscast, 0, res::t_skin1, pRes->shader_blend, t_body);

				// Set head transform
				t_head.SetPosition(t_body.GetPosition());
				//t_head.SetRotation(t_body.rot_glm_temp);
				t_head.Rotate(aViewYaw.Rad(), fw::Vector3(0, 1, 0));
				t_head.Rotate(aViewPitch.Rad(), fw::Vector3(1, 0, 0));
				t_head.Translate(glm::vec3(0.f,1.45f,0.f));
				t_head.SetScale(fw::Vector3(0.8f,0.8f,0.8f)); // temporary set scale (will resize mesh later, once I settle on a head size)
				// draw head
				DrawBlendMeshAtTransform(res::m_head, 0, res::t_skin1, pRes->shader_blend, t_head);
				DrawMeshAtTransform(res::m_equip_head_pickers, res::t_equip_atlas, pRes->shader_solid, t_head);

				// draw item
				t_item.SetPosition(t_body.GetPosition());
				t_item.Rotate(aYaw.Rad(), fw::Vector3(0, 1, 0));

				switch ((CHARA(index)->musket.eMusketHoldState))
				{
				case Musket::aim:
					CHARA(index)->musket.loc = fw::Lerp(CHARA(index)->musket.loc, fw::Vector3(0.13f, 0.9f, 0.3f), 0.1f);
					CHARA(index)->musket.yaw = fw::Lerp(CHARA(index)->musket.yaw, 0.f, 0.1f);
					CHARA(index)->musket.pitch = fw::Lerp(CHARA(index)->musket.pitch, 0.f, 0.1f);
					break;
				case Musket::inspect_pan:
					CHARA(index)->musket.loc = fw::Lerp(CHARA(index)->musket.loc, fw::Vector3(-0.23f, 1.2f, 0.4f), 0.1f);
					CHARA(index)->musket.yaw = fw::Lerp(CHARA(index)->musket.yaw, 45.f, 0.1f);
					CHARA(index)->musket.pitch = fw::Lerp(CHARA(index)->musket.pitch, -15.f, 0.1f);
					break;
				case Musket::inspect_barrel:
					CHARA(index)->musket.loc = fw::Lerp(CHARA(index)->musket.loc, fw::Vector3(0.f, 0.2f, 0.4f), 0.1f);
					CHARA(index)->musket.yaw = fw::Lerp(CHARA(index)->musket.yaw, 0.f, 0.1f);
					CHARA(index)->musket.pitch = fw::Lerp(CHARA(index)->musket.pitch, -80.f, 0.1f);
					break;
				}

				t_item.TranslateLocal(CHARA(index)->musket.loc); // set pose
				t_item.Rotate(glm::radians(CHARA(index)->musket.yaw), fw::Vector3(0.f, 1.f, 0.f));
				t_item.Rotate(glm::radians(CHARA(index)->musket.pitch), fw::Vector3(1.f, 0.f, 0.f));

				DrawMeshAtTransform(res::m_item_matchlock_01, res::t_item_matchlock_01, pRes->shader_solid, t_item);

				if (CHARA(index)->musket.bvMusketState.get(Musket::musket_state::fpan_powder_in))
					DrawBlendMeshAtTransform(res::mb_item_matchlock_01_pan_full, CHARA(index)->musket.fpan, res::t_item_matchlock_01, pRes->shader_blend, t_item);
				else
					DrawBlendMeshAtTransform(res::mb_item_matchlock_01_pan, CHARA(index)->musket.fpan, res::t_item_matchlock_01, pRes->shader_blend, t_item);

				DrawBlendMeshAtTransform(res::mb_item_matchlock_01_lever, CHARA(index)->musket.lever, res::t_item_matchlock_01, pRes->shader_blend, t_item);

				if (CHARA(index)->musket.eMusketHoldState == Musket::inspect_barrel || CHARA(index)->musket.bvMusketState.get(Musket::musket_state::barrel_rod_in)) // IF BARREL
					DrawBlendMeshAtTransform(res::mb_item_matchlock_01_rod, CHARA(index)->musket.rod, res::t_item_matchlock_01, pRes->shader_blend, t_item);

				//draw compass needle (temp until done with a GUI element)
				t_body.Translate(glm::vec3(0.f, 1.4f, 0.f));
				t_body.Translate(t_head.GetForward() * 0.5f);
				DrawMeshAtTransform(res::m_compass, res::t_guired, pRes->shader_solid, t_body);
			}
		}
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
				if (time > proj[index].ttd || ProjectileDoesIntersectEnv(index)) // If it's time to die
					DestroyProjectile(index);
				else // Otherwise
				{
					proj[index].t.position += proj[index].t.velocity * dt;
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
				Transform3D t;
				t.SetPosition(fw::Vector3(proj[index].t.position.x, proj[index].t.height, proj[index].t.position.y));
				DrawMeshAtTransform(res::m_proj, res::t_proj, pRes->shader_solid, t);
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
	///*
		//for every projectile
		for (int index = 0; index < block_proj.index_last; index++)
		{
			if (block_proj.used[index])
			{
				//cell_group cg = GetCollisionCells(GetPrj(p)->t.position);
				cell_group cg = GetCollisionCells(proj[index].t.position);
				for (int i = 0; i < 4; i++)
				{
					#define X cg.c[i].x
					#define Y cg.c[i].y
					// For all entities in this cell
					for (int e = 0; e < cells[X][Y].ents.size(); e++)
					{
						#define ID cells[X][Y].ents[e]
						if (block_entity.used[ID] && ent::state[ID].alive)
						{
							//get difference between positions
							fw::Vector2 vec = proj[index].t.position - ent::t[ID].position;
							//get distance
							float dist = fw::Length(vec);
							if (dist < 0.5f)
							{
								//kill
								//GETENT(ID)->hp -= 0.005f;
								//if (GETENT(ID)->hp < 0.f)
								//	GETENT(ID)->hp = 0.f;
								DestroyProjectile(index); // Destroy the projectile
							}
						}
						#undef ID
					}
					#undef X
					#undef Y
				}
			}
		}
	}

	bool ProjectileDoesIntersectEnv(btID index)
	{
		int x = roundf(proj[index].t.position.x);
		int y = roundf(proj[index].t.position.y);
		if (env::Get(x, y, env::eflag::eIMPASSABLE))
		{
			return true;
		}
		return false;
	}

	#endif
}