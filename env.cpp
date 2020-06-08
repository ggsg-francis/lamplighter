#include "env.h"
#include "global.h"

#include <iostream>
//#include <vector>
#include "memory.hpp"
#include "maths.hpp"
#include "graphics.hpp"
#include "archive.hpp"

#include "collision.h"

#include <queue>
#include <vector>

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

	bool Get(btui32 x, btui32 y, eflag::flag bit)
	{
		return mem::bvget((uint32_t)eCells.flags[x][y], (uint32_t)bit);
	}
	void Set(btui32 x, btui32 y, eflag::flag bit)
	{
		mem::bvset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}
	void UnSet(btui32 x, btui32 y, eflag::flag bit)
	{
		mem::bvunset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}

	void GetHeight(btf32& out_height, CellSpace& csinf)
	{
		// BILINEAR
		/*
		out_height = m::Lerp(
			m::Lerp((btf32)eCells.terrain_height[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(btf32)eCells.terrain_height[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y],
				fabs(csinf.offsetx)),
			m::Lerp((btf32)eCells.terrain_height[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y],
			(btf32)eCells.terrain_height[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y],
				fabs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
		//*/

		// BILINEAR2
		out_height = m::Lerp(
			m::Lerp((btf32)eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(btf32)eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
				(csinf.offsetx + 0.5f)),
			m::Lerp((btf32)eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(btf32)eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
				(csinf.offsetx + 0.5f)),
			(csinf.offsety + 0.5f)) / TERRAIN_HEIGHT_DIVISION;
	}

	void GetSlope(btf32& out_slope_x, btf32& out_slope_y, CellSpace& csinf)
	{
		out_slope_x = (btf32)(env::eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_x += (btf32)(env::eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_x /= 2.f;

		out_slope_y = (btf32)(env::eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_y += (btf32)(env::eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_y /= 2.f;
	}

	__forceinline bool LineTraceUtil_CheckCollideEnv(int x, int y, btf32 height_a, btf32 height_b, btf32 lerp)
	{
		btf32 mix = m::Lerp(height_a, height_b, lerp);
		if (((btf32)env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION) > mix + 0.7f)
			return true;
		return Get(x, y, eflag::eIMPASSABLE);
	}
	__forceinline bool LineTraceUtil_CompareA(int diff) {
		return diff < 0;
	}
	__forceinline bool LineTraceUtil_CompareB(int diff) {
		return diff <= 0;
	}
	__forceinline bool LineTraceUtil_Continue(
		int& coordX, int& coordY, int& coordA, int& coordB, int& diffX, int& diffY, int& diffA, int& diffB,
		int& x1, int& y1, int& x2, int& y2, int& a1, int& a2, int& diffAbsA, int& diffAbsB, bool(*compareFunc)(int),
		btf32 height_a, btf32 height_b)
	{
		int coord_end;
		// ??? (difference also called pk)
		int difference = 2 * diffAbsB - diffAbsA;
		// If the line travels positively, start at the beginning and go to the end
		if (diffA >= 0) {
			coordX = x1; coordY = y1; coord_end = a2;
		}
		// If the line travels negatively, take the reverse order
		else {
			coordX = x2; coordY = y2; coord_end = a1;
		}
		// Normally you would check the first tile here, but this is for checking vision not actual line drawing
		// and we can just assume we can see the tile we're standing on, lol
		// get distance between start and end
		btf32 axisLen = (btf32)(coordA - coord_end);
		// Loop until we reach the end (along the X or Y axis depending on the reference of coordA)
		for (int i = 0; coordA < coord_end; i++) {
			++coordA; // Increment along axis that is being looped
			if (compareFunc(difference)) // Check if the difference is < or <= zero, depending
				//???? modify diff somehow
				difference = difference + 2 * diffAbsB;
			else { // otherwise...
				if ((diffX < 0 && diffY < 0) || (diffX > 0 && diffY > 0))
					++coordB;
				else
					--coordB;
				//???? modify diff somehow
				difference = difference + 2 * (diffAbsB - diffAbsA);
			}
			// expensive calculations for height checking, get 0-1 value between start to end
			btf32 axisLerp = (btf32)(coordA - coord_end) / axisLen;
			// Check tile
			if (diffA < 0) {
				if (LineTraceUtil_CheckCollideEnv(coordX, coordY, height_a, height_b, axisLerp)) return true;
			}
			else {
				if (LineTraceUtil_CheckCollideEnv(coordX, coordY, height_b, height_a, axisLerp)) return true;
			}
		}
		return false;
	}
	bool LineTraceBh(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b)
	{
		int x, y, diff_x, diff_y, diff_abs_x, diff_abs_y;
		// Calculate the difference between each point
		diff_x = x2 - x1;
		diff_y = y2 - y1;
		// Get absolute differences
		diff_abs_x = abs(diff_x);
		diff_abs_y = abs(diff_y);
		// If X difference is longer
		if (diff_abs_y <= diff_abs_x) {
			if (LineTraceUtil_Continue(x, y, x, y, diff_x, diff_y, diff_x, diff_y,
				x1, y1, x2, y2, x1, x2, diff_abs_x, diff_abs_y, LineTraceUtil_CompareA, height_a, height_b))
				return false;
		}
		// If Y difference is longer
		else {
			if (LineTraceUtil_Continue(x, y, y, x, diff_x, diff_y, diff_y, diff_x,
				x1, y1, x2, y2, y1, y2, diff_abs_y, diff_abs_x, LineTraceUtil_CompareB, height_a, height_b))
				return false;
		}
		return true;
	}
	bool LineTrace_Bresenham_Bak(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b)
	{
		int x, y, diff_x, diff_y, diff_abs_x, diff_abs_y, difference, end;
		// Calculate the difference between each point
		diff_x = x2 - x1;
		diff_y = y2 - y1;
		// Get absolute differences
		diff_abs_x = abs(diff_x);
		diff_abs_y = abs(diff_y);
		// If X difference is longer
		if (diff_abs_y <= diff_abs_x) {
			//
			difference = 2 * diff_abs_y - diff_abs_x;
			// If the line travels positively, start at the beginning and go to the end
			if (diff_x >= 0) {
				x = x1; y = y1; end = x2;
			}
			// If the line travels negatively, take the reverse order
			else {
				x = x2; y = y2; end = x1;
			}
			// Check start point
			if (LineTraceUtil_CheckCollideEnv(x, y, height_a, height_b, 0.f)) return false;
			// Loop until we reach the end (loop along the X axis)
			for (int i = 0; x < end; i++) {
				x = x + 1;
				if (difference < 0)
					difference = difference + 2 * diff_abs_y;
				else {
					if ((diff_x < 0 && diff_y < 0) || (diff_x > 0 && diff_y > 0))
						y = y + 1;
					else
						y = y - 1;
					difference = difference + 2 * (diff_abs_y - diff_abs_x);
				}
				if (LineTraceUtil_CheckCollideEnv(x, y, height_a, height_b, 0.f)) return false;
			}
		}
		// If Y difference is longer
		else {
			//
			difference = 2 * diff_abs_x - diff_abs_y;
			if (diff_y >= 0) {
				x = x1; y = y1; end = y2;
			}
			else {
				x = x2; y = y2; end = y1;
			}
			// Check start point
			if (LineTraceUtil_CheckCollideEnv(x, y, height_a, height_b, 0.f)) return false;
			// Loop until we reach the end (loop along the Y axis)
			for (int i = 0; y < end; i++) {
				y = y + 1;
				if (difference <= 0)
					difference = difference + 2 * diff_abs_x;
				else {
					if ((diff_x < 0 && diff_y < 0) || (diff_x > 0 && diff_y > 0))
						x = x + 1;
					else
						x = x - 1;
					difference = difference + 2 * (diff_abs_x - diff_abs_y);
				}
				if (LineTraceUtil_CheckCollideEnv(x, y, height_a, height_b, 0.f)) return false;
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
			res::GetT(res::t_terrain_01), res::GetT(res::t_terrain_02),
			res::GetT(res::t_terrain_03), res::GetT(res::t_terrain_04),
			res::GetT(res::t_terrain_05), res::GetT(res::t_terrain_06),
			res::GetT(res::t_terrain_07), res::GetT(res::t_terrain_08),
			graphics::Matrix4x4());
	}
	void DrawTerrainDebug()
	{
		graphics::Matrix4x4 matr;
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			res::GetT(res::t_terrain_01), res::GetT(res::t_terrain_02),
			res::GetT(res::t_terrain_03), res::GetT(res::t_terrain_04),
			res::GetT(res::t_terrain_05), res::GetT(res::t_terrain_06),
			res::GetT(res::t_terrain_07), res::GetT(res::t_terrain_08),
			matr);
		graphics::MatrixTransform(matr, m::Vector3(0.f, 0.01f, 0.f));
		graphics::SetRenderWire();
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			res::GetT(res::t_col_black), res::GetT(res::t_col_black),
			res::GetT(res::t_col_black), res::GetT(res::t_col_black),
			res::GetT(res::t_col_black), res::GetT(res::t_col_black),
			res::GetT(res::t_col_black), res::GetT(res::t_col_black),
			matr);
		graphics::SetRenderSolid();
	}

	void SaveBin()
	{
		Clean();
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
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			for (int y = 0; y < WORLD_SIZE; ++y)
			{
				/*eCells.terrain_height_ne[x][y] = eCells.terrain_height[x][y];
				eCells.terrain_height_nw[x][y] = eCells.terrain_height[x][y];
				eCells.terrain_height_se[x][y] = eCells.terrain_height[x][y];
				eCells.terrain_height_sw[x][y] = eCells.terrain_height[x][y];*/
				eCells.terrain_height[x][y] = eCells.terrain_height_ne[x][y];
				/*eCells.terrain_height[x][y] = eCells.terrain_height_nw[x][y];
				eCells.terrain_height[x][y] = eCells.terrain_height_se[x][y];
				eCells.terrain_height[x][y] = eCells.terrain_height_sw[x][y];*/
			}
		}
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
			//wldMeshes[i].Unload();
		}
	}

	void GenerateTerrainMesh()
	{
		#ifdef DEF_TERRAIN_EXPERIMENTAL
		wldMeshTerrain.GenerateComplexEnv(eCells.terrain_height, eCells.terrain_material,
			(btui32*)&eCells.flags, eflag::EF_BLOCK_SHAPE,
			eCells.terrain_height_ne, eCells.terrain_height_nw,
			eCells.terrain_height_se, eCells.terrain_height_sw);
		#else
		wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
		#endif
	}
	void GenerateTerrainMeshEditor()
	{
		//wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
		#ifdef DEF_TERRAIN_EXPERIMENTAL
		wldMeshTerrain.GenerateComplexEnv(eCells.terrain_height, eCells.terrain_material,
			(btui32*)&eCells.flags, eflag::EF_BLOCK_SHAPE,
			eCells.terrain_height_ne, eCells.terrain_height_nw,
			eCells.terrain_height_se, eCells.terrain_height_sw);
		#else
		wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
		#endif
	}
}

enum NodeFromDir : btui8
{
	ND_UNSET,
	ND_FROM_NORTH,
	ND_FROM_SOUTH,
	ND_FROM_EAST,
	ND_FROM_WEST,
};
NodeFromDir node_cache[WORLD_SIZE][WORLD_SIZE];

void path::PathFind(void* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest)
{
	std::queue<PathNode> openSet;
	PathNode node; node.x = x; node.y = y;
	openSet.push(node);
	std::vector<PathNode> usedSet;
	usedSet.push_back(node);
	while(!openSet.empty())
	{
		PathNode current = openSet.front();
		openSet.pop();
		if (current.x == xDest && current.y == yDest)
			break;
		// N
		if (current.y < WORLD_SIZE_MAXINT && node_cache[current.x][current.y + 1] == ND_UNSET)
		{
			PathNode next; next.x = current.x; next.y = current.y + 1;
			openSet.push(next);
			node_cache[current.x][current.y + 1] = ND_FROM_SOUTH;
			usedSet.push_back(next);
		}
		// S
		if (current.y > 0 && node_cache[current.x][current.y - 1] == ND_UNSET)
		{
			PathNode next; next.x = current.x; next.y = current.y - 1;
			openSet.push(next);
			node_cache[current.x][current.y - 1] = ND_FROM_NORTH;
			usedSet.push_back(next);
		}
		// E
		if (current.x < WORLD_SIZE_MAXINT && node_cache[current.x + 1][current.y] == ND_UNSET)
		{
			PathNode next; next.x = current.x + 1; next.y = current.y;
			openSet.push(next);
			node_cache[current.x + 1][current.y] = ND_FROM_WEST;
			usedSet.push_back(next);
		}
		// W
		if (current.x > 0 && node_cache[current.x - 1][current.y] == ND_UNSET)
		{
			PathNode next; next.x = current.x - 1; next.y = current.y;
			openSet.push(next);
			node_cache[current.x - 1][current.y] = ND_FROM_EAST;
			usedSet.push_back(next);
		}
	}


	std::vector<PathNode>* path2 = ((std::vector<PathNode>*)path);

	// set path vector
	path2->clear();
	PathNode current; current.x = xDest; current.y = yDest;
	while (current.x != x && current.y != y)
	{
		path2->push_back(current);
		switch (node_cache[current.x][current.y])
		{
		case ND_FROM_NORTH:
			++current.y;
			break;
		case ND_FROM_SOUTH:
			--current.y;
			break;
		case ND_FROM_EAST:
			++current.x;
			break;
		case ND_FROM_WEST:
			--current.x;
			break;
		}
	}

	// clear modified cache
	for (int i = 0; i < usedSet.size(); ++i)
		node_cache[usedSet[i].x][usedSet[i].y] = ND_UNSET;
}
