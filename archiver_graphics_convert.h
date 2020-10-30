#pragma once

#include "graphics_structures.h"

namespace graphics
{
	enum MDHandleType
	{
		eDEFAULT,
		eBODY,
		eARM,
		eHEAD,
		eLEG,
	};

	void ConvertTex(char* FILENAME_SRC, void* FILE_WRITE, lui8 FILTER, lui8 EDGE);
	void ConvertMesh(char* FILENAME_SRC, void* FILE_WRITE);
	void ConvertMB(char* FILENAME_SRC_A, char* FILENAME_SRC_B, void* FILE_WRITE);
	void ConvertMD(char* FILENAME_SRC, void* FILE_WRITE, MDHandleType HANDLE_TYPE);
}
