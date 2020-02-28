#pragma once

//#include <stdint.h>

// Compiler configuration definitions
//#define DEF_OLDSKOOL // Use old rendering settings (nearest neighbor filtering, etc.
//#define DEF_DEBUG_DISPLAY // Draw debug information
//#define DEF_MULTISAMPLE
#define DEF_MULTISAMPLE_DEPTH 4
#define DEF_HDR
//#define DEF_LINEAR_FB
//#define DEF_DRAW_WIREFRAME
#define DEF_SMOOTH_FRAMERATE
// use cute sound
#define DEF_USE_CS
#define DEF_PERIODIC_SPAWN

// Any debug only defines
#ifdef _DEBUG
#define DEF_CONSOLE_OUTPUT
#endif

// Input
#define DEF_INPUT_MOUSE_1ST_PERSON
//#define DEF_INPUT_MOUSE_HIDDEN

//#define SHADOW_WIDTH 1024
//#define SHADOW_HEIGHT 1024
#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048

//world width and height in tiles
#define WORLD_SIZE 2048
#define WORLD_SIZE_SQUARED WORLD_SIZE * WORLD_SIZE
//values used when accessing the world bit vector
#define WORLD_BYTE_DEPTH 16
#define WORLD_BIT_DEPTH WORLD_BYTE_DEPTH * 8

//#define SCREEN_UPSCALE_THRESHOLD 1280u
#define SCREEN_UPSCALE_THRESHOLD 2048u

#define LIGHT_FAR 256.f
#define LIGHT_HALF 128.f
#define LIGHT_WIDTH 12.f

// Program Version
#define VERSION_MAJOR 15u

#define VERSION_SERVER_MAJOR 0u
#define VERSION_SERVER_MINOR 14u
#define VERSION_COMMENT "PSI"

#define MD_MATRIX_COUNT 4u

// Buffer / ID definitions
#define BUF_SIZE 512
// Merge BUF_NULL and ID_NULL
//#define BUF_NULL 513
#define BUF_NULL 0b1111111111111111
#define BUF_PLAYER_ALLOC 128
//#define ID_NULL 65535
#define ID_NULL 0b1111111111111111

#define INV_SIZE 64
#define INV_NULL 256 // ????

// Time to pass in seconds before one single tick
#define FRAME_TIME 1. / 30.
//#define FRAME_TIME 1. / 60.

// Terrain vertical precision per 1 unit
#define TERRAIN_HEIGHT_DIVISION 4.f

#define CEILING(x,y) (((x) + (y) - 1) / (y))

#ifdef __cplusplus
extern "C" {
	#endif

	#if _MSC_VER == 1900
	// Signed integers
	typedef signed char bti8;
	typedef short bti16;
	typedef int bti32;
	typedef long long bti64;
	// Unsigned integers
	typedef unsigned char btui8;
	typedef unsigned short btui16;
	typedef unsigned int btui32;
	typedef unsigned long long btui64;
	// Floating points
	typedef float btf32;
	typedef double btf64;
	// For specific uses
	typedef unsigned short btID;
	#else
	#error Using different c++ version than written for, please review! (global.h) 
	#endif

	typedef btui8 EntityType;
	#define ENTITY_TYPE_EDITOR_PAWN 0ui8
	#define ENTITY_TYPE_RESTING_ITEM 1ui8
	#define ENTITY_TYPE_CHARA 2ui8
	// Count the above
	#define ENTITY_TYPE_COUNT 3u

	typedef btui8 ItemType;
	#define ITEM_TYPE_MISC 0ui8
	#define ITEM_TYPE_EQUIP 1ui8
	#define ITEM_TYPE_WPN_MELEE 2ui8
	#define ITEM_TYPE_WPN_MATCHGUN 3ui8
	#define ITEM_TYPE_WPN_MAGIC 4ui8
	#define ITEM_TYPE_CONS 5ui8
	///*
	typedef btui8 AssetType;
	#define ASSET_NONE 0ui8
	#define ASSET_TEXTURE_FILE 1ui8
	#define ASSET_MESH_FILE 2ui8
	#define ASSET_MESHBLEND_FILE 3ui8
	#define ASSET_MESHDEFORM_FILE 4ui8
	//*/

	/*
	enum EntityType : btui8
	{
	// whatever else
	ENTITY_EDITOR_PAWN = 7ui8,
	// game use
	ENTITY_RESTING_ITEM = 8ui8,
	ENTITY_CHARA = 9ui8,
	};
	*/
	/*
	enum ItemType : btui8
	{
		ITEM_ROOT,
		ITEM_EQUIP,
		ITEM_WPN_MELEE,
		ITEM_WPN_MATCHGUN,
		ITEM_WPN_MAGIC,
		ITEM_CONS,
	};
	enum AssetType : btui8
	{
		ASSET_NONE,
		ASSET_TEXTURE_FILE,
		ASSET_MESH_FILE,
		ASSET_MESHBLEND_FILE,
		ASSET_MESHDEFORM_FILE,
	};
	//*/

	#ifdef __cplusplus
}
#endif