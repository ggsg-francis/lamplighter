#pragma once
#include <map>
#include <string>

namespace cfg
{
	// Is fullscreen
	extern bool bFullscreen;
	// In the case that this is the networked mode, take the server role
	extern bool bHost;
	// Run as editor
	extern bool bEditMode;
	// Use 2-player splitscreen mode (vs singleplayer mode)
	extern bool bSplitScreen;
	// Draw crosshairs
	extern bool bCrossHairs;

	extern unsigned int iIPA;
	extern unsigned int iIPB;
	extern unsigned int iIPC;
	extern unsigned int iIPD;
	extern unsigned short sIPPORT;

	//config variables loaded from a file
	//window size
	extern unsigned int iWinX;
	extern unsigned int iWinY;

	//camera stuff
	extern float fCameraFOV;
	extern float fCameraSensitivity;
	extern float fCameraNearClip;
	extern float fCameraFarClip;

	extern float fSyncTimer;
	//ip character string
	//extern std::string sIP;

	void LoadCfg();
}