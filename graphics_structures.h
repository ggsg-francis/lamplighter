#ifndef GRAPHICS_STRUCTURES_H
#define GRAPHICS_STRUCTURES_H

#ifdef __cplusplus
extern "C" {
	#endif

	#define MD_MATRIX_COUNT 4u

	//-------------------------------- VECTORS

	typedef struct vec2
	{
		btf32 x, y;
	} vec2;
	typedef struct vec3
	{
		btf32 x, y, z;
	} vec3;
	typedef struct vec4
	{
		btf32 x, y, z, w;
	} vec4;

	//-------------------------------- VERTEX TYPES

	#ifdef DEF_ARCHIVER
	// Vertex containing all vertex data a mesh could use
	struct Vert_ALL {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
		btf32 matr[MD_MATRIX_COUNT];
	};
	#endif // DEF_ARCHIVER

	// Vertex structure
	typedef struct Vertex {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	} Vertex;

	// Vertex structure used for blending between two meshes
	typedef struct VertexBlend {
		vec3 pos_a; // Position A
		vec3 pos_b; // Position B
		vec3 nor_a; // Normal A
		vec3 nor_b; // Normal B
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	} VertexBlend;

	// Vertex structure used for a rigged mesh
	typedef struct VertexDeform {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
		btf32 mat[MD_MATRIX_COUNT];
	} VertexDeform;

	// Vertex structure for terrain mesh
	typedef struct VertexTerrain {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	} VertexTerrain;

	#ifdef __cplusplus
}
#endif

#endif // END OF FILE
