#ifndef CFG_H
#define CFG_H

#include "global.h"

struct Config {
	// INTERNALLY SET

	// third person toggle
	bool b3PP = true;

	// VARIABLES LOADED FROM FILE

	unsigned int iFullscreen = 0u;
	bool bHost = false;
	bool bEditMode = false;
	bool bSplitScreen = false;
	bool bCrossHairs = true;

	// Config variables loaded from a file
	// Window size
	unsigned int iWinX = 640;
	unsigned int iWinY = 480;

	unsigned int iPort = 1111;

	// Camera stuff
	float fCameraFOV = 80.f;
	float fCameraSensitivity = 0.f;
	float fCameraNearClip = 0.1f;
	float fCameraFarClip = 100.f;

	char sConnAddr[HOSTNAME_MAX_LEN];
};
extern Config config;

namespace cfg
{
	void LoadCfg();
}

#endif