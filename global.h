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

//versioning: 0x + hex number eg. 0x04f0a7
#define VERSION_MAJOR 0x0
#define VERSION_MINOR 0x9
#define VERSION_EDITOR_MAJOR 0x0
#define VERSION_EDITOR_MINOR 0x9
#define VERSION_SERVER_MAJOR 0x0
#define VERSION_SERVER_MINOR 0x1

// Buffer / ID definitions
#define BUF_SIZE 512
#define BUF_NULL 513
#define BUF_PLAYER_ALLOC 128
#define ID_NULL 65535

// Time to pass in seconds before one single tick
#define FRAME_TIME 1. / 30.

// Terrain vertical precision per 1 unit
#define TERRAIN_HEIGHT_DIVISION 4.f

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