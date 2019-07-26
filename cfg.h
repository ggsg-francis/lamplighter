#pragma once
#include <map>
#include <string>

namespace cfg
{
	//is fullscreen
	extern bool bFullscreen;
	extern bool bShowConsole;

	extern unsigned int iIPA;
	extern unsigned int iIPB;
	extern unsigned int iIPC;
	extern unsigned int iIPD;
	extern unsigned short iIPPORT;

	//config variables loaded from a file
	//window size
	extern unsigned int iWinX;
	extern unsigned int iWinY;
	//render size
	extern unsigned int iRenX;
	extern unsigned int iRenY;

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