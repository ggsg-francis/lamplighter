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

	struct LRVec2 {
		btf32 x, y;
		LRVec2(btf32 X = 0.f, btf32 Y = 0.f) : x{ X }, y{ Y } {};
	};
	LRVec2 operator-(const LRVec2& a, const LRVec2& b) { return LRVec2(a.x - b.x, a.y - b.y); }
	btf32 Dot2(const LRVec2& a, const LRVec2& b) {
		return a.x * b.x + a.y * b.y;
	}

	struct EnvVert {
		LRVec2 pos;
		btf32 h;
	};

	struct EnvTri {
		EnvVert a, b, c;
	};

	mem::idbuf triCells[WORLD_SIZE][WORLD_SIZE];

	EnvTri* tris = nullptr;
	c2Poly* triCollides = nullptr;
	int tricount = 0;

	void EnvTriBary(LRVec2 p, LRVec2 a, LRVec2 b, LRVec2 c, btf32 &u, btf32 &v, btf32 &w)
	{
		LRVec2 v0 = b - a, v1 = c - a, v2 = p - a;
		btf32 d00 = Dot2(v0, v0);
		btf32 d01 = Dot2(v0, v1);
		btf32 d11 = Dot2(v1, v1);
		btf32 d20 = Dot2(v2, v0);
		btf32 d21 = Dot2(v2, v1);
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
		EnvTriBary(LRVec2(pos_x, pos_y), tri->a.pos, tri->b.pos, tri->c.pos, u, v, w);
		return tri->a.h * u + tri->b.h * v + tri->c.h * w;
	}
	bool GetTriExists(WCoord coords, btui32 index)
	{
		return (triCells[coords.x][coords.y])[index] != ID_NULL;
	}
	void* GetC2Tri(WCoord coords, btui32 index) {
		return (void*)&triCollides[(triCells[coords.x][coords.y])[index]];
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

	void GetHeight(btf32& out_height, CellSpace& csinf)
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

		for (int i = 0; i < triCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].Size(); ++i) {
			if ((triCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y])[i] != ID_NULL) {
				EnvTri* tri = &tris[(triCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y])[i]];
				EnvTriBary(
					LRVec2((btf32)csinf.c[eCELL_I].x + csinf.offsetx, (btf32)csinf.c[eCELL_I].y + csinf.offsety),
					tri->a.pos, tri->b.pos, tri->c.pos, u, v, w);
				// if we're inside the triangle
				if (u <= 1.f && v <= 1.f && w <= 1.f && u >= 0.f && v >= 0.f && w >= 0.f)
					out_height = tri->a.h * u + tri->b.h * v + tri->c.h * w;
			}
			else {
				printf("Tried to get height of nonexistent triangle.\n");
			}
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
		for (btui32 i = 0u; i < wldNumTextures; ++i)
		{
			DrawCompositeMesh(ID_NULL, wldMeshes[i], acv::GetT(wldTxtr[i]), SS_NORMAL, graphics::Matrix4x4());
		}
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

	void GenerateTerrainMesh()
	{
		graphics::Mesh* mesh = &acv::GetM(acv::m_world_phys);

		tricount = mesh->IcesSize() / 3;

		tris = new EnvTri[tricount];
		triCollides = new c2Poly[tricount];
		
		// Learn triangles from the mesh
		for (int i = 0; i < tricount; ++i) {
			// our own tris
			EnvTri* tri = &tris[i];
			tri->a.pos.x = mesh->Vces()[i * 3].pos.x;
			tri->a.pos.y = mesh->Vces()[i * 3].pos.z;
			tri->a.h = mesh->Vces()[i * 3].pos.y;
			tri->b.pos.x = mesh->Vces()[i * 3 + 1].pos.x;
			tri->b.pos.y = mesh->Vces()[i * 3 + 1].pos.z;
			tri->b.h = mesh->Vces()[i * 3 + 1].pos.y;
			tri->c.pos.x = mesh->Vces()[i * 3 + 2].pos.x;
			tri->c.pos.y = mesh->Vces()[i * 3 + 2].pos.z;
			tri->c.h = mesh->Vces()[i * 3 + 2].pos.y;
			// c2 collision tris
			c2Poly* poly = &triCollides[i];
			poly->count = 3;
			poly->verts[0].x = mesh->Vces()[i * 3].pos.x;
			poly->verts[0].y = mesh->Vces()[i * 3].pos.z;
			poly->verts[1].x = mesh->Vces()[i * 3 + 1].pos.x;
			poly->verts[1].y = mesh->Vces()[i * 3 + 1].pos.z;
			poly->verts[2].x = mesh->Vces()[i * 3 + 2].pos.x;
			poly->verts[2].y = mesh->Vces()[i * 3 + 2].pos.z;
			c2MakePoly(poly);
		}

		// precompute triangle cells
		// fast
		//*
		// for all triangles
		for (int i = 0; i < tricount; ++i) {
			btui32 bound_min_x = (btui32)floorf(Min2(tris[i].a.pos.x, tris[i].b.pos.x, tris[i].c.pos.x));
			btui32 bound_min_y = (btui32)floorf(Min2(tris[i].a.pos.y, tris[i].b.pos.y, tris[i].c.pos.y));
			btui32 bound_max_x = (btui32)ceilf(Max2(tris[i].a.pos.x, tris[i].b.pos.x, tris[i].c.pos.x));
			btui32 bound_max_y = (btui32)ceilf(Max2(tris[i].a.pos.y, tris[i].b.pos.y, tris[i].c.pos.y));
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
		//*/
		// reliable
		/*
		int temp = 32;
		for (int x = 1024 - temp; x < 1024 + temp; ++x) {
			for (int y = 1024 - temp; y < 1024 + temp; ++y) {
				c2AABB box;
				box.min.x = x - 0.5f;
				box.min.y = y - 0.5f;
				box.max.x = x + 0.5f;
				box.max.y = y + 0.5f;
				c2x transform = c2xIdentity();
				c2Manifold mani;
				// check against all triangles
				for (int i = 0; i < tricount; ++i) {
					//c2AABBtoPolyManifold(box, &triCollides[0], &transform, &mani);
					if (c2AABBtoPoly(box, &triCollides[i], &transform) == 1) {
						triCells[x][y].add(i);
					}
				}
			}
		}
		//*/

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
		delete[] tris;
		delete[] triCollides;
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

bool path::PathFind(Path* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest)
{
	std::queue<WCoord> openSet;
	WCoord node; node.x = x; node.y = y;
	openSet.push(node);
	std::vector<WCoord> usedSet;
	usedSet.push_back(node);
	while(!openSet.empty())
	{
		WCoord current = openSet.front();
		openSet.pop();
		if (current.x == xDest && current.y == yDest)
			break;
		// N
		if (current.y < WORLD_SIZE_MAXINT && node_cache[current.x][current.y + 1] == ND_UNSET)
		{
			// if next tile is not too high compared to this one
			if (env::eCells.terrain_height[current.x][current.y + 1] < env::eCells.terrain_height[current.x][current.y] + 4u)
			{
				WCoord next; next.x = current.x; next.y = current.y + 1;
				openSet.push(next);
				node_cache[current.x][current.y + 1] = ND_FROM_SOUTH;
				usedSet.push_back(next);
			}
		}
		// S
		if (current.y > 0 && node_cache[current.x][current.y - 1] == ND_UNSET)
		{
			// if next tile is not too high compared to this one
			if (env::eCells.terrain_height[current.x][current.y - 1] < env::eCells.terrain_height[current.x][current.y] + 4u)
			{
				WCoord next; next.x = current.x; next.y = current.y - 1;
				openSet.push(next);
				node_cache[current.x][current.y - 1] = ND_FROM_NORTH;
				usedSet.push_back(next);
			}
		}
		// E
		if (current.x < WORLD_SIZE_MAXINT && node_cache[current.x + 1][current.y] == ND_UNSET)
		{
			// if next tile is not too high compared to this one
			if (env::eCells.terrain_height[current.x + 1][current.y] < env::eCells.terrain_height[current.x][current.y] + 4u)
			{
				WCoord next; next.x = current.x + 1; next.y = current.y;
				openSet.push(next);
				node_cache[current.x + 1][current.y] = ND_FROM_WEST;
				usedSet.push_back(next);
			}
		}
		// W
		if (current.x > 0 && node_cache[current.x - 1][current.y] == ND_UNSET)
		{
			// if next tile is not too high compared to this one
			if (env::eCells.terrain_height[current.x - 1][current.y] < env::eCells.terrain_height[current.x][current.y] + 4u)
			{
				WCoord next; next.x = current.x - 1; next.y = current.y;
				openSet.push(next);
				node_cache[current.x - 1][current.y] = ND_FROM_EAST;
				usedSet.push_back(next);
			}
		}
	}

	path->len = 0u; // reset the path

	// construct path vector
	WCoord current; current.x = xDest; current.y = yDest;
	while (current.x != x || current.y != y)
	{
		if (path->len < PATH_NUM_NODES)
		{
			path->nodes[path->len] = current;
			++path->len;
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
		else return false; // return if we maxed out the path
	}
	path->nodes[path->len] = current;
	++path->len;

	// clear modified cache
	for (int i = 0; i < usedSet.size(); ++i)
		node_cache[usedSet[i].x][usedSet[i].y] = ND_UNSET;

	return true;
}
