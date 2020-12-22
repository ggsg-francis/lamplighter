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

	void ConvertTex(char* filename_src, void* file_write, lui8 FILTER, lui8 EDGE);
	void ConvertMesh(char* filename_src, void* file_write);
	void ConvertMB(char* filename_src_a, char* filename_src_b, void* file_write);
	void ConvertMeshSet(char* folder_src, void* file_write);
	void ConvertMD(char* filename_src, void* file_write, MDHandleType handle_type);
}
