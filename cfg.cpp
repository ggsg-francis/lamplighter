#include "cfg.h"

#include <iostream>
//file io
#include <fstream>
//attempts at error resolution
#include <sstream>

// for gnuc
#include <cstring>

#include <map>
#include <string>

Config config;

namespace cfg
{
	void LoadCfg()
	{
		config = Config(); // set all default values

		std::cout << "Loading preferences..." << std::endl;
		FILE* file = fopen("000config.txt", "r");
		if (file)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			bool exit = false;

			while (!exit)
			{
				char type = fgetc(file);

				//if we're at the end of the file
				if (type == EOF) return;
				//load variable name
				std::string variable;
				while (true)
				{
					char c = fgetc(file);
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
					char c = fgetc(file);
					//if we hit a line break, break
					if (c == 10)
					{
						break;
					}
					else if (c == EOF)
					{
						exit = true;
						break;
					}
					else
					{
						value.push_back(c);
					}
				}

				//apply read string to variable of the right type
				switch (type)
				{
				case 'b':
					//convert to bool
					if (strcmp(variable.c_str(), "Host") == 0)
						config.bHost = (bool)atoi(value.c_str());
					else if (strcmp(variable.c_str(), "EditMode") == 0)
						config.bEditMode = (bool)atoi(value.c_str());
					else if (strcmp(variable.c_str(), "SplitScreen") == 0)
						config.bSplitScreen = (bool)atoi(value.c_str());
					else if (strcmp(variable.c_str(), "CrossHairs") == 0)
						config.bCrossHairs = (bool)atoi(value.c_str());
					std::cout << "Configured boolean " << variable << " as " << value << std::endl;
					break;
				case 'i':
					//convert to int
					if (strcmp(variable.c_str(), "FullScreen") == 0)
						config.iFullscreen = atoi(value.c_str());
					else if (strcmp(variable.c_str(), "WinX") == 0)
						config.iWinX = atoi(value.c_str());
					else if (strcmp(variable.c_str(), "WinY") == 0)
						config.iWinY = atoi(value.c_str());
					else if (strcmp(variable.c_str(), "Port") == 0)
						config.iPort = atoi(value.c_str());
					std::cout << "Configured integer " << variable << " as " << value << std::endl;
					break;
				case 'f':
					//convert to float
					if (strcmp(variable.c_str(), "CameraFOV") == 0)
						config.fCameraFOV = atof(value.c_str());
					else if (strcmp(variable.c_str(), "CameraNearClip") == 0)
						config.fCameraNearClip = atof(value.c_str());
					else if (strcmp(variable.c_str(), "CameraFarClip") == 0)
						config.fCameraFarClip = atof(value.c_str());
					else if (strcmp(variable.c_str(), "CameraSensitivity") == 0)
						config.fCameraSensitivity = atof(value.c_str());
					std::cout << "Configured float " << variable << " as " << value << std::endl;
					break;
				case 's':
					if (strcmp(variable.c_str(), "ConnAddr") == 0 && value.length() < HOSTNAME_MAX_LEN)
						strcpy(config.sConnAddr, value.c_str());
					std::cout << "Configured string " << variable << " as " << value << std::endl;
					break;
				default:
					break;
				}
			}
		}
	}
}
