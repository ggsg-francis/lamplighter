#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "memory.hpp"

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

#define PATH_NUM_NODES 32

namespace path
{
	struct Path
	{
		WCoord nodes[PATH_NUM_NODES];
		btui8 len;
	};
	bool PathFind(Path* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest);
}

namespace env
{
	btui32 GetNumTris(WCoord coords);
	btf32 GetTriHeight(WCoord coords, btui32 index, btf32 pos_x, btf32 pos_y);
	bool GetTriExists(WCoord coords, btui32 index);
	void* GetC2Tri(WCoord coords, btui32 index);

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
			EF_3 = (0x1u << 0x3u), // If the tile is to be treated as a round object like a tree
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

	void GetHeight(btf32& OUT_HEIGHT, CellSpace& CELL_SPACE);
	void GetSlope(btf32& OUT_SLOPE_X, btf32& OUT_SLOPE_Y, CellSpace& CELL_SPACE);

	bool LineTraceBh(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b);

	void Tick();
	void DrawProps();
	void DrawTerrain();
	void DrawTerrainDebug();

	// Load world setting from binary file
	void SaveBin();
	// Save world setting to binary file
	void LoadBin();

	void Clean();
	void GeneratePropMeshes();
	void GenerateTerrainMesh();
	void GenerateTerrainMeshEditor();

	void Free();
}

#endif
