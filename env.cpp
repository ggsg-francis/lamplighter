#include "env.h"
#include "global.h"

#include <iostream>
//#include <vector>
#include "memory.hpp"
#include "maths.hpp"
#include "graphics.hpp"
#include "archive.hpp"

#include <queue>
#include <vector>

#if !DEF_GRID
#define CUTE_C2_IMPLEMENTATION
#include "3rdparty\cute_c2.h"
#endif

#define NUM_COMPOSITES 32u

#if !DEF_GRID
#define WORLD_HEIGHT_MAX INFINITY
#define WORLD_HEIGHT_MIN -INFINITY
#endif

namespace env
{
	#if DEF_GRID
	EnvNode eCells;

	graphics::CompositeMesh wldMeshes[NUM_COMPOSITES];
	graphics::TerrainMesh wldMeshTerrain;
	lid wldTxtr[NUM_COMPOSITES];
	lui32 wldNumTextures = 0u;
	#endif

	#if !DEF_GRID

	struct EnvMesh {
		// Points (for navigation)
		EnvVert* points = nullptr;
		lui32 pointcount = 0u;
		// Tris (for physics, and partially navigation)
		EnvTri* tris = nullptr;
		EnvTriMeta* trimeta = nullptr;
		c2Poly* triCollides = nullptr;
		lui32 tricount = 0u;
		// Lines, represent walls in the collision
		EnvLineSeg* lines = nullptr;
		lui32 linecount = 0u;

		// Check if vertices are identical
		bool VertCompare(lui32 a, lui32 b) {
			return a == b;
			// backup plan
			/*
			return env::points[a].pos.x == env::points[b].pos.x
			&& env::points[a].pos.y == env::points[b].pos.y
			&& env::points[a].h == env::points[b].h;
			//*/
		}
		// Record if point A is directly above point B
		void VertCompareLedgeDetect(lf32* out, lui32 a, lui32 b) {
			//if (env::points[a].pos.x == env::points[b].pos.x // If aligned with origin x
			//	&& env::points[a].pos.y == env::points[b].pos.y // If aligned with origin y
			//	&& env::points[a].h > env::points[b].h // If under the origin point
			//	&& *out < env::points[b].h) // If above the previously recorded below
			//	*out = env::points[b].h;
			if (m::Length(points[a].pos - points[b].pos) < 0.001f // If aligned with origin (or close enough)
				&& points[a].h > points[b].h // If under the origin point
				&& *out < points[b].h) // If above the previously recorded below
				*out = points[b].h;
		}
		void Generate(mem::idbuf* tcel, mem::idbuf* lcel) {
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
			trimeta = new EnvTriMeta[tricount];
			triCollides = new c2Poly[tricount];

			// Learn triangles from the mesh
			for (int i = 0; i < tricount; ++i) {
				// our own tris
				EnvTri* tri = &tris[i];
				tri->verts[0] = mesh->Ices()[i * 3];
				tri->verts[1] = mesh->Ices()[i * 3 + 1];
				tri->verts[2] = mesh->Ices()[i * 3 + 2];

				m::Vector3 tri_a = m::Vector3(points[tri->verts[0]].pos.x, points[tri->verts[0]].h, points[tri->verts[0]].pos.y);
				m::Vector3 tri_b = m::Vector3(points[tri->verts[1]].pos.x, points[tri->verts[1]].h, points[tri->verts[1]].pos.y);
				m::Vector3 tri_c = m::Vector3(points[tri->verts[2]].pos.x, points[tri->verts[2]].h, points[tri->verts[2]].pos.y);
				// get the edges of this triangle
				m::Vector3 dir_ab = m::Normalize(tri_a - tri_b);
				m::Vector3 dir_ac = m::Normalize(tri_a - tri_c);
				// get the normal of this triangle
				m::Vector3 dir_nor = m::Cross(dir_ab, dir_ac);
				dir_nor = m::Normalize(dir_nor); // test

				if (fabsf(dir_nor.y) <= 0.000001f) {
					tri->vertical = true;
					tri->facing_up = false; // just for the sake of it
				}
				else {
					tri->vertical = false;
					tri->slope.x = dir_nor.x;
					tri->slope.y = dir_nor.z;

					// Check if it's clockwise
					if ((points[tri->verts[1]].pos.y - points[tri->verts[0]].pos.y) * (points[tri->verts[2]].pos.x - points[tri->verts[1]].pos.x)
						- (points[tri->verts[2]].pos.y - points[tri->verts[1]].pos.y) * (points[tri->verts[1]].pos.x - points[tri->verts[0]].pos.x) < 0.f)
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
					poly->verts[0].x = points[tri->verts[0]].pos.x;
					poly->verts[0].y = points[tri->verts[0]].pos.y;
					poly->verts[1].x = points[tri->verts[1]].pos.x;
					poly->verts[1].y = points[tri->verts[1]].pos.y;
					poly->verts[2].x = points[tri->verts[2]].pos.x;
					poly->verts[2].y = points[tri->verts[2]].pos.y;
					c2MakePoly(poly);
				}
			}

			// todo: temp as fuck
			lines = (EnvLineSeg*)malloc(256 * sizeof(EnvLineSeg));

			// precompute triangle cells
			// for all triangles
			for (int i = 0; i < tricount; ++i) {
				// If this triangle represents a wall, create a line segment for it
				if (tris[i].vertical) {
					EnvTri* tri = &tris[i];

					bool same_ab = points[tri->verts[0]].pos.x == points[tri->verts[1]].pos.x
						&& points[tri->verts[0]].pos.y == points[tri->verts[1]].pos.y;
					bool same_bc = points[tri->verts[1]].pos.x == points[tri->verts[2]].pos.x
						&& points[tri->verts[1]].pos.y == points[tri->verts[2]].pos.y;
					bool same_ca = points[tri->verts[2]].pos.x == points[tri->verts[0]].pos.x
						&& points[tri->verts[2]].pos.y == points[tri->verts[0]].pos.y;

					if (!same_ab && !same_bc && !same_ca) {
						printf("Could not calculate wall triangle!\n");
					}
					// if two points are vertically aligned, we can proceed
					else {
						lui32 point_stack[2];
						lui32 point_straggler;

						if (same_ab) {
							point_stack[0] = tri->verts[0];
							point_stack[1] = tri->verts[1];
							point_straggler = tri->verts[2];
						}
						if (same_bc) {
							point_stack[0] = tri->verts[1];
							point_stack[1] = tri->verts[2];
							point_straggler = tri->verts[0];
						}
						if (same_ca) {
							point_stack[0] = tri->verts[2];
							point_stack[1] = tri->verts[0];
							point_straggler = tri->verts[1];
						}

						// sort the stack so 1 is higher
						if (points[point_stack[0]].h > points[point_stack[1]].h) {
							lui32 swap = point_stack[0];
							point_stack[0] = point_stack[1];
							point_stack[1] = swap;
						}

						// search for lines with the same XZ coords
						bool found_same = false;
						for (int j = 0; j < linecount; ++j) {
							EnvLineSeg* seg = &lines[j];

							// direct index would be easier...
							// Look for matching vertices
							// A matches stack
							if ((lines[j].pos_a == points[point_stack[0]].pos && lines[j].pos_b == points[point_straggler].pos)) {
								found_same = true;
								printf("Found matching line! Merging...\n");
								// modify line height
								if (points[point_stack[1]].h > lines[j].h_a_top) lines[j].h_a_top = points[point_stack[1]].h;
								if (points[point_stack[0]].h < lines[j].h_a_bot) lines[j].h_a_bot = points[point_stack[0]].h;
							}
							// B matches stack (B never matches the stack, for some reason)
							else if ((lines[j].pos_b == points[point_stack[0]].pos && lines[j].pos_a == points[point_straggler].pos)) {
								found_same = true;
								printf("Found matching line! Merging...\n");
								// modify line height
								if (points[point_stack[1]].h > lines[j].h_b_top) lines[j].h_b_top = points[point_stack[1]].h;
								if (points[point_stack[0]].h < lines[j].h_b_bot) lines[j].h_b_bot = points[point_stack[0]].h;
							}
						}
						// if we didnt find any matches, set a new line
						if (!found_same) {
							lines[linecount].pos_a = points[point_straggler].pos;
							lines[linecount].h_a_bot = points[point_straggler].h;
							lines[linecount].h_a_top = points[point_straggler].h;
							lines[linecount].pos_b = points[point_stack[0]].pos;
							lines[linecount].h_b_bot = points[point_stack[0]].h;
							lines[linecount].h_b_top = points[point_stack[1]].h;
							++linecount;
						}
					}
				}
				// Only add this triangle to any cells if its not a stupid useless wall triangle
				else {
					lui32 bound_min_x = (lui32)floorf(m::Min3(points[tris[i].verts[0]].pos.x, points[tris[i].verts[1]].pos.x, points[tris[i].verts[2]].pos.x));
					lui32 bound_min_y = (lui32)floorf(m::Min3(points[tris[i].verts[0]].pos.y, points[tris[i].verts[1]].pos.y, points[tris[i].verts[2]].pos.y));
					lui32 bound_max_x = (lui32)ceilf(m::Max3( points[tris[i].verts[0]].pos.x, points[tris[i].verts[1]].pos.x, points[tris[i].verts[2]].pos.x));
					lui32 bound_max_y = (lui32)ceilf(m::Max3( points[tris[i].verts[0]].pos.y, points[tris[i].verts[1]].pos.y, points[tris[i].verts[2]].pos.y));
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
									//triCells[x][y].Add(i);
									tcel[x * WORLD_SIZE + y].Add(i);
							}
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
					if (VertCompare(tri_compare->verts[0], i)) {
						// check for vertices allready added to neighbor list
						bool ne_already_added[2]{ false, false };
						for (int k = 0; k < vert->neighborcount; ++k) {
							if (vert->neighbors[k] == tri_compare->verts[1]) ne_already_added[0] = true;
							if (vert->neighbors[k] == tri_compare->verts[2]) ne_already_added[1] = true;
						}
						// add neighbors
						if (!ne_already_added[0]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[1];
							++vert->neighborcount;
						}
						if (!ne_already_added[1]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[2];
							++vert->neighborcount;
						}
					}
					if (VertCompare(tri_compare->verts[1], i)) {
						// check for vertices allready added to neighbor list
						bool ne_already_added[2]{ false, false };
						for (int k = 0; k < vert->neighborcount; ++k) {
							if (vert->neighbors[k] == tri_compare->verts[0]) ne_already_added[0] = true;
							if (vert->neighbors[k] == tri_compare->verts[2]) ne_already_added[1] = true;
						}
						// add neighbors
						if (!ne_already_added[0]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[0];
							++vert->neighborcount;
						}
						if (!ne_already_added[1]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[2];
							++vert->neighborcount;
						}
					}
					if (VertCompare(tri_compare->verts[2], i)) {
						// check for vertices allready added to neighbor list
						bool ne_already_added[2]{ false, false };
						for (int k = 0; k < vert->neighborcount; ++k) {
							if (vert->neighbors[k] == tri_compare->verts[0]) ne_already_added[0] = true;
							if (vert->neighbors[k] == tri_compare->verts[1]) ne_already_added[1] = true;
						}
						// add neighbors
						if (!ne_already_added[0]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[0];
							++vert->neighborcount;
						}
						if (!ne_already_added[1]) {
							vert->neighbors[vert->neighborcount] = tri_compare->verts[1];
							++vert->neighborcount;
						}
					}
				}
			}//*/

			#define DEF_TRI_NE_EXPERIMENTAL 0

			lf32* point_nearest_h_below = new lf32[pointcount];
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
				lui32 bound_min_x = (lui32)floorf(Min2(points[tri->verts[0]].pos.x, points[tri->verts[1]].pos.x, points[tri->verts[2]].pos.x));
				lui32 bound_min_y = (lui32)floorf(Min2(points[tri->verts[0]].pos.y, points[tri->verts[1]].pos.y, points[tri->verts[2]].pos.y));
				lui32 bound_max_x = (lui32)ceilf(Max2(points[tri->verts[0]].pos.x, points[tri->verts[1]].pos.x, points[tri->verts[2]].pos.x));
				lui32 bound_max_y = (lui32)ceilf(Max2(points[tri->verts[0]].pos.y, points[tri->verts[1]].pos.y, points[tri->verts[2]].pos.y));
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
									//if (tri_compare->verts[0] == tri->verts[0]) ++samecount;
									//if (tri_compare->verts[0] == tri->verts[1]) ++samecount;
									//if (tri_compare->verts[0] == tri->verts[2]) ++samecount;
									//if (tri_compare->verts[1] == tri->verts[0]) ++samecount;
									//if (tri_compare->verts[1] == tri->verts[1]) ++samecount;
									//if (tri_compare->verts[1] == tri->verts[2]) ++samecount;
									//if (tri_compare->verts[2] == tri->verts[0]) ++samecount;
									//if (tri_compare->verts[2] == tri->verts[1]) ++samecount;
									//if (tri_compare->verts[2] == tri->verts[2]) ++samecount;
									if (VertCompare(tri_compare->verts[0], tri->verts[0])) ++samecount;
									if (VertCompare(tri_compare->verts[0], tri->verts[1])) ++samecount;
									if (VertCompare(tri_compare->verts[0], tri->verts[2])) ++samecount;
									if (VertCompare(tri_compare->verts[1], tri->verts[0])) ++samecount;
									if (VertCompare(tri_compare->verts[1], tri->verts[1])) ++samecount;
									if (VertCompare(tri_compare->verts[1], tri->verts[2])) ++samecount;
									if (VertCompare(tri_compare->verts[2], tri->verts[0])) ++samecount;
									if (VertCompare(tri_compare->verts[2], tri->verts[1])) ++samecount;
									if (VertCompare(tri_compare->verts[2], tri->verts[2])) ++samecount;
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
						if (VertCompare(tri_compare->verts[0], tri->verts[0])) ++samecount;
						else if (VertCompare(tri_compare->verts[0], tri->verts[1])) ++samecount;
						else if (VertCompare(tri_compare->verts[0], tri->verts[2])) ++samecount;
						if (VertCompare(tri_compare->verts[1], tri->verts[0])) ++samecount;
						else if (VertCompare(tri_compare->verts[1], tri->verts[1])) ++samecount;
						else if (VertCompare(tri_compare->verts[1], tri->verts[2])) ++samecount;
						if (VertCompare(tri_compare->verts[2], tri->verts[0])) ++samecount;
						else if (VertCompare(tri_compare->verts[2], tri->verts[1])) ++samecount;
						else if (VertCompare(tri_compare->verts[2], tri->verts[2])) ++samecount;
						// If we share more than two points
						// It means that it's a neighbor tri
						if (samecount > 1) {
							// Set neighbor
							tri->neighbors[tri->neighborcount] = j;
							++tri->neighborcount;
						}
						// Find ledges
						// TODO: only need to do this once per other VERTEX, not per triangle
						if (tri->facing_up) {
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[0]], tri->verts[0], tri_compare->verts[0]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[0]], tri->verts[0], tri_compare->verts[1]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[0]], tri->verts[0], tri_compare->verts[2]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[1]], tri->verts[1], tri_compare->verts[0]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[1]], tri->verts[1], tri_compare->verts[1]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[1]], tri->verts[1], tri_compare->verts[2]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[2]], tri->verts[2], tri_compare->verts[0]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[2]], tri->verts[2], tri_compare->verts[1]);
							VertCompareLedgeDetect(&point_nearest_h_below[tri->verts[2]], tri->verts[2], tri_compare->verts[2]);
						}
					}
				}
				#endif
			}

			// Determine triangle edges with no neighbors
			for (int i = 0; i < pointcount; ++i) {
				points[i].nor.x = 0.f;
				points[i].nor.y = 0.f;
			}
			for (int i = 0; i < tricount; ++i) {
				EnvTri* tri = &tris[i];
				lui32 points_connected[3]{ 0u, 0u, 0u };
				tri->open_edge_ab = false;
				tri->open_edge_bc = false;
				tri->open_edge_ca = false;
				// find any edges that don't have neighbors attached
				for (int j = 0; j < tri->neighborcount; ++j) {
					EnvTri* tri_compare = &tris[tri->neighbors[j]];
					// Do we share any vertices?
					if (VertCompare(tri->verts[0], tri_compare->verts[0])) ++points_connected[0];
					else if (VertCompare(tri->verts[0], tri_compare->verts[1])) ++points_connected[0];
					else if (VertCompare(tri->verts[0], tri_compare->verts[2])) ++points_connected[0];
					if (VertCompare(tri->verts[1], tri_compare->verts[0])) ++points_connected[1];
					else if (VertCompare(tri->verts[1], tri_compare->verts[1])) ++points_connected[1];
					else if (VertCompare(tri->verts[1], tri_compare->verts[2])) ++points_connected[1];
					if (VertCompare(tri->verts[2], tri_compare->verts[0])) ++points_connected[2];
					else if (VertCompare(tri->verts[2], tri_compare->verts[1])) ++points_connected[2];
					else if (VertCompare(tri->verts[2], tri_compare->verts[2])) ++points_connected[2];
				}
				// we can figure out which edges aren't connected to other triangles based on
				// the number of connections each vertex has (maximum of 2)
				tri->open_edge_ab = points_connected[0] < points_connected[2] || points_connected[1] < points_connected[2];
				tri->open_edge_bc = points_connected[1] < points_connected[0] || points_connected[2] < points_connected[0];
				tri->open_edge_ca = points_connected[2] < points_connected[1] || points_connected[0] < points_connected[1];
				// test (i think we can just assume a certain winding direction for these
				m::Vector2 normal_ab = m::Normalize(points[tri->verts[0]].pos - points[tri->verts[1]].pos);
				normal_ab = m::Vector2(normal_ab.y, -normal_ab.x);
				m::Vector2 normal_bc = m::Normalize(points[tri->verts[1]].pos - points[tri->verts[2]].pos);
				normal_bc = m::Vector2(normal_bc.y, -normal_bc.x);
				m::Vector2 normal_ca = m::Normalize(points[tri->verts[2]].pos - points[tri->verts[0]].pos);
				normal_ca = m::Vector2(normal_ca.y, -normal_ca.x);

				// now make collision lines here
				// and make nav offsets
				//m::Vector2 v2 = lineLineIntersection(points[tri->verts[0]].pos, points[tri->verts[1]].pos, points[tri->verts[0]].pos, points[tri->verts[2]].pos);
				if (tri->open_edge_ab) {
					// if there was any ledge
					/*if (point_nearest_h_below[tri->verts[0]] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->verts[1]] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->verts[0]].pos;
					lines[linecount].h_a_top = points[tri->verts[0]].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->verts[0]];
					lines[linecount].pos_b = points[tri->verts[1]].pos;
					lines[linecount].h_b_top = points[tri->verts[1]].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->verts[1]];
					++linecount;
					}*/
					//temporary solution
					points[tri->verts[0]].nor += normal_ab;
					points[tri->verts[1]].nor += normal_ab;
				}
				if (tri->open_edge_bc) {
					// if there was any ledge
					/*if (point_nearest_h_below[tri->verts[1]] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->verts[2]] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->verts[1]].pos;
					lines[linecount].h_a_top = points[tri->verts[1]].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->verts[1]];
					lines[linecount].pos_b = points[tri->verts[2]].pos;
					lines[linecount].h_b_top = points[tri->verts[2]].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->verts[2]];
					++linecount;
					}*/
					//temporary solution
					points[tri->verts[1]].nor += normal_bc;
					points[tri->verts[2]].nor += normal_bc;
				}
				if (tri->open_edge_ca) {
					// if there was any ledge
					/*if (point_nearest_h_below[tri->verts[2]] != WORLD_HEIGHT_MIN && point_nearest_h_below[tri->verts[0]] != WORLD_HEIGHT_MIN) {
					lines[linecount].pos_a = points[tri->verts[2]].pos;
					lines[linecount].h_a_top = points[tri->verts[2]].h;
					lines[linecount].h_a_bot = point_nearest_h_below[tri->verts[2]];
					lines[linecount].pos_b = points[tri->verts[0]].pos;
					lines[linecount].h_b_top = points[tri->verts[0]].h;
					lines[linecount].h_b_bot = point_nearest_h_below[tri->verts[0]];
					++linecount;
					}*/
					//temporary solution
					points[tri->verts[2]].nor += normal_ca;
					points[tri->verts[0]].nor += normal_ca;
				}
			}

			// finish lines
			for (int i = 0; i < linecount; ++i) {
				EnvLineSeg* line = &lines[i];
				line->csn_position = (line->pos_a + line->pos_b) / 2;
				line->csn_scale.x = m::Length(line->pos_a - line->pos_b) / 2;
				line->csn_scale.y = 0.f;
				line->csn_rotation = m::Vec2ToAngRH(m::Normalize(line->pos_b - line->pos_a));
			}

			// precompute line cells
			// for all lines
			for (int i = 0; i < linecount; ++i) {
				lui32 bound_min_x = (lui32)floorf(m::Min2(lines[i].pos_a.x, lines[i].pos_b.x));
				lui32 bound_min_y = (lui32)floorf(m::Min2(lines[i].pos_a.y, lines[i].pos_b.y));
				lui32 bound_max_x = (lui32)ceilf(m::Max2(lines[i].pos_a.x, lines[i].pos_b.x));
				lui32 bound_max_y = (lui32)ceilf(m::Max2(lines[i].pos_a.y, lines[i].pos_b.y));
				// make sure the bounds are within the world
				if (bound_min_x < WORLD_SIZE && bound_min_y < WORLD_SIZE
					&& bound_max_x < WORLD_SIZE && bound_max_y < WORLD_SIZE) {
					// for every cell within the bounds
					for (int x = bound_min_x; x <= bound_max_x; ++x) {
						for (int y = bound_min_y; y <= bound_max_y; ++y) {
							c2AABB box;
							box.min.x = x - 0.5f; box.min.y = y - 0.5f;
							box.max.x = x + 0.5f; box.max.y = y + 0.5f;

							c2Ray ray;
							ray.p = c2V(lines[i].pos_a.x, lines[i].pos_a.y);
							m::Vector2 dir = m::Normalize(lines[i].pos_b - lines[i].pos_a);
							lf32 len = m::Length(lines[i].pos_b - lines[i].pos_a);
							ray.d = c2V(dir.x, dir.y);
							ray.t = len;

							c2Raycast raycast;
							// If the line segment and box intersect, add the line to the cell
							if (c2RaytoAABB(ray, box, &raycast)) {
								//lineCells[x][y].Add(i);
								lcel[x * WORLD_SIZE + y].Add(i);

							}
						}
					}
				}
			}

			delete[] point_nearest_h_below;
		}
		void Free() {
			delete[] points;
			delete[] tris;
			delete[] trimeta;
			delete[] triCollides;
			delete[] lines;
		}
		void Save(FILE* file, mem::idbuf* tcel, mem::idbuf* lcel) {
			if (file == NULL) return;
			// Points
			fwrite(&pointcount, 4, 1, file);
			fwrite(&points[0], sizeof(EnvVert), pointcount, file);
			// Triangles
			fwrite(&tricount, 4, 1, file);
			fwrite(&tris[0], sizeof(EnvTri), tricount, file);
			fwrite(&trimeta[0], sizeof(EnvTriMeta), tricount, file);
			fwrite(&triCollides[0], sizeof(c2Poly), tricount, file);
			// Lines
			fwrite(&linecount, 4, 1, file);
			fwrite(&lines[0], sizeof(EnvLineSeg), linecount, file);
			// Cells (temp)
			fwrite(tcel, sizeof(mem::idbuf) * WORLD_SIZE_SQUARED, 1, file);
			fwrite(lcel, sizeof(mem::idbuf) * WORLD_SIZE_SQUARED, 1, file);
		}
		void Load(FILE* file, mem::idbuf* tcel, mem::idbuf* lcel) {
			if (file == NULL) return;
			// Points
			fread(&pointcount, 4, 1, file);
			points = new EnvVert[pointcount];
			fread(&points[0], sizeof(EnvVert), pointcount, file);
			// Triangles
			fread(&tricount, 4, 1, file);
			tris = new EnvTri[tricount];
			fread(&tris[0], sizeof(EnvTri), tricount, file);
			trimeta = new EnvTriMeta[tricount];
			fread(&trimeta[0], sizeof(EnvTriMeta), tricount, file);
			triCollides = new c2Poly[tricount];
			fread(&triCollides[0], sizeof(c2Poly), tricount, file);
			// Lines
			fread(&linecount, 4, 1, file);
			lines = new EnvLineSeg[linecount];
			fread(&lines[0], sizeof(EnvLineSeg), linecount, file);
			// Cells (temp)
			if (tcel != nullptr)
				fread(tcel, sizeof(mem::idbuf) * WORLD_SIZE_SQUARED, 1, file);
			if (lcel != nullptr)
				fread(lcel, sizeof(mem::idbuf) * WORLD_SIZE_SQUARED, 1, file);
		}
	};
	EnvMesh emesh;

	// Cells
	mem::idbuf triCells[WORLD_SIZE][WORLD_SIZE];
	mem::idbuf lineCells[WORLD_SIZE][WORLD_SIZE];

	// from rosetta code
	// https://rosettacode.org/wiki/Find_the_intersection_of_two_lines#C
	double LineSlope(m::Vector2 a, m::Vector2 b) {
		if (a.x - b.x == 0.0)
			return NAN;
		else
			return (a.y - b.y) / (a.x - b.x);
	}
	m::Vector2 LineLineIntersectionInf(m::Vector2 a1, m::Vector2 a2, m::Vector2 b1, m::Vector2 b2) {
		m::Vector2 c;

		double slopeA = LineSlope(a1, a2), slopeB = LineSlope(b1, b2);

		if (slopeA == slopeB) {
			c.x = NAN;
			c.y = NAN;
		}
		else if (isnan(slopeA) && !isnan(slopeB)) {
			c.x = a1.x;
			c.y = (a1.x - b1.x)*slopeB + b1.y;
		}
		else if (isnan(slopeB) && !isnan(slopeA)) {
			c.x = b1.x;
			c.y = (b1.x - a1.x)*slopeA + a1.y;
		}
		else {
			c.x = (slopeA*a1.x - slopeB*b1.x + b1.y - a1.y) / (slopeA - slopeB);
			c.y = slopeB*(c.x - b1.x) + b1.y;
		}

		return c;
	}


	void EnvTriBary(m::Vector2 p, m::Vector2 a, m::Vector2 b, m::Vector2 c, lf32 &u, lf32 &v, lf32 &w)
	{
		m::Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
		lf32 d00 = m::Dot(v0, v0);
		lf32 d01 = m::Dot(v0, v1);
		lf32 d11 = m::Dot(v1, v1);
		lf32 d20 = m::Dot(v2, v0);
		lf32 d21 = m::Dot(v2, v1);
		lf32 denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}

	lui32 GetNumTris(WCoord coords) {
		return triCells[coords.x][coords.y].Size();
	}
	lf32 GetTriHeight(WCoord coords, lui32 index, lf32 pos_x, lf32 pos_y) {
		lf32 u, v, w;
		EnvTri* tri = &emesh.tris[(triCells[coords.x][coords.y])[index]];
		EnvTriBary(m::Vector2(pos_x, pos_y), emesh.points[tri->verts[0]].pos, emesh.points[tri->verts[1]].pos, emesh.points[tri->verts[2]].pos, u, v, w);
		return emesh.points[tri->verts[0]].h * u + emesh.points[tri->verts[1]].h * v + emesh.points[tri->verts[2]].h * w;
	}
	bool GetTriExists(WCoord coords, lui32 index) {
		return (triCells[coords.x][coords.y])[index] != ID_NULL;
	}
	void* GetC2Tri(WCoord coords, lui32 index) {
		return (void*)&emesh.triCollides[(triCells[coords.x][coords.y])[index]];
	}
	EnvTri* GetTri(WCoord coords, lui32 index) {
		return &emesh.tris[(triCells[coords.x][coords.y])[index]];
	}
	lui32 GetNumLines(WCoord coords) {
		return lineCells[coords.x][coords.y].Size();
	}
	EnvLineSeg* GetLine(WCoord coords, lui32 index) {
		return &emesh.lines[(lineCells[coords.x][coords.y])[index]];
	}

	void GetFloorsAndCeilings(CellSpace& csinf, lf32 in_height, EnvTriSurfaceSet* set) {
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
		lf32 u, v, w;
		lf32 height;
		lui32 intersectionCount = 0u;
		// For each triangle cell we're touching
		for (int cell = 0; cell < eCELL_COUNT; ++cell) {
			// For each triangle in this cell
			for (int i = 0; i < triCells[csinf.c[cell].x][csinf.c[cell].y].Size(); ++i) {
				// If this triangle exists
				if ((triCells[csinf.c[cell].x][csinf.c[cell].y])[i] != ID_NULL) {
					EnvTri* tri = &emesh.tris[(triCells[csinf.c[cell].x][csinf.c[cell].y])[i]];
					EnvTriBary(
						m::Vector2((lf32)csinf.c[eCELL_I].x + csinf.offsetx, (lf32)csinf.c[eCELL_I].y + csinf.offsety),
						emesh.points[tri->verts[0]].pos, emesh.points[tri->verts[1]].pos, emesh.points[tri->verts[2]].pos, u, v, w);
					// if we're inside the triangle
					if (u <= 1.f && v <= 1.f && w <= 1.f && u >= 0.f && v >= 0.f && w >= 0.f) {
						++intersectionCount;
						// Record the height of the triangle's plane at our coordinates
						height = emesh.points[tri->verts[0]].h * u + emesh.points[tri->verts[1]].h * v + emesh.points[tri->verts[2]].h * w;
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
		if (intersectionCount == 0u) {
			set->nearest_ceil_h_above = 0.f;
			set->nearest_flor_h_above = 0.f;
			set->nearest_ceil_h_below = 0.f;
			set->nearest_flor_h_below = 0.f;
		}
	}

	lid GetTriAtPos(lf32 x, lf32 y)
	{
		WCoord coords;
		coords.x = (lui16)roundf(x);
		coords.y = (lui16)roundf(y);
		for (int i = 0; i < triCells[coords.x][coords.y].Size(); ++i) {
			lf32 u, v, w;
			EnvTri* tri = &emesh.tris[(triCells[coords.x][coords.y])[i]];
			EnvTriBary(m::Vector2(x, y), emesh.points[tri->verts[0]].pos, emesh.points[tri->verts[1]].pos, emesh.points[tri->verts[2]].pos, u, v, w);
			if (u <= 1.f && v <= 1.f && w <= 1.f && u >= 0.f && v >= 0.f && w >= 0.f)
				return (triCells[coords.x][coords.y])[i];
		}
		return ID_NULL;
	}

	#endif

	#if DEF_GRID
	bool Get(lui32 x, lui32 y, eflag::flag bit)
	{
		return mem::bvget((uint32_t)eCells.flags[x][y], (uint32_t)bit);
	}
	void Set(lui32 x, lui32 y, eflag::flag bit)
	{
		mem::bvset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}
	void UnSet(lui32 x, lui32 y, eflag::flag bit)
	{
		mem::bvunset((uint32_t&)eCells.flags[x][y], (uint32_t)bit);
	}
	void GetHeight(lf32& out_height, CellSpace& csinf)
	{
		// BILINEAR
		/*
		out_height = m::Lerp(
			m::Lerp((lf32)eCells.terrain_height[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(lf32)eCells.terrain_height[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y],
				fabs(csinf.offsetx)),
			m::Lerp((lf32)eCells.terrain_height[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y],
			(lf32)eCells.terrain_height[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y],
				fabs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
		//*/

		// BILINEAR2
		out_height = m::Lerp(
			m::Lerp((lf32)eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(lf32)eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
				(csinf.offsetx + 0.5f)),
			m::Lerp((lf32)eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
			(lf32)eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y],
				(csinf.offsetx + 0.5f)),
			(csinf.offsety + 0.5f)) / TERRAIN_HEIGHT_DIVISION;
	}
	#else
	void GetNearestCeilingHeight(lf32& out_height, CellSpace& cs, lf32 in_height)
	{
		out_height = 0.f;
		EnvTriSurfaceSet surfset;
		GetFloorsAndCeilings(cs, in_height, &surfset);
		out_height = surfset.nearest_ceil_h_above;
	}
	void GetNearestSurfaceHeight(lf32& out_height, CellSpace& cs, lf32 in_height)
	{
		out_height = 0.f;

		EnvTriSurfaceSet surfset;
		GetFloorsAndCeilings(cs, in_height, &surfset);

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
	void GetNearestSurfaceHeight(lf32& out_height, EnvTri** out_tri, CellSpace& cs, lf32 in_height)
	{
		out_height = 0.f;
		*out_tri = nullptr;

		EnvTriSurfaceSet surfset;
		GetFloorsAndCeilings(cs, in_height, &surfset);

		// todo: need special treatment if there are no triangles on us at all
		// are we outside of any geometry
		if (surfset.nearest_ceil_h_below < surfset.nearest_flor_h_below) {
			out_height = surfset.nearest_flor_h_below;
			*out_tri = surfset.nearest_flor_below;
		}
		// or inside
		else {
			out_height = surfset.nearest_flor_h_above;
			*out_tri = surfset.nearest_flor_above;
		}
	}
	#endif
	#if DEF_GRID
	void GetSlope(lf32& out_slope_x, lf32& out_slope_y, CellSpace& csinf) {
		// Slope X
		out_slope_x = (lf32)(env::eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_x += (lf32)(env::eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_x /= (TERRAIN_HEIGHT_DIVISION * 2.f);
		// Slope Y
		out_slope_y = (lf32)(env::eCells.terrain_height_ne[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_se[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_y += (lf32)(env::eCells.terrain_height_nw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]
			- env::eCells.terrain_height_sw[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y]);
		out_slope_y /= (TERRAIN_HEIGHT_DIVISION * 2.f);
	}
	#endif

	#if !DEF_GRID
	// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
	// Tricks of the Windows Game Programming Gurus (2nd Edition)
	// https://www.amazon.com/dp/0672323699
	// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
	// intersect the intersection point may be stored in the floats i_x and i_y.
	char LineSegIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
		float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
	{
		float s1_x, s1_y, s2_x, s2_y;
		s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
		s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

		float s, t;
		s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
		t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			// Collision detected
			if (i_x != NULL)
				*i_x = p0_x + (t * s1_x);
			if (i_y != NULL)
				*i_y = p0_y + (t * s1_y);
			return 1;
		}

		return 0; // No collision
	}
	bool LineTrace(lf32 x1, lf32 y1, lf32 x2, lf32 y2, lf32 height_a, lf32 height_b)
	{
		lui32 bound_min_x = (lui32)floorf(m::Min2(x1, x2));
		lui32 bound_min_y = (lui32)floorf(m::Min2(y1, y2));
		lui32 bound_max_x = (lui32)ceilf(m::Max2(x1, x2));
		lui32 bound_max_y = (lui32)ceilf(m::Max2(y1, y2));

		// make sure the bounds are within the world
		if (bound_min_x < WORLD_SIZE && bound_min_y < WORLD_SIZE
			&& bound_max_x < WORLD_SIZE && bound_max_y < WORLD_SIZE) {
			// for every cell within the bounds
			for (int x = bound_min_x; x <= bound_max_x; ++x) {
				for (int y = bound_min_y; y <= bound_max_y; ++y) {
					for (int i = 0; i < lineCells[x][y].Size(); ++i) {
						EnvLineSeg* seg = &emesh.lines[(lineCells[x][y])[i]];
						m::Vector2 isec;
						int piss = LineSegIntersection(x1, y1, x2, y2, seg->pos_a.x, seg->pos_a.y, seg->pos_b.x, seg->pos_b.y, &isec.x, &isec.y);
						if (piss) return true;
					}
				}
			}
		}
		return false;
	}
	bool LineTrace(lf32 x1, lf32 y1, lf32 x2, lf32 y2, lf32 height_a, lf32 height_b, LineTraceHit* out_hit)
	{
		lui32 bound_min_x = (lui32)floorf(m::Min2(x1, x2));
		lui32 bound_min_y = (lui32)floorf(m::Min2(y1, y2));
		lui32 bound_max_x = (lui32)ceilf(m::Max2(x1, x2));
		lui32 bound_max_y = (lui32)ceilf(m::Max2(y1, y2));

		lf32 closest_hit = INFINITY;

		bool hit = false;
		m::Vector2 intersection_point;
		lf32 intersection_height;

		// make sure the bounds are within the world
		if (bound_min_x < WORLD_SIZE && bound_min_y < WORLD_SIZE
			&& bound_max_x < WORLD_SIZE && bound_max_y < WORLD_SIZE) {
			// for every cell within the bounds
			for (int x = bound_min_x; x <= bound_max_x; ++x) {
				for (int y = bound_min_y; y <= bound_max_y; ++y) {
					for (int i = 0; i < lineCells[x][y].Size(); ++i) {
						EnvLineSeg* seg = &emesh.lines[(lineCells[x][y])[i]];

						m::Vector2 isec;
						int piss = LineSegIntersection(x1, y1, x2, y2, seg->pos_a.x, seg->pos_a.y, seg->pos_b.x, seg->pos_b.y, &isec.x, &isec.y);
						if (piss) {
							// height collision (not quite accurate)
							lf32 lena = m::Length(m::Vector2(x2, y2) - m::Vector2(x1, y1));
							lf32 lenb = m::Length(seg->pos_b - seg->pos_a);

							lf32 lenpoint1a = m::Length(isec - m::Vector2(x1, y1)) / lena;
							lf32 lenpoint1b = m::Length(isec - m::Vector2(x2, y2)) / lena;
							lf32 lenpoint2a = m::Length(isec - seg->pos_a) / lenb;
							lf32 lenpoint2b = m::Length(isec - seg->pos_b) / lenb;

							lf32 comp_heighta = height_a * lenpoint1a + height_b * lenpoint1b;
							lf32 comp_heightbtop = seg->h_b_top * lenpoint2a + seg->h_a_top * lenpoint2b;
							lf32 comp_heightbbot = seg->h_b_bot * lenpoint2a + seg->h_a_bot * lenpoint2b;

							// hit
							if (comp_heightbtop >= comp_heighta && comp_heightbbot <= comp_heighta) { // height compare
								if (closest_hit > lenpoint1a) {
									closest_hit = lenpoint1a;
									hit = true;
									intersection_point = isec;
									intersection_height = comp_heighta;

									//out_hit->pos = isec;
									//out_hit->h = comp_heighta;
									//return true;
								}
							}
						}
					}
				}
			}
		}

		if (hit) {
			out_hit->pos = intersection_point;
			out_hit->h = intersection_height;
			return true;
		}

		return false;
	}
	#endif
	#if DEF_GRID
	forceinline bool LineTraceUtil_CheckCollideEnv(int x, int y, lf32 height_a, lf32 height_b, lf32 lerp)
	{
		lf32 mix = m::Lerp(height_a, height_b, lerp);
		if (((lf32)env::eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION) > mix + 0.7f)
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
		lf32 height_a, lf32 height_b)
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
		lf32 axisLen = (lf32)(coordA - coord_end);
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
			lf32 axisLerp = (lf32)(coordA - coord_end) / axisLen;
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
	bool LineTraceBh(int x1, int y1, int x2, int y2, lf32 height_a, lf32 height_b)
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
	bool LineTrace_Bresenham_Bak(int x1, int y1, int x2, int y2, lf32 height_a, lf32 height_b)
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
	#endif

	void Tick()
	{
		// eventually, this function could contain any 'propagation' like fire
	}

	void DrawProps()
	{
		#if DEF_GRID
		for (lui32 i = 0u; i < wldNumTextures; ++i)
		{
			DrawCompositeMesh(ID_NULL, wldMeshes[i], acv::GetT(wldTxtr[i]), SS_NORMAL, graphics::Matrix4x4());
		}
		#endif
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
		#if !DEF_GRID
		DrawMesh(ID_NULL, acv::GetM(acv::m_world), acv::GetT(acv::t_terrain_01), SS_NORMAL, graphics::Matrix4x4());
		//DrawMesh(ID_NULL, acv::GetM(acv::m_world_phys), acv::GetT(acv::t_terrain_01), SS_NORMAL, graphics::Matrix4x4());
		#else
		DrawTerrainMesh(ID_NULL, wldMeshTerrain,
			acv::GetT(acv::t_terrain_01), acv::GetT(acv::t_terrain_02),
			acv::GetT(acv::t_terrain_03), acv::GetT(acv::t_terrain_04),
			acv::GetT(acv::t_terrain_05), acv::GetT(acv::t_terrain_06),
			acv::GetT(acv::t_terrain_07), acv::GetT(acv::t_terrain_08),
			graphics::Matrix4x4());
		#endif
	}
	void DrawDebugGizmos(CellSpace* cs)
	{
		#if !DEF_GRID
		lid tri_id = GetTriAtPos(cs->c[eCELL_I].x + cs->offsetx, cs->c[eCELL_I].y + cs->offsety);
		if (tri_id != ID_NULL) {
			EnvTri* tri = &emesh.tris[GetTriAtPos(cs->c[eCELL_I].x + cs->offsetx, cs->c[eCELL_I].y + cs->offsety)];
			EnvVert* va = &emesh.points[tri->verts[0]];
			EnvVert* vb = &emesh.points[tri->verts[1]];
			EnvVert* vc = &emesh.points[tri->verts[2]];

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
		#endif
	}
	void DrawTerrainDebug()
	{
		#if !DEF_GRID
		DrawMesh(ID_NULL, acv::GetM(acv::m_world), acv::GetT(acv::t_terrain_01), SS_NORMAL, graphics::Matrix4x4());
		graphics::SetRenderWire();
		glLineWidth(2.f);
		DrawMesh(ID_NULL, acv::GetM(acv::m_world_phys), acv::GetT(acv::t_col_red), SS_NORMAL, graphics::Matrix4x4());
		graphics::SetRenderSolid();
		glLineWidth(1.f);
		#else
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
		#endif
	}

	void Clean()
	{
		#if DEF_GRID
		for (int x = 0; x < WORLD_SIZE; ++x) {
			for (int y = 0; y < WORLD_SIZE; ++y) {
				eCells.terrain_height[x][y] = m::Max<lui8>(4,
					eCells.terrain_height_ne[x][y], eCells.terrain_height_nw[x][y],
					eCells.terrain_height_se[x][y], eCells.terrain_height_sw[x][y]);
			}
		}
		#endif
	}

	#define ENV_SAVE_V 0u

	void SaveBin()
	{
		Clean();
		printf("Saving World\n");
		FILE *file = fopen(WORLD_FILENAME, "wb");
		if (file != NULL) {
			fseek(file, 0, SEEK_SET); // Seek the beginning of the file
			lui64 fversion = ENV_SAVE_V; // futureproofing
			fwrite(&fversion, 8, 1, file);
			#if DEF_GRID
			fwrite(&eCells, sizeof(EnvNode), 1, file);
			#else
			emesh.Save(file, &triCells[0][0], &lineCells[0][0]);
			#endif
			fclose(file);
		}
	}

	#undef ENV_SAVE_V

	void LoadBin()
	{
		printf("Loading World\n");
		FILE *file = fopen(WORLD_FILENAME, "rb");
		if (file != NULL) {
			fseek(file, 0, SEEK_SET); // Seek the beginning of the file
			lui64 fversion = 0u;
			fread(&fversion, 8, 1, file);
			#if DEF_GRID
			fread(&eCells, sizeof(EnvNode), 1, file);
			#else
			emesh.Load(file, &triCells[0][0], &lineCells[0][0]);
			#endif
			fclose(file);
		}
		#if DEF_GRID
		GenerateTerrainMesh();
		GeneratePropMeshes();
		#endif
	}

	#if !DEF_GRID
	void GeneratePhysMesh()
	{
		// Load the old mesh
		env::EnvMesh oldMesh;
		FILE *file = fopen(WORLD_FILENAME, "rb");
		if (file != NULL) {
			fseek(file, 0, SEEK_SET); // Seek the beginning of the file
			lui64 fversion = 0u;
			fread(&fversion, 8, 1, file);
			oldMesh.Load(file, nullptr, nullptr);
			fclose(file);

			// Generate new mesh
			emesh.Generate(&triCells[0][0], &lineCells[0][0]);

			// Now look for duplicates and copy their metadata

			// for every tri in the old mesh
			for (int i = 0; i < oldMesh.tricount; ++i) {
				// for every tri in the new mesh
				for (int j = 0; j < emesh.tricount; ++j) {
					EnvTri* oldtri = &oldMesh.tris[i];
					EnvTri* newtri = &emesh.tris[j];
					//int numMatches = 0;
					bool matches[3]{ false, false, false };
					// for each vertex in the old triangle
					for (int k = 0; k < 3; ++k) {
						// for each vertex in the new triangle
						for (int l = 0; l < 3; ++l) {
							// Compare positions
							EnvVert* oldvert = &oldMesh.points[oldtri->verts[k]];
							EnvVert* newvert = &emesh.points[newtri->verts[l]];
							if (oldvert->pos == newvert->pos && oldvert->h == newvert->h)
								matches[k] = true;
						}
					}
					// if 3 matches we've found a triangle that carries over from the old file
					if (matches[0] && matches[1] && matches[2]) {
						// Copy metadata
						EnvTriMeta* oldtrimeta = &oldMesh.trimeta[i];
						EnvTriMeta* newtrimeta = &emesh.trimeta[j];
						memcpy(newtrimeta, oldtrimeta, sizeof(EnvTriMeta));
						printf("Copying old tri %i to new tri %i\n", i, j);
						// Having found our match for this triangle,
						// we can move on to the next
						continue; // skip j iteration
					}
				}
			}

			// Free the old mesh
			oldMesh.Free();
		}
		else {
			emesh.Generate(&triCells[0][0], &lineCells[0][0]); // Make new
		}
	}
	#endif

	#if DEF_GRID
	void GeneratePropMeshes()
	{
		int tile_radius = 128;
		for (int x = 1024 - tile_radius; x < 1024 + tile_radius; ++x) {
			for (int y = 1024 - tile_radius; y < 1024 + tile_radius; ++y) {
				if (env::eCells.prop[x][y] != ID_NULL && env::eCells.prop[x][y] != 0u) {
					bool foundTxtr = false;
					// are we using a new texture on this asset
					for (lui32 i = 0; i < wldNumTextures; ++i) {
						if (acv::props[env::eCells.prop[x][y]].idTxtr == wldTxtr[i]) {
							wldMeshes[i].AddMesh(&acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
								m::Vector3((lf32)x, (lf32)eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, (lf32)y),
								(graphics::CompositeMesh::MeshOrientation)env::eCells.prop_dir[x][y]);
							foundTxtr = true;
						}
					}
					if (!foundTxtr) {
						for (lui32 i = 0; i < NUM_COMPOSITES; ++i) {
							if (wldTxtr[i] == 0u) {
								wldTxtr[i] = acv::props[env::eCells.prop[x][y]].idTxtr;
								wldMeshes[i].AddMesh(&acv::GetM(acv::props[env::eCells.prop[x][y]].idMesh),
									m::Vector3((lf32)x, (lf32)eCells.terrain_height[x][y] / TERRAIN_HEIGHT_DIVISION, (lf32)y),
									(graphics::CompositeMesh::MeshOrientation)env::eCells.prop_dir[x][y]);
								++wldNumTextures;
								break;
							}
						}
					}
				}
			}
		}
		for (lui32 i = 0; i < wldNumTextures; ++i) {
			wldMeshes[i].ReBindGL();
		}
	}
	void GenerateTerrainMesh()
	{
		wldMeshTerrain.GenerateComplexEnv(eCells.terrain_height, eCells.terrain_material,
			(lui32*)&eCells.flags, eflag::EF_INVISIBLE,
			eCells.terrain_height_ne, eCells.terrain_height_nw,
			eCells.terrain_height_se, eCells.terrain_height_sw);
		//wldMeshTerrain.GenerateFromHMap(eCells.terrain_height, eCells.terrain_material);
	}
	#endif
	void Free()
	{
		#if !DEF_GRID
		emesh.Free();
		#endif
	}
}

enum NodeFromDir : lui8
{
	ND_UNSET,
	ND_FROM_NORTH,
	ND_FROM_SOUTH,
	ND_FROM_EAST,
	ND_FROM_WEST,
};
NodeFromDir node_cache[WORLD_SIZE][WORLD_SIZE];

#define INT_NULL 0b11111111111111111111111111111111

#if !DEF_GRID
struct pathNodeTest {
	lui32 point;
	//lid owner_tri;
};
#endif
#if DEF_GRID
bool path::PathFind(Path* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest)
#else
bool path::PathFind(Path* path, lf32 x, lf32 y, lf32 xDest, lf32 yDest)
#endif
{
	#if DEF_GRID
	
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
	// Add destination node
	path->nodes[path->len] = current;
	++path->len;
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

	// clear modified cache
	for (int i = 0; i < usedSet.size(); ++i)
		node_cache[usedSet[i].x][usedSet[i].y] = ND_UNSET;

	return true;
	
	#else
	
	lui32* node_from_cache = new lui32[env::emesh.pointcount];
	for (int i = 0; i < env::emesh.pointcount; ++i) {
		node_from_cache[i] = INT_NULL;
	}

	// todo: use height to find the right floor triangles
	lid tri_start = env::GetTriAtPos(x, y);
	lid tri_dest = env::GetTriAtPos(xDest, yDest);
	if (tri_start != ID_NULL && tri_dest != ID_NULL)
	{
		pathNodeTest node_start;
		pathNodeTest node_dest;
		lf32 distances[3];
		
		// Find nearest node to our starting position
		distances[0] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_start].verts[0]].pos.x, y - env::emesh.points[env::emesh.tris[tri_start].verts[0]].pos.y));
		distances[1] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_start].verts[1]].pos.x, y - env::emesh.points[env::emesh.tris[tri_start].verts[1]].pos.y));
		distances[2] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_start].verts[2]].pos.x, y - env::emesh.points[env::emesh.tris[tri_start].verts[2]].pos.y));
		switch (m::MinIndex(3u, distances)) {
		case 0u: node_start.point = env::emesh.tris[tri_start].verts[0]; break;
		case 1u: node_start.point = env::emesh.tris[tri_start].verts[1]; break;
		case 2u: node_start.point = env::emesh.tris[tri_start].verts[2]; break;
		}
		// Find nearest node to our destination
		distances[0] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_dest].verts[0]].pos.x, y - env::emesh.points[env::emesh.tris[tri_dest].verts[0]].pos.y));
		distances[1] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_dest].verts[1]].pos.x, y - env::emesh.points[env::emesh.tris[tri_dest].verts[1]].pos.y));
		distances[2] = m::Length(m::Vector2(x - env::emesh.points[env::emesh.tris[tri_dest].verts[2]].pos.x, y - env::emesh.points[env::emesh.tris[tri_dest].verts[2]].pos.y));
		switch (m::MinIndex(3u, distances)) {
		case 0u: node_dest.point = env::emesh.tris[tri_dest].verts[0]; break;
		case 1u: node_dest.point = env::emesh.tris[tri_dest].verts[1]; break;
		case 2u: node_dest.point = env::emesh.tris[tri_dest].verts[2]; break;
		}

		// set intial node to point to itself, just in case
		node_from_cache[node_start.point] = node_start.point;

		std::queue<pathNodeTest> openSet;
		openSet.push(node_start);
		// while there are still nodes to analyse
		while (!openSet.empty()) {
			pathNodeTest current = openSet.front();
			openSet.pop();
			// if we've reached the destination point
			if (current.point == node_dest.point)
				break;

			// Get the point we're examining for neighbors
			env::EnvVert* point_this = &env::emesh.points[current.point];

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
		lui32 vert_examine = node_dest.point;
		while (vert_examine != node_start.point) {
			if (path->len < PATH_NUM_NODES) {
				if (node_from_cache[vert_examine] == INT_NULL) {
					printf("Path led us to node that hasn't been examined!\n");
					return false;
				}
				printf("Path added node %i\n", vert_examine);
				// Set this path node
				path->pos_x[path->len] = env::emesh.points[vert_examine].pos.x + env::emesh.points[vert_examine].nor.x * 0.5f; // todo: replace 0.5 with agent size
				path->pos_y[path->len] = env::emesh.points[vert_examine].pos.y + env::emesh.points[vert_examine].nor.y * 0.5f;
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
	// Could not find a triangle under the target
	return false;
	
	#endif
}
