#ifndef GLOBAL_H
#define GLOBAL_H

#include "global_project.h"

// Compiler configuration definitions

// Alter the generic RPG engine into a multiplayer focused one optimised for the mech game
// this is done in the project configuration setting now
//#define DEF_NMP

#ifdef _MSC_VER
#define DEF_USE_CS // Use cute sound
#endif

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

#define SHADOW_RESOLUTION 1024
//#define SHADOW_RESOLUTION 2048
#define SHADOW_FAR 256.f
#define SHADOW_HALF 128.f
#define SHADOW_WIDTH 12.f

// World width and height in tiles
#define WORLD_SIZE 2048
#define WORLD_SIZE_MAXINT 2047
//#define WORLD_SIZE 128
//#define WORLD_SIZE_MAXINT 127
#define WORLD_SIZE_OLD 2048
#define WORLD_SIZE_MAXINT_OLD 2047
#define WORLD_SIZE_SQUARED (WORLD_SIZE * WORLD_SIZE)

//#define SCREEN_UPSCALE_THRESHOLD 1024u
#define SCREEN_UPSCALE_THRESHOLD 1664u
#define SCREEN_POSTPROCESS_DOWNSAMPLE_DIVISION 4

// Release version
#define VERSION_MAJOR 17u
#define VERSION_MINOR 0u
#define VERSION_COMMENT "prototype"

#define MD_MATRIX_COUNT 4u

#define ARCHIVE_FILENAME "_ARCHIVE"
#define ARCHIVE_DATA_FILENAME "_ARCHIVE_DATA"
#define ARCHIVE_C_FILENAME "../archive_data.h"
#define WORLD_FILENAME "_WORLD"
#define ARCHIVE_MAX_LOADED_DATA 0b00000010000000000000000000000000

// Buffer / ID definitions
#define BUF_SIZE 512
// TODO: Merge BUF_NULL and ID_NULL
#define BUF_NULL 0b1111111111111111
#define ID_NULL 0b1111111111111111

#define UI32_NULL 0b11111111111111111111111111111111

// Inventory definitions
#define INV_SIZE 64
#define INV_NULL 256 // inventory null value (is this even used?)

#define HEAD_TURN_SPEED 8.f

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
#define TERRAIN_UV_SCALE 0.125f

#define CEILING(x,y) (((x) + (y) - 1) / (y))

// Maximum IPv4 size: 576 bytes

#define HOSTNAME_MAX_LEN 22

#define forceinline __forceinline

#if defined _MSC_VER
#define PACKED_STRUCT(name) __pragma(pack(push,1)) struct name __pragma(pack(pop))
#elif defined(__GNUC__)
#define PACKED_STRUCT(name) struct __attribute__((packed)) name
#endif

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
	#error Using different c++ compiler than written for, please review! (global.h)
	#endif

	typedef struct WCoord {
		btui16 x;
		btui16 y;
	} WCoord;

	#define ENTITY_TYPE_NULL 255u

	typedef btui8 EntityType;
	#define ENTITY_TYPE_EDITOR_PAWN 0u
	#define ENTITY_TYPE_RESTING_ITEM 1u
	#define ENTITY_TYPE_ACTOR 2u
	#define ENTITY_TYPE_COUNT 3u

	typedef btui8 ItemType;
	#define ITEM_TYPE_MISC 0u
	#define ITEM_TYPE_EQUIP 1u
	#define ITEM_TYPE_WPN_MELEE 2u
	#define ITEM_TYPE_WPN_MATCHGUN 3u
	#define ITEM_TYPE_WPN_MAGIC 4u
	#define ITEM_TYPE_CONS 5u
	#define ITEM_TYPE_COUNT 6u

	typedef btui8 ActivatorType;
	#define ACTIVATOR_TYPE_JUNK 0u
	#define ACTIVATOR_TYPE_COUNT 1u

	typedef btui8 AssetType;
	#define ASSET_NONE 0u
	#define ASSET_TEXTURE_FILE 1u
	#define ASSET_MESH_FILE 2u
	#define ASSET_MESHBLEND_FILE 3u
	#define ASSET_MESHDEFORM_FILE 4u
	#define ASSET_SOUNDWAV_FILE 5u

	#ifdef __cplusplus
}
#endif

#endif
