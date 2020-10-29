#ifndef CFG_H
#define CFG_H

#include "global.h"

struct Config {
	//-------------------------------- INTERNALLY SET

	// third person toggle
	#if DEF_PROJECT == PROJECT_BC
	bool b3PP = true;
	#else
	bool b3PP = false;
	#endif

	//-------------------------------- VARIABLES LOADED FROM FILE

	unsigned int iFullscreen = 0u;
	bool bHost = false;
	bool bEditMode = false;
	bool bSplitScreen = false;
	bool bCrossHairs = true;

	// Config variables loaded from a file
	// Window size
	unsigned int iWinX = 640;
	unsigned int iWinY = 480;

	// Camera stuff
	float fCameraFOV = 80.f;
	float fCameraSensitivity = 0.f;
	float fCameraNearClip = 0.1f;
	float fCameraFarClip = 100.f;
	float fVolume = 0.5f;

	// Connection stuff
	char sConnAddr[HOSTNAME_MAX_LEN];
	unsigned int iPort = 1111;
	unsigned int iNumNWPlayers = 3u;
};
extern Config config;

namespace cfg
{
	void LoadCfg();
}

#endif