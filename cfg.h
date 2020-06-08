#ifndef CFG_H
#define CFG_H

#include "global.h"

namespace cfg
{
	// Is fullscreen
	extern unsigned int iFullscreen;
	// In the case that this is the networked mode, take the server role
	extern bool bHost;
	// Run as editor
	extern bool bEditMode;
	// Use 2-player splitscreen mode (vs singleplayer mode)
	extern bool bSplitScreen;
	// Draw crosshairs
	extern bool bCrossHairs;

	//config variables loaded from a file
	//window size
	extern unsigned int iWinX;
	extern unsigned int iWinY;

	extern unsigned int iPort;

	//camera stuff
	extern float fCameraFOV;
	extern float fCameraSensitivity;
	extern float fCameraNearClip;
	extern float fCameraFarClip;

	extern char sConnAddr[HOSTNAME_MAX_LEN];

	void LoadCfg();
}

#endif