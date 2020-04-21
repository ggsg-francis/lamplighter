#pragma once

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

namespace path
{
	struct PathNode
	{
		btcoord x, y;
	};
	void PathFind(void* path, btcoord x, btcoord y, btcoord xDest, btcoord yDest);
}

namespace env
{
	// Environment flags
	namespace eflag
	{
		enum flag : btui32
		{
			eNULL = 0x0u,
			// Base collision
			eIMPASSABLE = (0x1u << 0x0u),
			EF_BASE_1 = (0x1u << 0x1u),
			NODE_FLAG_BASE_2 = (0x1u << 0x2u),
			EF_CYLINDRIC = (0x1u << 0x3u), // If the tile is to be treated as a round object like a tree
			// Edge collision
			eSurfN = (0x1u << 0x4u),
			eSurfS = (0x1u << 0x5u),
			eSurfE = (0x1u << 0x6u),
			eSurfW = (0x1u << 0x7u),
			// Convex corner collision
			eCorOutNE = (0x1u << 0x8u),
			eCorOutNW = (0x1u << 0x9u),
			eCorOutSE = (0x1u << 0xau),
			eCorOutSW = (0x1u << 0xbu),
			// Unused flag spaces (reserved for special collision, such as diagonal edges?)
			eBLOCK_LIGHT_SKY = (0x1u << 0xcu),
			EF_LIGHTSRC = (0x1u << 0xdu),
			EF_SPAWN_TEST = (0x1u << 0xeu),
			EF_SPAWN_ITEM_TEST = (0x1u << 0xfu),
			// Status effects
			EF_UNUSED_16 = (0x1u << 0x10u),
			EF_BURNING = (0x1u << 0x11u),
			EF_SMOKE = (0x1u << 0x12u),
			EF_POISON = (0x1u << 0x13u),
			EF_20 = (0x1u << 0x14u),
			EF_21 = (0x1u << 0x15u),
			EF_22 = (0x1u << 0x16u),
			EF_23 = (0x1u << 0x17u),
			// Nothing much
			EF_24 = (0x1u << 0x18u),
			EF_25 = (0x1u << 0x19u),
			EF_26 = (0x1u << 0x1au),
			EF_FOODSRC = (0x1u << 0x1bu),
			// Meatstuff
			EF_28 = (0x1u << 0x1cu),
			EF_CONTAINS_MEAT = (0x1u << 0x1du),
			EF_30 = (0x1u << 0x1eu),
			EF_PROPAGATES_MEAT = (0x1u << 0x1fu),
			// 32 bits (4 bytes)
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
		btui8 terrain_height[WORLD_SIZE][WORLD_SIZE];
		btui8 terrain_material[WORLD_SIZE][WORLD_SIZE];
		btui8 water_height[WORLD_SIZE][WORLD_SIZE];
	};

	extern EnvNode eCells;

	bool Get(btui32 x, btui32 y, eflag::flag bit);
	void Set(btui32 x, btui32 y, eflag::flag bit);
	void UnSet(btui32 x, btui32 y, eflag::flag bit);

	void GetHeight(btf32& OUT_HEIGHT, CellSpace& CELL_SPACE);

	bool LineTrace_Bresenham(int x1, int y1, int x2, int y2, btf32 height_a, btf32 height_b);

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
}