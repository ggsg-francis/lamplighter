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

#define CUTE_C2_IMPLEMENTATION
#include "3rdparty\cute_c2.h"

#define NUM_COMPOSITES 8u

//#define WORLD_HEIGHT_MAX 1024.f
//#define WORLD_HEIGHT_MIN -1024.f

#define WORLD_HEIGHT_MAX INFINITY
#define WORLD_HEIGHT_MIN -INFINITY

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

	mem::idbuf triCells[WORLD_SIZE][WORLD_SIZE];

	// Points (for navigation)
	EnvVert* points = nullptr;
	btui32 pointcount = 0u;
	// Tris (for physics, and partially navigation)
	EnvTri* tris = nullptr;
	c2Poly* triCollides = nullptr;
	btui32 tricount = 0u;
	// Lines, represent walls in the collision
	EnvLineSeg* lines = nullptr;
	btui32 linecount = 0u;

	m::Vector2 lineLineIntersection(m::Vector2 A, m::Vector2 B, m::Vector2 C, m::Vector2 D)
	{
		// Line AB represented as a1x + b1y = c1 
		btf32 a1 = B.y - A.y;
		btf32 b1 = A.x - B.x;
		btf32 c1 = a1*(A.x) + b1*(A.y);

		// Line CD represented as a2x + b2y = c2 
		btf32 a2 = D.y - C.y;
		btf32 b2 = C.x - D.x;
		btf32 c2 = a2*(C.x) + b2*(C.y);

		btf32 determinant = a1*b2 - a2*b1;

		if (determinant == 0.f) {
			// The lines are parallel. This is simplified 
			// by returning a pair of FLT_MAX 
			return m::Vector2(FLT_MAX, FLT_MAX);
		}
		else {
			btf32 x = (b2*c1 - b1*c2) / determinant;
			btf32 y = (a1*c2 - a2*c1) / determinant;
			return m::Vector2(x, y);
		}
	}

	void EnvTriBary(m::Vector2 p, m::Vector2 a, m::Vector2 b, m::Vector2 c, btf32 &u, btf32 &v, btf32 &w)
	{
		m::Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
		btf32 d00 = m::Dot(v0, v0);
		btf32 d01 = m::Dot(v0, v1);
		btf32 d11 = m::Dot(v1, v1);
		btf32 d20 = m::Dot(v2, v0);
		btf32 d21 = m::Dot(v2, v1);
		btf32 denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}

	btui32 GetNumTris(WCoord coords) {
		return triCells[coords.x][coords.y].Size();
	}
	btf32 GetTriHeight(WCoord coords, btui32 index, btf32 pos_x, btf32 pos_y) {
		btf32 u, v, w;
		EnvTri* tri = &tris[(triCells[coords.x][coords.y])[index]];
		EnvTriBary(m::Vector2(pos_x, pos_y), points[tri->a].pos, points[tri->b].pos, points[tri->c].pos, u, v, w);
		return points[tri->a].h * u + points[tri->b].h * v + points[tri->c].h * w;
	}
	bool GetTriExists(WCoord coords, btui32 index)
	{
		return (triCells[coords.x][coords.y])[index] != ID_NULL;
	}
	void* GetC2Tri(WCoord coords, btui32 index) {
		return (void*)&triCollides[(triCells[coords.x][coords.y])[index]];
	}
	EnvTri* GetTri(WCoord coords, btui32 index) {
		return &tris[(triCells[coords.x][coords.y])[index]];
	}

	void GetFloorsAndCeilings(CellSpace& csinf, btf32 in_height, EnvTriSurfaceSet* set) {
		// Reset values
		set->nearest_ceil_above = nullptr;
		set->nearest_flor_above = nullptr;
		set->nearest_ceil_below = nullptr;
		set->nearest_flor_below = nullptr;
		set->nearest_ceil_h_above = WORLD_HEIGHT_MAX;
		set->nearest_flor_h_above = WORLD_HEIGHT_MAX;
		set->nearest_ceil_h_below = WORLD_HEIGHT_MIN;
		set->nearest_flor_h_below = WORLD_HEIGHT_MIN;
		// Throwaway values
		btf32 u, v, w;
		btf32 height;
		// For each triangle cell we're touching
		for (int cell = 0; cell < eCELL_COUNT; ++cell) {
			// For each triangle in this cell
			for (int i = 0; i < triCells[csinf.c[cell].x][csinf.c[cell].y].Size(); ++i) {
				// If this triangle exists
				if ((triCells[csinf.c[cell].x][csinf.c[cell].y])[i] != ID_NULL) {
					EnvTri* tri = &tris[(triCells[csinf.c[cell].x][csinf.c[cell].y])[i]];
					EnvTriBary(
						m::Vector2((btf32)csinf.c[eCELL_I].x + csinf.offsetx, (btf32)csinf.c[eCELL_I].y + csinf.offsety),
						points[tri->a].pos, points[tri->b].pos, points[tri->c].pos, u, v, w);
					// if we're inside the triangle
					if (u <= 1.f && v <= 1.f && w <= 1.f && u >= 0.f && v >= 0.f && w >= 0.f) {
						// Record the height of the triangle's plane at our coordinates
						height = points[tri->a].h * u + points[tri->b].h * v + points[tri->c].h * w;
						// Apply height if it's below the input height and above any already recorded height
						if (height <= in_height && height > set->nearest_flor_h_below) {
							if (tri->facing_up) {
								set->nearest_flor_h_below = height;
								set->nearest_flor_below = tri;
							}
							else {
								set->nearest_ceil_h_below = height;
								set->nearest_ceil_below = tri;
							}
						}
						// Apply height if it's below above input height and below any already recorded height
						if (height > in_height && height < set->nearest_ceil_h_above) {
							if (tri->facing_up) {
								set->nearest_flor_h_above = height;
								set->nearest_flor_above = tri;
							}
							else {
								set->nearest_ceil_h_above = height;
								set->nearest_ceil_above = tri;
							}
						}
					}
				}
				else {
					printf("Tried to get height of nonexistent triangle.\n");
				}
			}
		}
	}

	btID GetTriAtPos(btf32 x, btf32 y)
	{
		WCoord coords;
		coords.x = (btui16)roundf(x);
		coords.y = (btui16)roundf(y);
		for (int i = 0; i < triCells[coords.x][coords.y].Size(); ++i) {
			btf32 u, v, w;
			EnvTri* tri = &tris[(triCells[coords.x][coords.y])[i]];
			EnvTriBary(m::Vector2(x, y), points[tri->a].pos, points[tri->b].pos, points[tri->c].pos, u, v, w);
			if (u <= 1.f && v <= 1.f && w <= 1.f && u >= 0.f && v >= 0.f && w >= 0.f)
				return (triCells[coords.x][coords.y])[i];
		}
		return ID_NULL;
	}

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

	void GetNearestSurfaceHeight(btf32& out_height, CellSpace& csinf, btf32 in_height)
	{
		out_height = 0.f;

		// BILINEAR2
		//out_height = m::Lerp(
		//	m::Lerp((btf32)eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
		//	(btf32)eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
		//		(csinf.offsetx + 0.5f)),
		//	m::Lerp((btf32)eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
		//	(btf32)eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
		//		(csinf.offsetx + 0.5f)),
		//	(csinf.offsety + 0.5f)) / TERRAIN_HEIGHT_DIVISION;

		btf32 u, v, w;
		btf32 height;
		EnvTriSurfaceSet surfset;
		GetFloorsAndCeilings(csinf, in_height, &surfset);

		// todo: need special treatment if there are no triangles on us at all
		// are we outside of any geometry
		if (surfset.nearest_ceil_h_below < surfset.nearest_flor_h_below) {
			out_height = surfset.nearest_flor_h_below;
		}
		// or inside
		else {
			out_height = surfset.nearest_flor_h_above;
		}
	}

	void GetSlope(btf32& out_slope_x, btf32& out_slope_y, CellSpace& csinf)
	{
		out_slope_x = 0.f;
		out_slope_y = 0.f;

		//*
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
		//*/
	}

	forceinline bool LineTraceUtil_CheckCollideEnv(int x, int y, btf32 height_a, btf32 height_b, btf32 lerp)
	{
		btf32 mix = m::Lerp(height_a, height_b, lerp);
		if (((btf32)env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION) > mix + 0.7f)
			return true;
		return Get(x, y, eflag::EF_IMPASSABLE);
	}
	forceinline bool LineTraceUtil_CompareA(int diff) {
		return diff < 0;
	}
	forceinline bool LineTraceUtil_CompareB(int diff) {
		return diff <= 0;
	}
	forceinline bool LineTraceUtil_Continue(
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

	void Tick()
	{
		// eventually, this function could contain any 'propagation' like fire
	}

	void DrawProps()
	{
		//for (btui32 i = 0u; i < wldNumTextures; ++i)
		//{
		//	DrawCompositeMesh(ID_NULL, wldMeshes[i], acv::GetT(wldTxtr[i]), SS_NORMAL, graphics::Matrix4x4());
		//}
		
		
		//graphics::SetFrontFaceInverse();
		//graphics::Matrix4x4 matr;
		//graphics::MatrixTransform(matr, 0.f, 0.f, 0.f);
		//DrawTerrainMesh(ID_NULL, wldMeshTerrain,
		//	acv::GetT(acv::t_gui_box), acv::GetT(acv::t_gui_hurt),
		//	acv::GetT(acv::t_terrain_sanddirt), acv::GetT(acv::t_terrain_sanddirt),
		//	matr);
		//graphics::SetFrontFace();

	}
	void DrawTerrain()
	{
		
		DrawMesh(ID_NULL, acv::GetM(acv::m_world), acv::GetT(acv::t_terrain_01), SS_NORMAL, graphics::Matrix4x4());
		//DrawMesh(ID_NULL, acv::GetM(acv::m_world_phys), acv::GetT(acv::t_terrain_01), SS_NORMAL, graphics::Matrix4x4());
		//DrawTerrainMesh(ID_NULL, wldMeshTerrain,
		//	acv::GetT(acv::t_terrain_01), acv::GetT(acv::t_terrain_02),
		//	acv::GetT(acv::t_terrain_03), acv::GetT(acv::t_terrain_04),
		//	acv::GetT(acv::t_terrain_05), acv::GetT(acv::t_terrain_06),
		//	acv::GetT(acv::t_terrain_07), acv::GetT(acv::t_terrain_08),
		//	graphics::Matrix4x4());
	}
	void DrawDebugGizmos(CellSpace* cs)
	{
		btID tri_id = GetTriAtPos(cs->c[eCELL_I].x + cs->offsetx, cs->c[eCELL_I].y + cs->offsety);
		if (tri_id != ID_NULL) {
			EnvTri* tri = &tris[GetTriAtPos(cs->c[eCELL_I].x + cs->offsetx, cs->c[eCELL_I].y + cs->offsety)];
			EnvVert* va = &points[tri->a];
			EnvVert* vb = &points[tri->b];
			EnvVert* vc = &points[tri->c];

			graphics::Matrix4x4 matr;
			// draw vertices
			graphics::MatrixTransform(matr, m::Vector3(va->pos.x, va->h, va->pos.y));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			graphics::MatrixTransform(matr, m::Vector3(vb->pos.x, vb->h, vb->pos.y));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			graphics::MatrixTransform(matr, m::Vector3(vc->pos.x, vc->h, vc->pos.y));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			// draw vertex offsets
			graphics::MatrixTransform(matr, m::Vector3(va->pos.x + va->nor.x * 0.5f, va->h, va->pos.y + va->nor.y * 0.5f));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_red), SS_NORMAL, matr);
			graphics::MatrixTransform(matr, m::Vector3(vb->pos.x + vb->nor.x * 0.5f, vb->h, vb->pos.y + vb->nor.y * 0.5f));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_red), SS_NORMAL, matr);
			graphics::MatrixTransform(matr, m::Vector3(vc->pos.x + vc->nor.x * 0.5f, vc->h, vc->pos.y + vc->nor.y * 0.5f));
			DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_red), SS_NORMAL, matr);
			// draw edges
			if (tri->open_edge_ab) {
				graphics::MatrixTransform(matr, m::Vector3((va->pos.x + vb->pos.x) / 2.f, (va->h + vb->h) / 2.f, (va->pos.y + vb->pos.y) / 2.f));
				DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			}
			if (tri->open_edge_bc) {
				graphics::MatrixTransform(matr, m::Vector3((vb->pos.x + vc->pos.x) / 2.f, (vb->h + vc->h) / 2.f, (vb->pos.y + vc->pos.y) / 2.f));
				DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			}
			if (tri->open_edge_ca) {
				graphics::MatrixTransform(matr, m::Vector3((vc->pos.x + va->pos.x) / 2.f, (vc->h + va->h) / 2.f, (vc->pos.y + va->pos.y) / 2.f));
				DrawMesh(ID_NULL, acv::GetM(acv::m_debug_bb), acv::GetT(acv::t_col_black), SS_NORMAL, matr);
			}
		}
	}
	void DrawTerrainDebug()
	{
		graphics::Matrix4x4 matr;
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			acv::GetT(acv::t_terrain_01), acv::GetT(acv::t_terrain_02),
			acv::GetT(acv::t_terrain_03), acv::GetT(acv::t_terrain_04),
			acv::GetT(acv::t_terrain_05), acv::GetT(acv::t_terrain_06),
			acv::GetT(acv::t_terrain_07), acv::GetT(acv::t_terrain_08),
			matr);
		graphics::MatrixTransform(matr, m::Vector3(0.f, 0.01f, 0.f));
		graphics::SetRenderWire();
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			acv::GetT(acv::t_col_black), acv::GetT(acv::t_col_black),
			acv::GetT(acv::t_col_black), acv::GetT(acv::t_col_black),
			acv::GetT(acv::t_col_black), acv::GetT(acv::t_col_black),
			acv::GetT(acv::t_col_black), acv::GetT(acv::t_col_black),
			matr);
		graphics::SetRenderSolid();
	}

	void SaveBin()
	{
		Clean();
		printf("Saving World\n");
		FILE *out = fopen(WORLD_FILENAME, "wb");
		if (out != NULL)
		{
			fseek(out, 0, SEEK_SET); // Seek the beginning of the file
			const size_t wrote = fwrite(&eCells, sizeof(EnvNode), 1, out);
			fclose(out);
		}
	}

	void LoadBin()
	{
		printf("Loading World\n");
		FILE *in = fopen(WORLD_FILENAME, "rb");
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
				eCells.terrain_height[x][y] = m::Max<btui8>(4,
					eCells.terrain_height_ne[x][y], eCells.terrain_height_nw[x][y],
					eCells.terrain_height_se[x][y], eCells.terrain_height_sw[x][y]);
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
							wldMeshes[i].AddMesh(&acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
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
								wldMeshes[i].AddMesh(&acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
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

	btf32 Max2(btf32 a, btf32 b, btf32 c)
	{
		btf32 max = a;
		if (b > max) max = b;
		if (c > max) max = c;
		return max;
	}
	btf32 Min2(btf32 a, btf32 b, btf32 c)
	{
		btf32 min = a;
		if (b < min) min = b;
		if (c < min) min = c;
		return min;
	}

	// Check if vertices are identical
	bool VertCompare(btui32 a, btui32 b) {
		return a == b;
		// backup plan
		/*
		return env::points[a].pos.x == env::points[b].pos.x
			&& env::points[a].pos.y == env::points[b].pos.y
			&& env::points[a].h == env::points[b].h;
		//*/
	}
	// Record if point A is directly above point B
	void VertCompareLedgeDetect(btf32* out, btui32 a, btui32 b) {
		if (env::points[a].pos.x == env::points[b].pos.x // If aligned with origin x
			&& env::points[a].pos.y == env::points[b].pos.y // If aligned with origin y
			&& env::points[a].h > env::points[b].h // If under the origin point
			&& *out < env::points[b].h) // If above the previously recorded below
			*out = env::points[b].h;
	}
	void GenerateTerrainMesh()
	{
		graphics::Mesh* mesh = &acv::GetM(acv::m_world_phys);

		pointcount = mesh->VcesSize();
		points = new EnvVert[pointcount];

		for (int i = 0; i < pointcount; ++i) {
			points[i].pos.x = mesh->Vces()[i].pos.x;
			points[i].pos.y = mesh->Vces()[i].pos.z;
			points[i].h = mesh->Vces()[i].pos.y;
		}

		tricount = mesh->IcesSize() / 3;

		if (pointcount == tricount * 3u) {
			printf("----------------------------------------------------------------\nWARNING: Collision mesh has three vertices per triangle - none are shared!\n----------------------------------------------------------------\n");
		}

		tris = new EnvTri[tricount];
		triCollides = new c2Poly[tricount];
		
		// Learn triangles from the mesh
		for (int i = 0; i < tricount; ++i) {
			// our own tris
			EnvTri* tri = &tris[i];
			tri->a = mesh->Ices()[i * 3];
			tri->b = mesh->Ices()[i * 3 + 1];
			tri->c = mesh->Ices()[i * 3 + 2];

			// Check if it's clockwise
			if ((points[tri->b].pos.y - points[tri->a].pos.y) * (points[tri->c].pos.x - points[tri->b].pos.x)
				- (points[tri->c].pos.y - points[tri->b].pos.y) * (points[tri->b].pos.x - points[tri->a].pos.x) < 0.f)
				tri->facing_up = true;
			else tri->facing_up = false;

			// c2 collision tris
			c2Poly* poly = &triCollides[i];
			poly->count = 3;
			/*poly->verts[0].x = mesh->Vces()[i * 3].pos.x;
			poly->verts[0].y = mesh->Vces()[i * 3].pos.z;
			poly->verts[1].x = mesh->Vces()[i * 3 + 1].pos.x;
			poly->verts[1].y = mesh->Vces()[i * 3 + 1].pos.z;
			poly->verts[2].x = mesh->Vces()[i * 3 + 2].pos.x;
			poly->verts[2].y = mesh->Vces()[i * 3 + 2].pos.z;*/
			poly->verts[0].x = points[tri->a].pos.x;
			poly->verts[0].y = points[tri->a].pos.y;
			poly->verts[1].x = points[tri->b].pos.x;
			poly->verts[1].y = points[tri->b].pos.y;
			poly->verts[2].x = points[tri->c].pos.x;
			poly->verts[2].y = points[tri->c].pos.y;
			c2MakePoly(poly);
		}

		// precompute triangle cells
		// for all triangles
		for (int i = 0; i < tricount; ++i) {
			btui32 bound_min_x = (btui32)floorf(Min2(points[tris[i].a].pos.x, points[tris[i].b].pos.x, points[tris[i].c].pos.x));
			btui32 bound_min_y = (btui32)floorf(Min2(points[tris[i].a].pos.y, points[tris[i].b].pos.y, points[tris[i].c].pos.y));
			btui32 bound_max_x = (btui32)ceilf(Max2(points[tris[i].a].pos.x, points[tris[i].b].pos.x, points[tris[i].c].pos.x));
			btui32 bound_max_y = (btui32)ceilf(Max2(points[tris[i].a].pos.y, points[tris[i].b].pos.y, points[tris[i].c].pos.y));
			// make sure the bounds are within the world
			if (bound_min_x < WORLD_SIZE && bound_min_y < WORLD_SIZE
				&& bound_max_x < WORLD_SIZE && bound_max_y < WORLD_SIZE) {
				// for every cell within the bounds
				for (int x = bound_min_x; x <= bound_max_x; ++x) {
					for (int y = bound_min_y; y <= bound_max_y; ++y) {
						c2AABB box;
						box.min.x = x - 0.5f; box.min.y = y - 0.5f;
						box.max.x = x + 0.5f; box.max.y = y + 0.5f;
						c2x transform = c2xIdentity();
						if (c2AABBtoPoly(box, &triCollides[i], &transform) == 1)
							triCells[x][y].Add(i);
					}
				}
			}
		}

		// determine vertex neighbors (will make triangle neighbors obsolete i guess)
		//*
		for (int i = 0; i < pointcount; ++i) {
			// Triangle to find neighbors of
			EnvVert* vert = &points[i];
			vert->neighborcount = 0u;
			// Now for all other triangles
			for (int j = 0; j < tricount; ++j) {
				EnvTri* tri_compare = &tris[j];
				// Do we share any vertices?
				if (VertCompare(tri_compare->a, i)) {
					// check for vertices allready added to neighbor list
					bool ne_already_added[2]{ false, false };
					for (int k = 0; k < vert->neighborcount; ++k) {
						if (vert->neighbors[k] == tri_compare->b) ne_already_added[0] = true;
						if (vert->neighbors[k] == tri_compare->c) ne_already_added[1] = true;
					}
					// add neighbors
					if (!ne_already_added[0]) {
						vert->neighbors[vert->neighborcount] = tri_compare->b;
						++vert->neighborcount;
					}
					if (!ne_already_added[1]) {
						vert->neighbors[vert->neighborcount] = tri_compare->c;
						++vert->neighborcount;
					}
				}
				if (VertCompare(tri_compare->b, i)) {
					// check for vertices allready added to neighbor list
					bool ne_already_added[2]{ false, false };
					for (int k = 0; k < vert->neighborcount; ++k) {
						if (vert->neighbors[k] == tri_compare->a) ne_already_added[0] = true;
						if (vert->neighbors[k] == tri_compare->c) ne_already_added[1] = true;
					}
					// add neighbors
					if (!ne_already_added[0]) {
						vert->neighbors[vert->neighborcount] = tri_compare->a;
						++vert->neighborcount;
					}
					if (!ne_already_added[1]) {
						vert->neighbors[vert->neighborcount] = tri_compare->c;
						++vert->neighborcount;
					}
				}
				if (VertCompare(tri_compare->c, i)) {
					// check for vertices allready added to neighbor list
					bool ne_already_added[2]{ false, false };
					for (int k = 0; k < vert->neighborcount; ++k) {
						if (vert->neighbors[k] == tri_compare->a) ne_already_added[0] = true;
						if (vert->neighbors[k] == tri_compare->b) ne_already_added[1] = true;
					}
					// add neighbors
					if (!ne_already_added[0]) {
						vert->neighbors[vert->neighborcount] = tri_compare->a;
						++vert->neighborcount;
					}
					if (!ne_already_added[1]) {
						vert->neighbors[vert->neighborcount] = tri_compare->b;
						++vert->neighborcount;
					}
				}
			}
		}//*/

		#define DEF_TRI_NE_EXPERIMENTAL 0

		btf32* point_nearest_h_below = new btf32[pointcount];
		for (int i = 0; i < pointcount; ++i)
			point_nearest_h_below[i] = WORLD_HEIGHT_MIN;

		// Determine triangle neighbors
		// I'm sorry about how slow this is
		// TODO: do this after computing cells so we don't have to iterate so much
		for (int i = 0; i < tricount; ++i) {
			// Triangle to find neighbors of
			EnvTri* tri = &tris[i];
			tri->neighborcount = 0u;
			#if DEF_TRI_NE_EXPERIMENTAL == 1
			btui32 bound_min_x = (btui32)floorf(Min2(points[tri->a].pos.x, points[tri->b].pos.x, points[tri->c].pos.x));
			btui32 bound_min_y = (btui32)floorf(Min2(points[tri->a].pos.y, points[tri->b].pos.y, points[tri->c].pos.y));
			btui32 bound_max_x = (btui32)ceilf( Max2(points[tri->a].pos.x, points[tri->b].pos.x, points[tri->c].pos.x));
			btui32 bound_max_y = (btui32)ceilf( Max2(points[tri->a].pos.y, points[tri->b].pos.y, points[tri->c].pos.y));
			// make sure the bounds are within the world
			if (bound_min_x < WORLD_SIZE && bound_min_y < WORLD_SIZE
				&& bound_max_x < WORLD_SIZE && bound_max_y < WORLD_SIZE) {
				// for every cell within the bounds
				for (int x = bound_min_x; x <= bound_max_x; ++x) {
					for (int y = bound_min_y; y <= bound_max_y; ++y) {
						// for each triangle in the cell
						for (int j = 0; j < triCells[x][y].Size(); ++j) {
							// make sure it's not the same triangle
							if (i != (triCells[x][y])[j]) {
								int samecount = 0;
								EnvTri* tri_compare = &tris[(triCells[x][y])[j]];
								// Do we share any vertices? (there should be a possibility of checking directly like this, but no verts are shared)
								// This should be fixed somehow later so i'll leave this extra code here for now
								//if (tri_compare->a == tri->a) ++samecount;
								//if (tri_compare->a == tri->b) ++samecount;
								//if (tri_compare->a == tri->c) ++samecount;
								//if (tri_compare->b == tri->a) ++samecount;
								//if (tri_compare->b == tri->b) ++samecount;
								//if (tri_compare->b == tri->c) ++samecount;
								//if (tri_compare->c == tri->a) ++samecount;
								//if (tri_compare->c == tri->b) ++samecount;
								//if (tri_compare->c == tri->c) ++samecount;
								if (VertCompare(tri_compare->a, tri->a)) ++samecount;
								if (VertCompare(tri_compare->a, tri->b)) ++samecount;
								if (VertCompare(tri_compare->a, tri->c)) ++samecount;
								if (VertCompare(tri_compare->b, tri->a)) ++samecount;
								if (VertCompare(tri_compare->b, tri->b)) ++samecount;
								if (VertCompare(tri_compare->b, tri->c)) ++samecount;
								if (VertCompare(tri_compare->c, tri->a)) ++samecount;
								if (VertCompare(tri_compare->c, tri->b)) ++samecount;
								if (VertCompare(tri_compare->c, tri->c)) ++samecount;
								// If we share more than two points
								// It means that it's a neighbor tri
								if (samecount > 1) {
									// Set neighbor
									tri->neighbors[tri->neighborcount] = j;
									++tri->neighborcount;
								}
							}
						}
					}
				}
			}
			#else
			// Now for all other triangles (or early exit if we've already found 3 neighbors)
			for (int j = 0; j < tricount; ++j) {
				// make sure it's not the same triangle
				if (i != j) {
					int samecount = 0;
					EnvTri* tri_compare = &tris[j];
					// Do we share any vertices?
					if (VertCompare(tri_compare->a, tri->a)) ++samecount;
					else if (VertCompare(tri_compare->a, tri->b)) ++samecount;
					else if (VertCompare(tri_compare->a, tri->c)) ++samecount;
					if (VertCompare(tri_compare->b, tri->a)) ++samecount;
					else if (VertCompare(tri_compare->b, tri->b)) ++samecount;
					else if (VertCompare(tri_compare->b, tri->c)) ++samecount;
					if (VertCompare(tri_compare->c, tri->a)) ++samecount;
					else if (VertCompare(tri_compare->c, tri->b)) ++samecount;
					else if (VertCompare(tri_compare->c, tri->c)) ++samecount;
					// If we share more than two points
					// It means that it's a neighbor tri
					if (samecount > 1) {
						// Set neighbor
						tri->neighbors[tri->neighborcount] = j;
						++tri->neighborcount;
					}
					// Find ledges
					// TODO: only need to do this once per other VERTEX, not per triangle
					VertCompareLedgeDetect(&point_nearest_h_below[tri->a], tri->a, tri_compare->a);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->a], tri->a, tri_compare->b);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->a], tri->a, tri_compare->c);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->b], tri->b, tri_compare->a);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->b], tri->b, tri_compare->b);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->b], tri->b, tri_compare->c);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->c], tri->c, tri_compare->a);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->c], tri->c, tri_compare->b);
					VertCompareLedgeDetect(&point_nearest_h_below[tri->c], tri->c, tri_compare->c);
				}
			}
			#endif
		}

		// todo: temp as fuck
		lines = (EnvLineSeg*)malloc(256 * sizeof(EnvLineSeg));

		// Determine triangle edges with no neighbors
		for (int i = 0; i < pointcount; ++i) {
			points[i].nor.x = 0.f;
			points[i].nor.y = 0.f;
		}
		for (int i = 0; i < tricount; ++i) {
			EnvTri* tri = &tris[i];
			btui32 points_connected[3]{ 0u, 0u, 0u };
			tri->open_edge_ab = false;
			tri->open_edge_bc = false;
			tri->open_edge_ca = false;
			// find any edges that don't have neighbors attached
			for (int j = 0; j < tri->neighborcount; ++j) {
				EnvTri* tri_compare = &tris[tri->neighbors[j]];
				// Do we share any vertices?
				if (VertCompare(tri->a, tri_compare->a)) ++points_connected[0];
				else if (VertCompare(tri->a, tri_compare->b)) ++points_connected[0];
				else if (VertCompare(tri->a, tri_compare->c)) ++points_connected[0];
				if (VertCompare(tri->b, tri_compare->a)) ++points_connected[1];
				else if (VertCompare(tri->b, tri_compare->b)) ++points_connected[1];
				else if (VertCompare(tri->b, tri_compare->c)) ++points_connected[1];
				if (VertCompare(tri->c, tri_compare->a)) ++points_connected[2];
				else if (VertCompare(tri->c, tri_compare->b)) ++points_connected[2];
				else if (VertCompare(tri->c, tri_compare->c)) ++points_connected[2];
			}
			// we can figure out which edges aren't connected to other triangles based on
			// the number of connections each vertex has (maximum of 2)
			tri->open_edge_ab = points_connected[0] < points_connected[2] || points_connected[1] < points_connected[2];
			tri->open_edge_bc = points_connected[1] < points_connected[0] || points_connected[2] < points_connected[0];
			tri->open_edge_ca = points_connected[2] < points_connected[1] || points_connected[0] < points_connected[1];
			// test (i think we can just assume a certain winding direction for these
			m::Vector2 normal_ab = m::Normalize(points[tri->a].pos - points[tri->b].pos);
			normal_ab = m::Vector2(normal_ab.y, -normal_ab.x);
			m::Vector2 normal_bc = m::Normalize(points[tri->b].pos - points[tri->c].pos);
			normal_bc = m::Vector2(normal_bc.y, -normal_bc.x);
			m::Vector2 normal_ca = m::Normalize(points[tri->c].pos - points[tri->a].pos);
			normal_ca = m::Vector2(normal_ca.y, -normal_ca.x);
			
			// now make collision lines here
			// and make nav offsets
			//m::Vector2 v2 = lineLineIntersection(points[tri->a].pos, points[tri->b].pos, points[tri->a].pos, points[tri->c].pos);
			if (tri->open_edge_ab) {
				// if there was any ledge
				if (point_nearest_h_below[tri->a] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->b] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->a].pos;
					lines[linecount].h_a_top = points[tri->a].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->a];
					lines[linecount].pos_b = points[tri->b].pos;
					lines[linecount].h_b_top = points[tri->b].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->b];
					++linecount;
				}
				//temporary solution
				points[tri->a].nor += normal_ab;
				points[tri->b].nor += normal_ab;
			}
			if (tri->open_edge_bc) {
				// if there was any ledge
				if (point_nearest_h_below[tri->b] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->c] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->b].pos;
					lines[linecount].h_a_top = points[tri->b].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->b];
					lines[linecount].pos_b = points[tri->c].pos;
					lines[linecount].h_b_top = points[tri->c].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->c];
					++linecount;
				}
				//temporary solution
				points[tri->b].nor += normal_bc;
				points[tri->c].nor += normal_bc;
			}
			if (tri->open_edge_ca) {
				// if there was any ledge
				if (point_nearest_h_below[tri->c] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->a] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->c].pos;
					lines[linecount].h_a_top = points[tri->c].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->c];
					lines[linecount].pos_b = points[tri->a].pos;
					lines[linecount].h_b_top = points[tri->a].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->a];
					++linecount;
				}
				//temporary solution
				points[tri->c].nor += normal_ca;
				points[tri->a].nor += normal_ca;
			}
		}

		delete[] point_nearest_h_below;

		wldMeshTerrain.GenerateComplexEnv(eCells.terrain_height, eCells.terrain_material,
			(btui32*)&eCells.flags, eflag::EF_INVISIBLE,
			eCells.terrain_height_ne, eCells.terrain_height_nw,
			eCells.terrain_height_se, eCells.terrain_height_sw);
		//wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);

	}
	void GenerateTerrainMeshEditor()
	{
		wldMeshTerrain.GenerateComplexEnv(eCells.terrain_height, eCells.terrain_material,
			(btui32*)&eCells.flags, eflag::EF_INVISIBLE,
			eCells.terrain_height_ne, eCells.terrain_height_nw,
			eCells.terrain_height_se, eCells.terrain_height_sw);
		//wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
	}
	void Free()
	{
		delete[] points;
		delete[] tris;
		delete[] triCollides;
		delete[] lines;
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

#define INT_NULL 0b11111111111111111111111111111111

struct pathNodeTest {
	btui32 point;
	//btID owner_tri;
};
bool path::PathFind(Path* path, btf32 x, btf32 y, btf32 xDest, btf32 yDest)
{
	btui32* node_from_cache = new btui32[env::pointcount];
	for (int i = 0; i < env::pointcount; ++i) {
		node_from_cache[i] = INT_NULL;
	}

	//todo calculate these
	btID tri_start = env::GetTriAtPos(x, y);

	// TODO: compare point distances
	//btf32 fa = m::Length(m::Vector2(x - env::points[env::tris[tri_start].a].pos.x, y - env::points[env::tris[tri_start].a].pos.y));
	//btf32 fb = m::Length(m::Vector2(x - env::points[env::tris[tri_start].a].pos.x, y - env::points[env::tris[tri_start].a].pos.y));
	//btf32 fc = m::Length(m::Vector2(x - env::points[env::tris[tri_start].a].pos.x, y - env::points[env::tris[tri_start].a].pos.y));

	btID tri_dest = env::GetTriAtPos(xDest, yDest);

	pathNodeTest node_start;
	//node_start.owner_tri = tri_start;
	node_start.point = env::tris[tri_start].a; // todo: actually find the closest point, but for now just use the first
	pathNodeTest node_dest;
	//node_dest.owner_tri = tri_dest;
	node_dest.point = env::tris[tri_dest].a; // todo: actually find the closest point, but for now just use the first

	// set intial node to point to itself, just in case
	node_from_cache[node_start.point] = node_start.point;

	std::queue<pathNodeTest> openSet;
	openSet.push(node_start);
	// while there are still nodes to analyse
	while(!openSet.empty()) {
		pathNodeTest current = openSet.front();
		openSet.pop();
		// if we've reached the destination point
		if (current.point == node_dest.point)
			break;

		// Get the point we're examining for neighbors
		env::EnvVert* point_this = &env::points[current.point];

		// for each neighbor of this point
		for (int i = 0; i < point_this->neighborcount; ++i) {
			// make sure the neighboring point hasn't been checked already (INT_NULL)
			if (node_from_cache[point_this->neighbors[i]] == INT_NULL) {
				pathNodeTest node;
				node.point = point_this->neighbors[i];
				// Add this to the queue
				openSet.push(node);
				// set where we came from
				node_from_cache[point_this->neighbors[i]] = current.point;
			}
		}
	}

	//temp
	for (int i = 0; i < PATH_NUM_NODES; ++i) {
		path->pos_x[i] = 0.f;
		path->pos_y[i] = 0.f;
	}
	path->len = 0u; // reset the path


	// construct path vector by tracing backwards from the destination point
	// Add destination node
	path->pos_x[path->len] = xDest;
	path->pos_y[path->len] = yDest;
	++path->len;
	btui32 vert_examine = node_dest.point;
	while (vert_examine != node_start.point) {
		if (path->len < PATH_NUM_NODES) {
			if (node_from_cache[vert_examine] == INT_NULL) {
				printf("Path led us to node that hasn't been examined!\n");
				return false;
			}
			printf("Path added node %i\n", vert_examine);
			// Set this path node
			path->pos_x[path->len] = env::points[vert_examine].pos.x + env::points[vert_examine].nor.x * 0.5f; // todo: replace 0.5 with agent size
			path->pos_y[path->len] = env::points[vert_examine].pos.y + env::points[vert_examine].nor.y * 0.5f;
			++path->len;
			// backtrace previous point from this point
			vert_examine = node_from_cache[vert_examine];
		}
		else return false; // return if we maxed out the path
	}
	// Original position
	//path->pos_x[path->len] = x;
	//path->pos_y[path->len] = y;
	//++path->len;

	delete[] node_from_cache;

	return true;
}
