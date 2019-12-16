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

	void GetHeight(btf32& out_height, CellSpace& csinf)
	{
		// NEAREST
		out_height = (btf32)eCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].height / TERRAIN_HEIGHT_DIVISION;

		// BILINEAR
		/*
		out_height = m::Lerp(
			m::Lerp((btf32)eCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].height, (btf32)eCells[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y].height, abs(csinf.offsetx)),
			m::Lerp((btf32)eCells[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y].height, (btf32)eCells[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y].height, abs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
		*/
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
				UnSet(x, y, eflag::eSurfN);
				UnSet(x, y, eflag::eSurfS);
				UnSet(x, y, eflag::eSurfE);
				UnSet(x, y, eflag::eSurfW);
				UnSet(x, y, eflag::eCorOutNE);
				UnSet(x, y, eflag::eCorOutNW);
				UnSet(x, y, eflag::eCorOutSE);
				UnSet(x, y, eflag::eCorOutSW);

				//don't bother doing anything if this tile is not solid
				if (!Get(x, y, eflag::eIMPASSABLE))
					//if (!ltrmem::bitget(nodes[x][y].flags, EF_IMPASSABLE))
				{
					//edges
					//n
					if (Get(x, y + 1, eflag::eIMPASSABLE) || eCells[x][y + 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eSurfN);
					//s
					if (Get(x, y - 1, eflag::eIMPASSABLE) || eCells[x][y - 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eSurfS);
					//e
					if (Get(x + 1, y, eflag::eIMPASSABLE) || eCells[x + 1][y].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eSurfE);
					//w
					if (Get(x - 1, y, eflag::eIMPASSABLE) || eCells[x - 1][y].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eSurfW);
					//corners
					//ne
					if (Get(x + 1, y + 1, eflag::eIMPASSABLE) || eCells[x + 1][y + 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eCorOutNE);
					//nw
					if (Get(x - 1, y + 1, eflag::eIMPASSABLE) || eCells[x - 1][y + 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eCorOutNW);
					//se
					if (Get(x + 1, y - 1, eflag::eIMPASSABLE) || eCells[x + 1][y - 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eCorOutSE);
					//sw
					if (Get(x - 1, y - 1, eflag::eIMPASSABLE) || eCells[x + 1][y - 1].height > eCells[x][y].height + 2ui8)
						Set(x, y, eflag::eCorOutSW);
				}
			}
		}
	}
}