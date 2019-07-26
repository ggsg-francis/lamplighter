#pragma once

#include "memory.hpp"

//world width and height in tiles
#define WORLD_SIZE 128
#define WORLD_SIZE_SQUARED WORLD_SIZE * WORLD_SIZE
//values used when accessing the world bit vector
#define WORLD_BYTE_DEPTH 16
#define WORLD_BIT_DEPTH WORLD_BYTE_DEPTH * 8

// Environment model types

#define EMT_EMPTY 0
#define EMT_PLACEHOLDER_1 1
#define EMT_PLACEHOLDER_2 2
#define EMT_PLACEHOLDER_3 3
#define EMT_WALL_STR_N 4
#define EMT_WALL_STR_S 5
#define EMT_WALL_STR_E 6
#define EMT_WALL_STR_W 7
#define EMT_WALL_COR_OUT_NE 8
#define EMT_WALL_COR_OUT_NW 9
#define EMT_WALL_COR_OUT_SE 10
#define EMT_WALL_COR_OUT_SW 11
#define EMT_WALL_COR_IN_NE 12
#define EMT_WALL_COR_IN_NW 13
#define EMT_WALL_COR_IN_SE 14
#define EMT_WALL_COR_IN_SW 15

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
			EF_UNUSED_12 = (0x1u << 0xcu),
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

	namespace nbit
	{
		enum nsew : btui8
		{
			N_,
			S_,
			E_,
			W_,
		};

		enum node_bit : btui8
		{
			N = 0b00000000,
			S = 0b00000001,
			E = 0b00000010,
			W = 0b00000011,

			N2 = 0b00000000,
			S2 = 0b00000100,
			E2 = 0b00001000,
			W2 = 0b00001100,

			N3 = 0b00000000,
			S3 = 0b00010000,
			E3 = 0b00100000,
			W3 = 0b00110000,

			N4 = 0b00000000,
			S4 = 0b01000000,
			E4 = 0b10000000,
			W4 = 0b11000000,

			MASK1 = 0b00000011,
			MASK2 = 0b00001100,
			MASK3 = 0b00110000,
			MASK4 = 0b11000000,

			CLEAR1 = 0b11111100,
			CLEAR2 = 0b11110011,
			CLEAR3 = 0b11001111,
			CLEAR4 = 0b00111111,

			//MAX = 0b11111111,

			//NT = S3 >> 4u,
			//test = 0b00001100,
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

	nbit::node_bit get_node_bit(btui8 srcx, btui8 srcy, btui8 dstx, btui8 dsty);

	node_coord get_node_from(btui8 srcx, btui8 srcy, btui8 dstx, btui8 dsty);

	struct node_v001
	{
		enum surface : btui8
		{
			ground,
			water,
			tar,
			grass,
			undergrowth,
		};

		enum material : btui8
		{
			soft,
			med,
			hard,
		};

		eflag::flag flags = eflag::eNULL;

		surface surftype = ground;
		btui8 model_type = 0;
		material mat = med;
		bti8 height = 0;

		btui8 num_05 = 0;
		btui8 num_06 = 0;
		btui8 num_07 = 0;
		btui8 num_08 = 0;

		btui8 num_09 = 0;
		btui8 foodlevel = 0;
		btui8 num_11 = 0;
		btui8 meatlevel = 0;
	};

	struct path_node
	{
		//temp
		uint8_t frontier_index = -1;
		int from_x = -1, from_y = -1;
		//float f = 0.f, g = 0.f, h = 0.f;
		bool open = true;
	};

	extern node_v001 nodes[WORLD_SIZE][WORLD_SIZE];
	extern path_node pnodes[WORLD_SIZE][WORLD_SIZE];

	bool Get(uint x, uint y, eflag::flag bit);
	void Set(uint x, uint y, eflag::flag bit);
	void UnSet(uint x, uint y, eflag::flag bit);

	//void Tick();

	//load world setting from binary file
	void SaveBin();
	//save world setting to binary file
	void LoadBin();

	void GeneratePaths();
	void GeneratePhysicsSurfaces();
	void GenerateModelTypes();
}

namespace ltr
{
	extern unsigned int tilesTemplate[8][8];
}