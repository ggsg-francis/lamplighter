#pragma once

#include "memory.hpp"

//world width and height in tiles
#define WORLD_SIZE 2048
#define WORLD_SIZE_SQUARED WORLD_SIZE * WORLD_SIZE
//values used when accessing the world bit vector
#define WORLD_BYTE_DEPTH 16
#define WORLD_BIT_DEPTH WORLD_BYTE_DEPTH * 8

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
			EF_UNUSED_13 = (0x1u << 0xdu),
			EF_UNUSED_14 = (0x1u << 0xeu),
			EF_UNUSED_15 = (0x1u << 0xfu),
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

	struct node_v001
	{
		eflag::flag flags = eflag::eNULL;

		btui16 prop = 0ui16;
		btui8 mat = 0ui8;
		btui8 height = 0ui8;

		btui8 num_01 = 0ui8;
		btui8 num_02 = 0ui8;
		btui8 num_03 = 0ui8;
		btui8 num_04 = 0ui8;

		btui8 water_height = 0ui8;
		btui8 num_06 = 0ui8;
		btui8 num_07 = 0ui8;
		btui8 num_08 = 0ui8;
	};

	extern node_v001 eCells[WORLD_SIZE][WORLD_SIZE];

	bool Get(uint x, uint y, eflag::flag bit);
	void Set(uint x, uint y, eflag::flag bit);
	void UnSet(uint x, uint y, eflag::flag bit);

	//void GetHeight(btf32& OUT_HEIGHT, CellGroup& CELL_GROUP);
	void GetHeight(btf32& OUT_HEIGHT, CellSpace& CELL_SPACE);

	//void Tick();

	//load world setting from binary file
	void SaveBin();
	//save world setting to binary file
	void LoadBin();

	void GeneratePhysicsSurfaces();
}