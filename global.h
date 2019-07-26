#pragma once

#include <stdint.h>

//compiler configuration definitions
#define DEF_OLDSKOOL // use old rendering settings (nearest neighbor filtering, etc.
//#define DEF_DEBUG_DISPLAY // draw debug information
#define DEF_3D // draw in perspective
//#define DEF_CAM_EXPERIMENTAL // various camera stuff
#define DEF_CAM_ROTATE // Use a rotating camera
//#define DEF_FAUX2D // use old perspective method (in shader)
#define DEF_BLIT_FRAME
//#define DEF_MULTISAMPLE
// Input
#define DEF_INPUT_MOUSE_1ST_PERSON
//#define DEF_INPUT_MOUSE_HIDDEN
//#define DEF_SDL_USE_ZBUF

#ifndef DEF_3D
#define ORTHOSIZE 6.f
#endif

// for faux2d rendering
#define TILE_RENDER_SIZE_X 64.f
//#define TILE_RENDER_SIZE_Y 48.f
#define TILE_RENDER_SIZE_Y 64.f
#define VERTICAL_UPSCALE 1.25f

#define BUF_SIZE 512

#define BUF_PLAYER 8
//#define BUF_SIZE 256
#define BUF_NULL 513
#define BUF_PLAYER_ALLOC 128

//#define ID_NULL 255
#define ID_NULL 65535

#define FRAME_TIME 1. / 30.
//#define FRAME_TIME 1. / 60.

#define TERRAIN_HEIGHT_DIVISION 8.f

//versioning: 0x + hex number eg. 0x04f0a7
#define VERSION_MAJOR 0x0
#define VERSION_MINOR 0x9
#define VERSION_EDITOR_MAJOR 0x0
#define VERSION_EDITOR_MINOR 0x9
#define VERSION_SERVER_MAJOR 0x0
#define VERSION_SERVER_MINOR 0x1

#ifdef __cplusplus
extern "C" {
	#endif

	// Signed integers
	typedef int8_t bti8;
	typedef int16_t bti16;
	typedef int32_t bti32;
	typedef int64_t bti64;

	// Unsigned integers
	typedef uint8_t btui8;
	typedef uint16_t btui16;
	typedef uint32_t btui32;
	typedef uint64_t btui64;
	// Floating points
	typedef float btf32;
	typedef double btf64;

	//typedef uint16_t id_t;
	typedef unsigned short btID;

	#ifdef __cplusplus
}
#endif