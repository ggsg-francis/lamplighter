#ifndef GLOBAL_H
#define GLOBAL_H

#include "global_project.h"

// Compiler configuration definitions

// this is done in the project configuration setting now
//#define DEF_NMP

#ifdef DEF_NMP
#define DEF_V_NMP 1
#else
#define DEF_V_NMP 0
#endif
// Release version
#define VERSION_MAJOR 23u // Major version should be incremented any time save compatibility is broken
#define VERSION_MINOR 1 // Minor version should be incremented for every new release
// This version number basically says which game this is
#define VERSION_PROJECT (\
DEF_PROJECT |\
(DEF_V_NMP << 8u) |\
(DEF_GRID << 9u) |\
(DEF_AUTOSAVE_ON_START << 10u) |\
(DEF_AUTO_RELOAD_ON_DEATH << 11u) |\
(DEF_AUTOAIM << 12u) |\
(DEF_LONGJUMP << 13u) |\
(DEF_WALL_SLIDE_PRESERVE_SPEED << 14u) |\
(DEF_NPC_INFINITE_CONS << 15u) |\
(DEF_PVP << 16u) |\
(DEF_SPAWN_ON_INIT << 17u) |\
(DEF_SPAWN_NPC << 18u)\
)

#define DEF_USE_CS // Use cute sound

#ifdef DEF_NMP
#define NUM_PLAYERS 4 // danger zone
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

#if DEF_PROJECT == PROJECT_EX
//#define SCREEN_UPSCALE_THRESHOLD 1024u
#define SCREEN_UPSCALE_THRESHOLD 1664u
#elif DEF_PROJECT == PROJECT_BC
#define SCREEN_UPSCALE_THRESHOLD 640u
#endif
#define SCREEN_POSTPROCESS_DOWNSAMPLE_DIVISION 4

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

// https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

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
	typedef signed char li8;
	typedef signed short int li16;
	typedef signed int li32;
	typedef signed long long int li64;
	// Unsigned integers
	typedef unsigned char lui8;
	typedef unsigned short int lui16;
	typedef unsigned int lui32;
	typedef unsigned long long lui64;
	// Floating points
	typedef float lf32;
	typedef double lf64;
	typedef long double lf96;
	// For specific uses
	typedef unsigned short int lid;
	#elif defined _MSC_VER
	// Signed integers
	typedef signed __int8 li8;
	typedef signed __int16 li16;
	typedef signed __int32 li32;
	typedef signed __int64 li64;
	// Unsigned integers
	typedef unsigned __int8 lui8;
	typedef unsigned __int16 lui16;
	typedef unsigned __int32 lui32;
	typedef unsigned __int64 lui64;
	// Floating points
	typedef float lf32;
	typedef double lf64;
	// For specific uses
	typedef lui16 lid;
	#else
	#error Using different c++ compiler than written for, please review! (global.h)
	#endif

	typedef struct WCoord {
		lui16 x;
		lui16 y;
	} WCoord;

	// TODO: these should probably go to their respective places too

	typedef lui8 ItemType;
	#define ITEM_TYPE_MISC 0u
	#define ITEM_TYPE_EQUIP 1u
	#define ITEM_TYPE_WPN_MELEE 2u
	#define ITEM_TYPE_WPN_MATCHGUN 3u
	#define ITEM_TYPE_WPN_MAGIC 4u
	#define ITEM_TYPE_CONS 5u
	#define ITEM_TYPE_COUNT 6u

	typedef lui8 ActivatorType;
	#define ACTIVATOR_TYPE_JUNK 0u
	#define ACTIVATOR_TYPE_COUNT 1u

	typedef lui8 AssetType;
	#define ASSET_NONE 0u
	#define ASSET_TEXTURE_FILE 1u
	#define ASSET_MESH_FILE 2u
	#define ASSET_MESHBLEND_FILE 3u
	#define ASSET_MESHDEFORM_FILE 4u
	#define ASSET_SOUNDWAV_FILE 5u

	#include <float.h>
	inline void SetFP() {
		lui32 current_word = 0;
		//_controlfp_s(&current_word, _DN_SAVE, _MCW_DN); // Set denormal (don't know what this does)
		//_controlfp_s(&current_word, _PC_24, _MCW_PC); // Set precision control (can't do on x64)
		_controlfp_s(&current_word, _RC_NEAR, _MCW_RC); // Set rounding control
	}

	#ifdef __cplusplus
}
#endif

#endif
