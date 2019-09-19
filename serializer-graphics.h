#pragma once

//new graphics namespace, we want to move our stuff into here!
namespace serializer_graphics
{
	struct vec2
	{
		float x = 0.f, y = 0.f;
	};
	struct vec3
	{
		float x = 0.f, y = 0.f, z = 0.f;
	};
	struct vec4
	{
		float x = 0.f, y = 0.f, z = 0.f, w = 0.f;
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| VERTEX

	// Vertex structure
	struct vert {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for blending between two meshes
	struct vert_blend {
		vec3 pos_a; // Position
		vec3 pos_b;
		vec3 nor_a; // Normal
		vec3 nor_b;
		vec2 uvc; // UV Coords (TexCoords)
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| CONVERSION FUNCTIONS

	void ConvertTex(char* FILENAME_SRC, char* FILENAME_DST, btui8 FILTER, btui8 EDGE);
	void ConvertMesh(char* FILENAME_SRC, char* FILENAME_DST);
	void ConvertMB(char* FILENAME_SRC_A, char* FILENAME_SRC_B, char* FILENAME_DST);
}