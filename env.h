#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "memory.hpp"
#include "maths.hpp"

// Enumerator representing the intended function of the 4 cell coordinates used
enum eCell : lui32
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
	lf32 offsetx;
	lf32 offsety;
};

#define PATH_NUM_NODES 64

namespace env
{
	#if !DEF_GRID
	
	struct EnvVert { // 256 bytes (should be reduced when we ditch vert neighbors)
		m::Vector2 pos;
		lf32 h;
		m::Vector2 nor;
		lui32 neighbors[32];
		lui32 neighborcount;
	
		lui64 buffer1[13];
	};

	enum EnvFloorMat : lui8 {
		FLOOR_STANDARD,
		FLOOR_WATER,
		FLOOR_TAR,
		FLOOR_QUICKSAND,
		FLOOR_LAVA,
		FLOOR_ICE,
		FLOOR_ACID,
	};
	// Triangle metadata
	struct EnvTriMeta { // 64 bytes (I want to leave a lot of room for new things okay)
		EnvFloorMat floorMaterial;
		bool filler[63];
	};
	// Triangle used for floor collision
	struct EnvTri { // 48 bytes
		// Vertex indices
		lui32 verts[3];
		// Slope for slide calculations
		m::Vector2 slope;
		// indices of the three neighboring triangles
		ID16 neighbors[3];
		// Which island do I belong to
		lui16 group;
		// How many neighbors do I have (1-3)
		lui8 neighborcount;
		// test
		bool open_edge_ab;
		bool open_edge_bc;
		bool open_edge_ca;
		// shitty
		bool facing_up;
		bool vertical;
		
		bool filler[11];
	};
	
	// Line segment used for wall collision
	struct EnvLineSeg { // 64 bytes
		m::Vector2 pos_a; // Point position
		lf32 h_a_top; // Point height top
		lf32 h_a_bot; // Point height bottom
		m::Vector2 pos_b; // Point position
		lf32 h_b_top; // Point height top
		lf32 h_b_bot; // Point height bottom
		// collision variables
		lf32 csn_rotation;
		m::Vector2 csn_position;
		m::Vector2 csn_scale;
	
		bool filler[12];
	};

	lui32 GetNumTris(WCoord coords);
	lf32 GetTriHeight(WCoord coords, lui32 index, lf32 pos_x, lf32 pos_y);
	bool GetTriExists(WCoord coords, lui32 index);
	void* GetC2Tri(WCoord coords, lui32 index);
	EnvTri* GetTri(WCoord coords, lui32 index);
	lui32 GetNumLines(WCoord coords);
	EnvLineSeg* GetLine(WCoord coords, lui32 index);

	struct EnvTriSurfaceSet {
		EnvTri* nearest_ceil_above;
		EnvTri* nearest_flor_above;
		EnvTri* nearest_ceil_below;
		EnvTri* nearest_flor_below;
		lf32 nearest_ceil_h_above;
		lf32 nearest_flor_h_above;
		lf32 nearest_ceil_h_below;
		lf32 nearest_flor_h_below;
	};
	void GetFloorsAndCeilings(CellSpace& csinf, lf32 in_height, EnvTriSurfaceSet* set);

	#endif

	// Environment flags
	namespace eflag
	{
		enum flag : lui32
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
		lui8 x, y;
		node_coord()
		{
			this->x = 0u; this->y = 0u;
		}
		node_coord(lui8 x, lui8 y)
		{
			this->x = x; this->y = y;
		}
	};

	#if DEF_GRID
	enum NodePropDirection : lui8
	{
		eNORTH, eSOUTH, eEAST, eWEST,
	};
	// New structure is much easier to add new properties to saved versions
	struct EnvNode
	{
		eflag::flag flags[WORLD_SIZE][WORLD_SIZE];
		lui16 prop[WORLD_SIZE][WORLD_SIZE];
		NodePropDirection prop_dir[WORLD_SIZE][WORLD_SIZE];
		lui8 terrain_height[WORLD_SIZE][WORLD_SIZE]; // unuse this if possible
		lui8 terrain_material[WORLD_SIZE][WORLD_SIZE];
		lui8 water_height[WORLD_SIZE][WORLD_SIZE]; // this one's basically free i think
		lui8 terrain_height_ne[WORLD_SIZE][WORLD_SIZE];
		lui8 terrain_height_nw[WORLD_SIZE][WORLD_SIZE];
		lui8 terrain_height_se[WORLD_SIZE][WORLD_SIZE];
		lui8 terrain_height_sw[WORLD_SIZE][WORLD_SIZE];
		ID16 spawn_id[WORLD_SIZE][WORLD_SIZE];
	};
	extern EnvNode eCells;
	#endif

	#if DEF_GRID
	bool Get(lui32 x, lui32 y, eflag::flag bit);
	void Set(lui32 x, lui32 y, eflag::flag bit);
	void UnSet(lui32 x, lui32 y, eflag::flag bit);
	void GetHeight(lf32& OUT_HEIGHT, CellSpace& CELL_SPACE);
	void GetSlope(lf32& OUT_SLOPE_X, lf32& OUT_SLOPE_Y, CellSpace& CELL_SPACE);
	#else
	// Get the height of the nearest ceiling above this point
	void GetNearestCeilingHeight(lf32& out_height, CellSpace& cell_space, lf32 in_height);
	// Get the height of the ground.
	// In_height represents the origin point so we can exist underneath other triangles
	void GetNearestSurfaceHeight(lf32& out_height, CellSpace& cell_space, lf32 in_height);
	void GetNearestSurfaceHeight(lf32& out_height, EnvTri** out_tri, CellSpace& cell_space, lf32 in_height);
	#endif

	struct LineTraceHit {
		m::Vector2 pos;
		lf32 h;
	};
	// returns true if it hits a wall
	bool LineTrace(lf32 x1, lf32 y1, lf32 x2, lf32 y2, lf32 height_a, lf32 height_b);
	bool LineTrace(lf32 x1, lf32 y1, lf32 x2, lf32 y2, lf32 height_a, lf32 height_b, LineTraceHit* out_hit);
	#if DEF_GRID
	// returns true, counter-intuitively, if it DOESNT hit anything
	bool LineTraceBh(int x1, int y1, int x2, int y2, lf32 height_a, lf32 height_b);
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
	#if !DEF_GRID
	void GeneratePhysMesh();
	#endif

	//void Clean();
	#if DEF_GRID
	void GeneratePropMeshes();
	void GenerateTerrainMesh();
	#endif

	void Free();
}

namespace path
{
	struct Path
	{
		#if DEF_GRID
		WCoord nodes[PATH_NUM_NODES];
		#else
		lf32 pos_x[PATH_NUM_NODES];
		lf32 pos_y[PATH_NUM_NODES];
		#endif
		lui8 len;
	};
	#if DEF_GRID
	bool PathFind(Path* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest);
	#else
	bool PathFind(Path* path, lf32 x, lf32 y, lf32 xDest, lf32 yDest);
	#endif
}

#endif
