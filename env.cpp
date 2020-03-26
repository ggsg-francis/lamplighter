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
	EnvNode eCells;

	//mem::CkBuffer<graphics::CompositeMesh> wldMeshes;
	graphics::CompositeMesh wldMeshes[NUM_COMPOSITES];
	graphics::TerrainMesh wldMeshTerrain;
	//graphics::CompositeMesh wldMesh;
	btID wldTxtr[NUM_COMPOSITES];
	btui32 wldNumTextures = 0u;

	bool Get(uint x, uint y, eflag::flag bit)
	{
		return mem::bvget((uint32_t)eCells.flags[x][y], (uint32_t)bit);
	}
	void Set(uint x, uint y, eflag::flag bit)
	{
		mem::bvset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}
	void UnSet(uint x, uint y, eflag::flag bit)
	{
		mem::bvunset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}

	void GetHeight(btf32& out_height, CellSpace& csinf)
	{
		// NEAREST
		out_height = (btf32)eCells.terrain_height[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y] / TERRAIN_HEIGHT_DIVISION;

		// BILINEAR
		/*
		out_height = m::Lerp(
			m::Lerp((btf32)eCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].height, (btf32)eCells[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y].height, abs(csinf.offsetx)),
			m::Lerp((btf32)eCells[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y].height, (btf32)eCells[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y].height, abs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
		*/
	}

	bool LineTraceUtil_HeightCheck(int x, int y, int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b)
	{
		// TODO: figure out the relative distance between agents and get an actual height test
		//btf32 dist_to_a = m;
		if (((btf32)env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION) > height_a + 0.7f)
		{
			return true;
		}
		if (((btf32)env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION) > height_b + 0.7f)
		{
			return true;
		}
		return false;
	}

	bool LineTrace_Bresenham(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b)
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
			if (Get(x, y, eflag::eIMPASSABLE) || LineTraceUtil_HeightCheck(x, y, x1, y1, x2, y2, height_a, height_b))
				return false;
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
				if (Get(x, y, eflag::eIMPASSABLE) || LineTraceUtil_HeightCheck(x, y, x1, y1, x2, y2, height_a, height_b))
					return false;
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
			if (Get(x, y, eflag::eIMPASSABLE) || LineTraceUtil_HeightCheck(x, y, x1, y1, x2, y2, height_a, height_b))
				return false;
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
				if (Get(x, y, eflag::eIMPASSABLE) || LineTraceUtil_HeightCheck(x, y, x1, y1, x2, y2, height_a, height_b))
					return false;
			}
		}
		return true;
	}

	void Tick()
	{
		// eventually, this function could contain any 'propagation' like fire
	}

	void DrawProps()
	{
		for (btui32 i = 0u; i < wldNumTextures; ++i)
		{
			DrawCompositeMesh(ID_NULL, wldMeshes[i], res::GetT(wldTxtr[i]), SS_NORMAL, graphics::Matrix4x4());
		}
		//graphics::SetFrontFaceInverse();
		//graphics::Matrix4x4 matr;
		//graphics::MatrixTransform(matr, 0.f, 0.f, 0.f);
		//DrawTerrainMesh(ID_NULL, wldMeshTerrain,
		//	res::GetT(res::t_gui_box), res::GetT(res::t_gui_hurt),
		//	res::GetT(res::t_terrain_sanddirt), res::GetT(res::t_terrain_sanddirt),
		//	matr);
		//graphics::SetFrontFace();

	}
	void DrawTerrain()
	{
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			res::GetT(res::t_terrain_scorch), res::GetT(res::t_terrain_sanddirt),
			res::GetT(res::t_terrain_sanddirt), res::GetT(res::t_terrain_sanddirt),
			graphics::Matrix4x4());
	}
	void DrawTerrainDebug()
	{
		graphics::Matrix4x4 matr;
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			res::GetT(res::t_terrain_scorch), res::GetT(res::t_terrain_sanddirt),
			res::GetT(res::t_terrain_sanddirt), res::GetT(res::t_terrain_sanddirt),
			matr);
		graphics::MatrixTransform(matr, m::Vector3(0.f, 0.01f, 0.f));
		graphics::SetRenderWire();
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			res::GetT(res::t_col_red), res::GetT(res::t_col_red),
			res::GetT(res::t_col_red), res::GetT(res::t_col_red),
			matr);
		graphics::SetRenderSolid();
	}

	void SaveBin()
	{
		printf("Saving [world.ltrwld]");
		FILE *out = fopen("save/world.ltrwld", "wb");
		if (out != NULL)
		{
			fseek(out, 0, SEEK_SET); // Seek the beginning of the file
			const size_t wrote = fwrite(&eCells, sizeof(EnvNode), 1, out);
			fclose(out);
		}
	}

	void LoadBin()
	{
		printf("Loading [world.ltrwld]");
		FILE *in = fopen("save/world.ltrwld", "rb");
		if (in != NULL)
		{
			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			const size_t read = fread(&eCells, sizeof(EnvNode), 1, in);
			fclose(in);
		}
		GenerateTerrainMesh();
		GeneratePropMeshes();
	}

	void Clean()
	{
		/*
		for (int x = 1; x < WORLD_SIZE - 1; x++)
		{
			for (int y = 1; y < WORLD_SIZE - 1; y++)
			{
				
			}
		}
		*/
	}

	void GeneratePropMeshes()
	{
		//temp
		//set_node_dir(8, 8, 5, 5, nbit::N);
		//set_node_dir(8, 8, 4, 5, nbit::E);
		//node_coord nc = get_node_from(8, 8, 5, 5);

		//*
		//wldMeshes.Add(new graphics::CompositeMesh());
		int tile_radius = 128;
		for (int x = 1024 - tile_radius; x < 1024 + tile_radius; ++x)
		{
			for (int y = 1024 - tile_radius; y < 1024 + tile_radius; ++y)
			{
				if (env::eCells.prop[x][y] != ID_NULL && env::eCells.prop[x][y] != 0u)
				{
					bool foundTxtr = false;
					// are we using a new texture on this asset
					for (btui32 i = 0; i < wldNumTextures; ++i)
					{
						if (acv::props[env::eCells.prop[x][y]].idTxtr == wldTxtr[i])
						{
							wldMeshes[i].AddMesh(&res::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
								m::Vector3((btf32)x, (btf32)eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, (btf32)y),
								(graphics::CompositeMesh::MeshOrientation)env::eCells.prop_dir[x][y]);
							foundTxtr = true;
						}
					}
					if (!foundTxtr)
					{
						for (btui32 i = 0; i < NUM_COMPOSITES; ++i)
						{
							if (wldTxtr[i] == 0u)
							{
								wldTxtr[i] = acv::props[env::eCells.prop[x][y]].idTxtr;
								wldMeshes[i].AddMesh(&res::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
									m::Vector3((btf32)x, (btf32)eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, (btf32)y),
									(graphics::CompositeMesh::MeshOrientation)env::eCells.prop_dir[x][y]);
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

	void GenerateTerrainMesh()
	{
		wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
		wldMeshTerrain.ReBindGL();
	}
}
