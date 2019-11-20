#pragma once

//new graphics namespace, we want to move our stuff into here!
namespace serializer_graphics
{
	struct vec2
	{
		btf32 x = 0.f, y = 0.f;
	};
	struct vec3
	{
		btf32 x = 0.f, y = 0.f, z = 0.f;
	};
	struct vec4
	{
		btf32 x = 0.f, y = 0.f, z = 0.f, w = 0.f;
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| VERTEX

	#define VERT_BONE_COUNT 4

	// Vertex containing all vertex data a mesh could use
	struct vert_everything {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
		btui32 boneID[VERT_BONE_COUNT]; // Bone handle
		btf32 boneWeight[VERT_BONE_COUNT]; // Bone weight per handle
		btui32 util_bindex = 0u;
	};

	// Vertex structure
	struct vert {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for blending between two meshes
	struct vert_blend {
		vec3 pos_a; // Position A
		vec3 pos_b; // Position B
		vec3 nor_a; // Normal A
		vec3 nor_b; // Normal B
		vec2 uvc; // UV Coords (TexCoords)
	};

	// Vertex structure used for a rigged mesh
	struct vert_rig {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		btui32 boneID[VERT_BONE_COUNT]; // Bone handle
		btf32 boneWeight[VERT_BONE_COUNT]; // Bone weight per handle
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| CONVERSION FUNCTIONS

	void ConvertTex(char* FILENAME_SRC, char* FILENAME_DST, btui8 FILTER, btui8 EDGE);
	void ConvertMesh(char* FILENAME_SRC, char* FILENAME_DST);
	void ConvertMB(char* FILENAME_SRC_A, char* FILENAME_SRC_B, char* FILENAME_DST);
	void ConvertMRig(char* FILENAME_SRC, char* FILENAME_DST);
}