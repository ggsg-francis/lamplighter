#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "memory.hpp"
#include "maths.hpp"

typedef btui32 btcoord;
//duplicate struct (of what?)
struct CellCoord
{
	btcoord x, y;
	CellCoord(btcoord _x, btcoord _y) : x{ _x }, y{ _y } {};
};
// Enumerator representing the intended function of the 4 cell coordinates used
enum eCell : btui32
{
	eCELL_I, // This cell
	eCELL_X, // Next cell along the X axis
	eCELL_Y, // Next cell along the Y axis
	eCELL_XY, // Next cell along both axes
	eCELL_COUNT
};
// Collection of 4 cells relevant to an entity
struct CellGroup
{
	CellCoord c[eCELL_COUNT]{ CellCoord(0u,0u), CellCoord(0u,0u), CellCoord(0u,0u), CellCoord(0u,0u) };
};
// Collection of 4 cells and XY offsets relevant to an entity
struct CellSpace
{
	CellCoord c[eCELL_COUNT]{ CellCoord(0u,0u), CellCoord(0u,0u), CellCoord(0u,0u), CellCoord(0u,0u) };
	btf32 offsetx;
	btf32 offsety;
};

#define PATH_NUM_NODES 64

namespace env
{
	#if !DEF_GRID
	
	struct EnvVert {
		m::Vector2 pos;
		btf32 h;
		m::Vector2 nor;
		btui32 neighbors[32];
		btui32 neighborcount;
	};

	// Triangle used for floor collision
	struct EnvTri {
		// Vertex indices
		btui32 a, b, c;
		// Slope for slide calculations
		m::Vector2 slope;
		// indices of the three neighboring triangles
		btID neighbors[3];
		// Which island do I belong to
		btui16 group;
		btui8 neighborcount;
		// test
		bool open_edge_ab;
		bool open_edge_bc;
		bool open_edge_ca;
		// shitty
		bool facing_up;
		bool vertical;
	};

	// Line segment used for wall collision
	struct EnvLineSeg {
		m::Vector2 pos_a; // Point position
		btf32 h_a_top; // Point height top
		btf32 h_a_bot; // Point height bottom
		m::Vector2 pos_b; // Point position
		btf32 h_b_top; // Point height top
		btf32 h_b_bot; // Point height bottom
		// collision variables
		btf32 csn_rotation;
		m::Vector2 csn_position;
		m::Vector2 csn_scale;
	};

	btui32 GetNumTris(WCoord coords);
	btf32 GetTriHeight(WCoord coords, btui32 index, btf32 pos_x, btf32 pos_y);
	bool GetTriExists(WCoord coords, btui32 index);
	void* GetC2Tri(WCoord coords, btui32 index);
	EnvTri* GetTri(WCoord coords, btui32 index);
	btui32 GetNumLines(WCoord coords);
	EnvLineSeg* GetLine(WCoord coords, btui32 index);

	struct EnvTriSurfaceSet {
		EnvTri* nearest_ceil_above;
		EnvTri* nearest_flor_above;
		EnvTri* nearest_ceil_below;
		EnvTri* nearest_flor_below;
		btf32 nearest_ceil_h_above;
		btf32 nearest_flor_h_above;
		btf32 nearest_ceil_h_below;
		btf32 nearest_flor_h_below;
	};
	void GetFloorsAndCeilings(CellSpace& csinf, btf32 in_height, EnvTriSurfaceSet* set);

	#endif

	// Environment flags
	namespace eflag
	{
		enum flag : btui32
		{
			EF_EMPTY = 0x0u,
			// Base collision
			EF_IMPASSABLE = (0x1u),
			EF_INVISIBLE = (0x1u << 0x1u),
			EF_2 = (0x1u << 0x2u),
			EF_FPP_HERE = (0x1u << 0x3u), // If the tile is to be treated as a round object like a tree
			// Shape information
			EF_TEMP_1 = (0x1u << 0x4u),
			EF_TEMP_2 = (0x1u << 0x5u),
			EF_TEMP_3 = (0x1u << 0x6u),
			EF_TEMP_4 = (0x1u << 0x7u),
			EF_TEMP_5 = (0x1u << 0x8u),
			EF_TEMP_6 = (0x1u << 0x9u),
			EF_TEMP_7 = (0x1u << 0xau),
			EF_TEMP_8 = (0x1u << 0xbu),
			// Misc.
			eBLOCK_LIGHT_SKY = (0x1u << 0xcu),
			EF_LIGHTSRC = (0x1u << 0xdu),
			EF_SPAWN_TEST = (0x1u << 0xeu),
			EF_SPAWN_ITEM_TEST = (0x1u << 0xfu),
			// Status effects
			EF_16 = (0x1u << 0x10u),
			EF_BURNING = (0x1u << 0x11u),
			EF_SMOKE = (0x1u << 0x12u),
			EF_POISON = (0x1u << 0x13u),
			EF_20 = (0x1u << 0x14u),
			EF_21 = (0x1u << 0x15u),
			EF_22 = (0x1u << 0x16u),
			EF_23 = (0x1u << 0x17u),
			EF_24 = (0x1u << 0x18u),
			EF_25 = (0x1u << 0x19u),
			EF_26 = (0x1u << 0x1au),
			EF_27 = (0x1u << 0x1bu),
			EF_28 = (0x1u << 0x1cu),
			EF_29 = (0x1u << 0x1du),
			EF_30 = (0x1u << 0x1eu),
			EF_31 = (0x1u << 0x1fu),

			// put some templates here with |
		};
	}

	struct node_coord
	{
		btui8 x, y;
		node_coord()
		{
			this->x = 0u; this->y = 0u;
		}
		node_coord(btui8 x, btui8 y)
		{
			this->x = x; this->y = y;
		}
	};

	enum NodePropDirection : btui8
	{
		eNORTH, eSOUTH, eEAST, eWEST,
	};

	// New structure is much easier to add new properties to saved versions
	struct EnvNode
	{
		eflag::flag flags[WORLD_SIZE][WORLD_SIZE];
		btui16 prop[WORLD_SIZE][WORLD_SIZE];
		NodePropDirection prop_dir[WORLD_SIZE][WORLD_SIZE];
		btui8 terrain_height[WORLD_SIZE][WORLD_SIZE]; // unuse this if possible
		btui8 terrain_material[WORLD_SIZE][WORLD_SIZE];
		btui8 water_height[WORLD_SIZE][WORLD_SIZE]; // this one's basically free i think
		btui8 terrain_height_ne[WORLD_SIZE][WORLD_SIZE];
		btui8 terrain_height_nw[WORLD_SIZE][WORLD_SIZE];
		btui8 terrain_height_se[WORLD_SIZE][WORLD_SIZE];
		btui8 terrain_height_sw[WORLD_SIZE][WORLD_SIZE];
		btID spawn_id[WORLD_SIZE][WORLD_SIZE];
	};

	extern EnvNode eCells;

	bool Get(btui32 x, btui32 y, eflag::flag bit);
	void Set(btui32 x, btui32 y, eflag::flag bit);
	void UnSet(btui32 x, btui32 y, eflag::flag bit);

	#if DEF_GRID
	void GetHeight(btf32& OUT_HEIGHT, CellSpace& CELL_SPACE);
	void GetSlope(btf32& OUT_SLOPE_X, btf32& OUT_SLOPE_Y, CellSpace& CELL_SPACE);
	#else
	// Get the height of the nearest ceiling above this point
	void GetNearestCeilingHeight(btf32& out_height, CellSpace& cell_space, btf32 in_height);
	// Get the height of the ground.
	// In_height represents the origin point so we can exist underneath other triangles
	void GetNearestSurfaceHeight(btf32& out_height, CellSpace& cell_space, btf32 in_height);
	void GetNearestSurfaceHeight(btf32& out_height, EnvTri** out_tri, CellSpace& cell_space, btf32 in_height);
	#endif

	struct LineTraceHit {
		m::Vector2 pos;
		btf32 h;
	};
	// returns true if it hits a wall
	bool LineTrace(btf32 x1, btf32 y1, btf32 x2, btf32 y2, btf32 height_a, btf32 height_b);
	bool LineTrace(btf32 x1, btf32 y1, btf32 x2, btf32 y2, btf32 height_a, btf32 height_b, LineTraceHit* out_hit);
	#if DEF_GRID
	// returns true, counter-intuitively, if it DOESNT hit anything
	bool LineTraceBh(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b);
	#endif

	void Tick();
	void DrawProps();
	void DrawTerrain();
	void DrawDebugGizmos(CellSpace* cs);
	void DrawTerrainDebug();

	// Load world setting from binary file
	void SaveBin();
	// Save world setting to binary file
	void LoadBin();

	void Clean();
	void GeneratePropMeshes();
	void GenerateTerrainMesh();

	void Free();
}

namespace path
{
	struct Path
	{
		#if DEF_GRID
		WCoord nodes[PATH_NUM_NODES];
		#else
		btf32 pos_x[PATH_NUM_NODES];
		btf32 pos_y[PATH_NUM_NODES];
		#endif
		btui8 len;
	};
	#if DEF_GRID
	bool PathFind(Path* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest);
	#else
	bool PathFind(Path* path, btf32 x, btf32 y, btf32 xDest, btf32 yDest);
	#endif
}

#endif
