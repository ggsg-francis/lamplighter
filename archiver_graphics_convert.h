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

	void ConvertTex(char* FILENAME_SRC, char* FILENAME_DST, btui8 FILTER, btui8 EDGE);
	void ConvertMesh(char* FILENAME_SRC, char* FILENAME_DST);
	void ConvertMB(char* FILENAME_SRC_A, char* FILENAME_SRC_B, char* FILENAME_DST);
	void ConvertMD(char* FILENAME_SRC, char* FILENAME_DST, MDHandleType HANDLE_TYPE);
}
