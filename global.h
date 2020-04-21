#pragma once

// Compiler configuration definitions

// Alter the generic RPG engine into a multiplayer focused one optimised for the mech game
// this is done in the project configuration setting now
//#define DEF_NMP

//#define DEF_DEBUG_DISPLAY // Draw debug information
//#define DEF_MULTISAMPLE // TODO: chara shader doesnt work with multisampling for, er, some reason?
#define DEF_MULTISAMPLE_DEPTH 4
#define DEF_HDR
//#define DEF_LINEAR_FB
//#define DEF_DRAW_WIREFRAME
#define DEF_SMOOTH_FRAMERATE // Run the CPU a little hotter to make sure the framerate is as smooth as possible
#ifdef _MSC_VER
#define DEF_USE_CS // Use cute sound
#endif
//#define DEF_AUTO_RELOAD_ON_DEATH
//#define DEF_SPAWN_ON_START
//#define DEF_PERIODIC_SPAWN // temporary definition that enables random enemy spawning
//#define DEF_AUTOSAVE_ON_START // temporary definition that enables autosave
#define DEF_TERRAIN_USE_EROSION_TRIANGULATION
//#define DEF_3PP // Use third person perspective (for the purpose of testing the entity animation quality)
//#define DEF_OLDSKOOL // Use old rendering settings (nearest neighbor filtering, etc.)
#define DEF_CUSTOM_MIPMAP
//#define DEF_CUSTOM_MIPMAP_FOLIAGE_MIN
#define DEF_SHADOW_ALL_OBJECTS
//#define DEF_TB_MODE

#ifdef DEF_NMP
#define NUM_PLAYERS 2
#else
#define NUM_PLAYERS 2 // always 2 (co-op splitscreen)
#endif

// Any debug only defines
#ifdef _DEBUG
#define DEF_CONSOLE_OUTPUT
#else
#define NDEBUG
#endif

// Input
#define DEF_INPUT_MOUSE_1ST_PERSON
//#define DEF_INPUT_MOUSE_HIDDEN

#define SHADOW_RESOLUTION 1024
//#define SHADOW_RESOLUTION 2048
#define SHADOW_FAR 256.f
#define SHADOW_HALF 128.f
#define SHADOW_WIDTH 12.f

// World width and height in tiles
#define WORLD_SIZE 2048
#define WORLD_SIZE_MAXINT 2047
#define WORLD_SIZE_SQUARED (WORLD_SIZE * WORLD_SIZE)

//#define SCREEN_UPSCALE_THRESHOLD 2048u
#define SCREEN_UPSCALE_THRESHOLD 1664u

// Release version
#define VERSION_MAJOR 16u
#define VERSION_MINOR 1u
#define VERSION_COMMENT "prototype"

#define MD_MATRIX_COUNT 4u

// Buffer / ID definitions
#define BUF_SIZE 512
// TODO: Merge BUF_NULL and ID_NULL
#define BUF_NULL 0b1111111111111111
#define ID_NULL 0b1111111111111111

#define UI32_NULL 0b11111111111111111111111111111111

// Inventory definitions
#define INV_SIZE 64
#define INV_NULL 256 // inventory null value (is this even used?)

// Time to pass in seconds before one single tick
#ifdef DEF_TB_MODE
#define FRAME_TIME (1. / 60.)
#define FR_SPEED_MULT 0.5
#else
#define FRAME_TIME (1. / 30.)
#define FR_SPEED_MULT 1.0
#endif

// Terrain vertical precision per 1 unit
#define TERRAIN_HEIGHT_DIVISION 4.f

#define CEILING(x,y) (((x) + (y) - 1) / (y))

#ifdef __cplusplus
extern "C" {
	#endif

	#ifdef __GNUC__
	// Signed integers
	typedef signed char bti8;
	typedef signed short int bti16;
	typedef signed int bti32;
	typedef signed long long int bti64;
	// Unsigned integers
	typedef unsigned char btui8;
	typedef unsigned short int btui16;
	typedef unsigned int btui32;
	typedef unsigned long long btui64;
	// Floating points
	typedef float btf32;
	typedef double btf64;
	typedef long double btf96;
	// For specific uses
	typedef unsigned short int btID;
	#elif defined _MSC_VER
	// Signed integers
	typedef signed __int8 bti8;
	typedef signed __int16 bti16;
	typedef signed __int32 bti32;
	typedef signed __int64 bti64;
	// Unsigned integers
	typedef unsigned __int8 btui8;
	typedef unsigned __int16 btui16;
	typedef unsigned __int32 btui32;
	typedef unsigned __int64 btui64;
	// Floating points
	typedef float btf32;
	typedef double btf64;
	// For specific uses
	typedef unsigned __int16 btID;
	#else
	#error Using different c++ version than written for, please review! (global.h) 
	#endif

	typedef btui8 EntityType;
	#define ENTITY_TYPE_EDITOR_PAWN 0u
	#define ENTITY_TYPE_RESTING_ITEM 1u
	#define ENTITY_TYPE_CHARA 2u
	// Count the above
	#define ENTITY_TYPE_COUNT 3u

	typedef btui8 ItemType;
	#define ITEM_TYPE_MISC 0u
	#define ITEM_TYPE_EQUIP 1u
	#define ITEM_TYPE_WPN_MELEE 2u
	#define ITEM_TYPE_WPN_MATCHGUN 3u
	#define ITEM_TYPE_WPN_MAGIC 4u
	#define ITEM_TYPE_CONS 5u
	///*
	typedef btui8 AssetType;
	#define ASSET_NONE 0u
	#define ASSET_TEXTURE_FILE 1u
	#define ASSET_MESH_FILE 2u
	#define ASSET_MESHBLEND_FILE 3u
	#define ASSET_MESHDEFORM_FILE 4u
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
