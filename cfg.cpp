#include "cfg.h"

#include <iostream>
//file io
#include <fstream>
//attempts at error resolution
#include <sstream>

namespace cfg
{
	// Is fullscreen
	bool bFullscreen = false;
	// Show output console
	bool bShowConsole = false;
	// Run as editor
	bool bEditMode = false;
	bool bSplitScreen = false;

	// IP address variables
	unsigned int iIPA = 0;
	unsigned int iIPB = 0;
	unsigned int iIPC = 0;
	unsigned int iIPD = 0;
	unsigned short iIPPORT = 0;

	// Config variables loaded from a file
	// Window size
	unsigned int iWinX = 640;
	unsigned int iWinY = 480;

	// Camera stuff
	float fCameraFOV = 80.f;
	float fCameraSensitivity = 0.f;
	float fCameraNearClip = 0.1f;
	float fCameraFarClip = 100.f;

	float fSyncTimer = 5.f;

	void LoadCfg()
	{
		//temporary pointers used for this function only
		//(although, they could be made global in the case that they become useful?)
		bool* bptr[] = { &bFullscreen, &bShowConsole, &bEditMode, &bSplitScreen };
		unsigned int* iptr[] = { &iIPA, &iIPB, &iIPC, &iIPD, &iWinX, &iWinY};
		unsigned short* sptr[] = { &iIPPORT };
		//std::string* sptr[] = { &sIP };
		float* fptr[] = { &fCameraFOV, &fCameraSensitivity, &fCameraNearClip, &fCameraFarClip, &fSyncTimer };

		std::cout << "Loading preferences..." << std::endl;
		std::ifstream ifs("000config.txt");
		//number of current variable
		int bCount = 0;
		int iCount = 0;
		int sCount = 0;
		int fCount = 0;

		while (true)
		{
			char type;
			ifs.read((char*)&type, sizeof(char));
			//if we're at the end of the file
			if (type == '<')
			{
				break;
			}
			//load variable name
			std::string variable;
			while (true)
			{
				char c = 0;
				ifs.read((char*)&c, sizeof(char));
				if (c == '=')
				{
					break;
				}
				else
				{
					variable.push_back(c);
				}
			}


			//load value
			std::string value;
			while (true)
			{
				char c = 0;
				ifs.read((char*)&c, sizeof(char));
				//if we hit a line break, break
				if (c == 10)
				{
					break;
				}
				else
				{
					value.push_back(c);
				}
			}

			//apply read string to variable of the right type
			std::stringstream ss;
			switch (type)
			{
			case 'b':
				//convert to bool
				ss << value;
				//if (variable == )
					//std::cout << "found fullscreen bool" << std::endl;

				ss >> *bptr[bCount];
				std::cout << "Configured boolean " << variable << " as " << value << std::endl;
				bCount++;
				break;
			case 'i':
				//convert to int
				ss << value;
				ss >> *iptr[iCount];
				std::cout << "Configured integer " << variable << " as " << value << std::endl;
				iCount++;
				break;
			case 's':
				//convert to short
				ss << value;
				ss >> *sptr[sCount];
				std::cout << "Configured short " << variable << " as " << value << std::endl;
				sCount++;
				break;
			case 'f':
				//convert to float
				ss << value;
				ss >> *fptr[fCount];
				std::cout << "Configured float " << variable << " as " << value << std::endl;
				fCount++;
				break;
			default:
				break;
			}
		}
		ifs.close();
	}
}