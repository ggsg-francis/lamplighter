#include "env.h"
#include "global.h"

#include <iostream>
//#include <vector>
#include "memory.hpp"
#include "maths.hpp"
#include "graphics.hpp"
#include "archive.hpp"

// TODO: temporary until 'draw x soso' is removed from this file
//#include "objects.h"

#define NUM_COMPOSITES 8u

namespace env
{
	//get these tf out of here
	EnvNode eCells[WORLD_SIZE][WORLD_SIZE];

	//mem::CkBuffer<graphics::CompositeMesh> wldMeshes;
	graphics::CompositeMesh wldMeshes[NUM_COMPOSITES];
	//graphics::CompositeMesh wldMesh;
	btID wldTxtr[NUM_COMPOSITES];
	btui32 wldNumTextures = 0u;

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

	bool LineTrace(btf32 x1, btf32 y1, btf32 x2, btf32 y2)
	{
		btf32 fx = x1;
		btf32 fy = y1;

		btf32 ofsx = x1 - x2;
		btf32 ofsy = y1 - y2;
		btf32 len = sqrt(ofsx * ofsx + ofsy * ofsy);
		const btf32 stepLength = 0.5f; // 1 by default
		btui32 nbSteps = (btui32)floorf(len / stepLength);

		for (int i = 0; i < nbSteps; ++i)
		{
			bti32 ix = (bti32)roundf(fx);
			bti32 iy = (bti32)roundf(fy);
			if (env::Get(ix, iy, env::eflag::eIMPASSABLE)) return false;
			fx = m::Lerp((btf32)x1, (btf32)x2, (btf32)i * (stepLength / (btf32)nbSteps));
			fy = m::Lerp((btf32)y1, (btf32)y2, (btf32)i * (stepLength / (btf32)nbSteps));
		}
		return true;
	}

	bool LineTrace_Bresenham(int x1, int y1, int x2, int y2)
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
			if (Get(x, y, eflag::eIMPASSABLE)) return false;
			//t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
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
				if (Get(x, y, eflag::eIMPASSABLE)) return false;
				//t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
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
			if (Get(x, y, eflag::eIMPASSABLE)) return false;
			//t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
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
				if (Get(x, y, eflag::eIMPASSABLE)) return false;
				//t_EnvLightmap.SetPixelChannelG(x, y, 0xFFui8);
			}
		}
		return true;
	}


	void Tick()
	{
		// eventually, this function could contain any 'propagation' like fire
	}

	void Draw()
	{
		for (btui32 i = 0u; i < wldNumTextures; ++i)
		{
			DrawCompositeMesh(ID_NULL, wldMeshes[i], res::GetT(wldTxtr[i]), SS_NORMAL, graphics::Matrix4x4());
		}
	}

	void SaveBin()
	{
		std::cout << "Saving [world.ltrwld]" << std::endl;
		FILE *out = fopen("save/world.ltrwld", "wb");
		if (out != NULL)
		{
			fseek(out, 0, SEEK_SET); // Seek the beginning of the file
			for (int x = 0; x < WORLD_SIZE; x++)
				const size_t wrote = fwrite(&eCells[x][0], sizeof(EnvNode), WORLD_SIZE, out);
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
				const size_t read = fread(&eCells[x][0], sizeof(EnvNode), WORLD_SIZE, in);
			fclose(in);
		}

		//temp
		//set_node_dir(8, 8, 5, 5, nbit::N);
		//set_node_dir(8, 8, 4, 5, nbit::E);
		//node_coord nc = get_node_from(8, 8, 5, 5);

		//*
		//wldMeshes.Add(new graphics::CompositeMesh());
		int tile_radius = 24;
		for (int x = 1024 - tile_radius; x < 1024 + tile_radius; ++x)
		{
			for (int y = 1024 - tile_radius; y < 1024 + tile_radius; ++y)
			{
				if (env::eCells[x][y].prop != ID_NULL && env::eCells[x][y].prop != 0u)
				{
					bool foundTxtr = false;
					// are we using a new texture on this asset
					for (btui32 i = 0; i < wldNumTextures; ++i)
					{
						if (acv::props[env::eCells[x][y].prop].idTxtr == wldTxtr[i])
						{
							wldMeshes[i].AddMesh(&res::GetM(acv::props[env::eCells[x][y].prop].idMesh),
								m::Vector3((btf32)x, (btf32)eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, (btf32)y));
							foundTxtr = true;
						}
					}
					if (!foundTxtr)
					{
						for (btui32 i = 0; i < NUM_COMPOSITES; ++i)
						{
							if (wldTxtr[i] == 0u)
							{
								wldTxtr[i] = acv::props[env::eCells[x][y].prop].idTxtr;
								wldMeshes[i].AddMesh(&res::GetM(acv::props[env::eCells[x][y].prop].idMesh),
									m::Vector3((btf32)x, (btf32)eCells[x][y].height / TERRAIN_HEIGHT_DIVISION, (btf32)y));
								++wldNumTextures;
								break;
							}
						}
					}
				}
			}
		}
		for (btui32 i = 0; i < wldNumTextures; ++i)
		{
			wldMeshes[i].ReBindGL();
		}
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