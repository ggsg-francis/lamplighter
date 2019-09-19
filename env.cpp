#include "env.h"
#include "global.h"

#include <iostream>
#include <vector>
#include "maths.hpp"

namespace env
{
	//get these tf out of here
	node_v001 eCells[WORLD_SIZE][WORLD_SIZE];

	bool Get(uint x, uint y, eflag::flag bit)
	{
		return mem::bvget((uint32_t)eCells[x][y].flags, (uint32_t)bit);
	}
	void Set(uint x, uint y, eflag::flag bit)
	{
		mem::bvset((uint32_t&)eCells[x][y].flags, (uint32_t)bit);
	}
	void UnSet(uint x, uint y, eflag::flag bit)
	{
		mem::bvunset((uint32_t&)eCells[x][y].flags, (uint32_t)bit);
	}

	void GetHeight(btf32& out_height, CellSpaceInfo& csinf)
	{
		out_height = m::Lerp(
			m::Lerp((btf32)eCells[csinf.c[0u].x][csinf.c[0u].y].height, (btf32)eCells[csinf.c[1u].x][csinf.c[1u].y].height, abs(csinf.offsetx)),
			m::Lerp((btf32)eCells[csinf.c[2u].x][csinf.c[2u].y].height, (btf32)eCells[csinf.c[3u].x][csinf.c[3u].y].height, abs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
	}

	void GetAngles(btf32& out_angx, btf32& out_angy, CellSpaceInfo& csinf)
	{
		#define run 1.f

		btf32 rise_x = m::Lerp(
			((btf32)eCells[csinf.c[0u].x][csinf.c[0u].y].height - (btf32)eCells[csinf.c[1u].x][csinf.c[1u].y].height),
			((btf32)eCells[csinf.c[2u].x][csinf.c[2u].y].height - (btf32)eCells[csinf.c[3u].x][csinf.c[3u].y].height),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
		btf32 rise_y = m::Lerp(
			((btf32)eCells[csinf.c[0u].x][csinf.c[0u].y].height - (btf32)eCells[csinf.c[2u].x][csinf.c[2u].y].height),
			((btf32)eCells[csinf.c[1u].x][csinf.c[1u].y].height - (btf32)eCells[csinf.c[3u].x][csinf.c[3u].y].height),
			abs(csinf.offsetx)) / TERRAIN_HEIGHT_DIVISION;

		out_angx = atanf(rise_x / run);
		out_angy = atanf(rise_y / run);

		#undef run
	}

	void Tick()
	{
		//
	}

	void SaveBin()
	{
		std::cout << "Saving [world.ltrwld]" << std::endl;
		FILE *out = fopen("save/world.ltrwld", "wb");
		if (out != NULL)
		{
			fseek(out, 0, SEEK_SET); // Seek the beginning of the file
			for (int x = 0; x < WORLD_SIZE; x++)
				const size_t wrote = fwrite(&eCells[x][0], sizeof(node_v001), WORLD_SIZE, out);
			fclose(out);
		}
	}
	void LoadBin()
	{
		std::cout << "Loading [world.ltrwld]" << std::endl;
		FILE *in = fopen("save/world.ltrwld", "rb");
		if (in != NULL)
		{
			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			for (int x = 0; x < WORLD_SIZE; x++)
				const size_t read = fread(&eCells[x][0], sizeof(node_v001), WORLD_SIZE, in);
			fclose(in);
		}

		//temp
		//set_node_dir(8, 8, 5, 5, nbit::N);
		//set_node_dir(8, 8, 4, 5, nbit::E);
		//node_coord nc = get_node_from(8, 8, 5, 5);


	}

	void GeneratePhysicsSurfaces()
	{
		//generate tile flags from binary info
		//probably temporary, we'll see.
		for (int x = 1; x < WORLD_SIZE - 1; x++)
		{
			for (int y = 1; y < WORLD_SIZE - 1; y++)
			{
				env::UnSet(x, y, eflag::eSurfN);
				env::UnSet(x, y, eflag::eSurfS);
				env::UnSet(x, y, eflag::eSurfE);
				env::UnSet(x, y, eflag::eSurfW);
				env::UnSet(x, y, eflag::eCorOutNE);
				env::UnSet(x, y, eflag::eCorOutNW);
				env::UnSet(x, y, eflag::eCorOutSE);
				env::UnSet(x, y, eflag::eCorOutSW);

				//don't bother doing anything if this tile is not solid
				if (!env::Get(x, y, eflag::eIMPASSABLE))
					//if (!ltrmem::bitget(env::nodes[x][y].flags, EF_IMPASSABLE))
				{
					//edges
					//n
					if (env::Get(x, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfN);
					//if (mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_N);
				//s
					if (env::Get(x, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfS);
					//if (mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_S);
				//e
					if (env::Get(x + 1, y, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfE);
					//if (mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_E);
				//w
					if (env::Get(x - 1, y, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfW);
					//if (mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_W);
				//corners
				//ne
					if (env::Get(x + 1, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutNE);
					//if (mem::bitget32_auto(env::nodes[x + 1][y + 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_NE);
				//nw
					if (env::Get(x - 1, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutNW);
					//if (mem::bitget32_auto(env::nodes[x - 1][y + 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_NW);
				//se
					if (env::Get(x + 1, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutSE);
					//if (mem::bitget32_auto(env::nodes[x + 1][y - 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_SE);
				//sw
					if (env::Get(x - 1, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutSW);
					//if (mem::bitget32_auto(env::nodes[x - 1][y - 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
					//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_SW);
				}
			}
		}
	}

	void GenerateModelTypes()
	{
		//generate tile flags from binary info
		//probably temporary, we'll see.
		for (int x = 1; x < WORLD_SIZE - 1; x++)
		{
			for (int y = 1; y < WORLD_SIZE - 1; y++)
			{
				//only draw models if its on an impassable tile (for now)
				if (env::Get(x, y, eflag::eIMPASSABLE))
				{
					bool face_n = false, face_s = false, face_e = false, face_w = false;

					//wall facing up
					if (!env::Get(x, y + 1, eflag::eIMPASSABLE))
					{
						face_n = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_N;
					}
					//wall facing down
					if (!env::Get(x, y - 1, eflag::eIMPASSABLE))
					{
						face_s = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_S;
					}
					//wall facing right
					if (!env::Get(x + 1, y, eflag::eIMPASSABLE))
					{
						face_e = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_E;
					}
					//wall facing left
					if (!env::Get(x - 1, y, eflag::eIMPASSABLE))
					{
						face_w = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_W;
					}
				}
			}
		}
	}
}

namespace ltr
{
	//layers contain cells which contain tiles

	//obviously doesnt belong in this file but who cares right now
	//eventuall encapsulate in class
	unsigned int tilesTemplate[8][8]
	{
		{	1,	1,	1,	1,	1,	1,	1,	1, },
		{	1,	0,	0,	0,	1,	0,	0,	1, },
		{	1,	1,	0,	0,	1,	0,	0,	1, },
		{	1,	0,	0,	0,	0,	0,	0,	1, }, //top side here for some reason
		{	1,	1,	0,	0,	0,	0,	0,	0, },
		{	1,	1,	0,	0,	1,	0,	0,	1, },
		{	1,	1,	0,	0,	0,	0,	0,	1, },
		{	1,	1,	1,	1,	1,	1,	1,	1, },
	};
}