#pragma once

//#include <stdint.h>

// Compiler configuration definitions
//#define DEF_OLDSKOOL // Use old rendering settings (nearest neighbor filtering, etc.
//#define DEF_DEBUG_DISPLAY // Draw debug information
#define DEF_BLIT_FRAME
//#define DEF_MULTISAMPLE
//#define DEF_MULTISAMPLE_DEPTH 4
#define DEF_HDR
// Input
#define DEF_INPUT_MOUSE_1ST_PERSON
//#define DEF_INPUT_MOUSE_HIDDEN

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
//#define SHADOW_WIDTH 2048
//#define SHADOW_HEIGHT 2048

// Program Version
#define VERSION_MAJOR 0u
#define VERSION_MINOR 13u
#define VERSION_BUILD 2u
#define VERSION_SERVER_MAJOR 0u
#define VERSION_SERVER_MINOR 13u
#define VERSION_COMMENT "test"

// Buffer / ID definitions
#define BUF_SIZE 512
#define BUF_NULL 513
#define BUF_PLAYER_ALLOC 128
//#define ID_NULL 65535
#define ID_NULL 0b1111111111111111

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
	#error New c++ version not accounted for! Please check and add the new compiler version to the fundamental type definitions in global.h!
	#endif

	#ifdef __cplusplus
}
#endif