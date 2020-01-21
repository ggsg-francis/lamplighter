#pragma once

#define MD_MATRIX_COUNT 4u

namespace graphics
{
	//-------------------------------- VECTORS

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
	struct Vertex {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for blending between two meshes
	struct VertexBlend {
		vec3 pos_a; // Position A
		vec3 pos_b; // Position B
		vec3 nor_a; // Normal A
		vec3 nor_b; // Normal B
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for a rigged mesh
	struct VertexDeform {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
		btf32 mat[MD_MATRIX_COUNT];
	};

	// Vertex structure for terrain mesh
	struct VertexTerrain {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	//-------------------------------- VERTEX ATTRIBUTES

	// This looks ridiculous, but the 'offsetof' macro was on the fritz in release builds for some reason.
	#ifndef DEF_ARCHIVER
	enum v_i : btui32 { // Vert Indices
		VI_POS, VI_NOR, VI_UVC, VI_COL,
	};
	enum v_o : size_t { // Vert Offsets
		VO_POS = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->pos))),
		VO_NOR = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->nor))),
		VO_UVC = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->uvc))),
		VO_COL = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->col))),
	};
	enum vb_i : btui32 { // VertBlend Indices
		vbi_pos_a, vbi_pos_b, vbi_nor_a, vbi_nor_b, vbi_uvc, vbi_col,
	};
	enum vb_o : size_t { // VertBlend Offsets
		vb_pos_a = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->pos_a))),
		vb_pos_b = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->pos_b))),
		vb_nor_a = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->nor_a))),
		vb_nor_b = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->nor_b))),
		vb_uvc = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->uvc))),
		vb_col = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->col))),
	};
	enum vd_i : btui32 { // VertDeform Indices
		vdi_pos, vdi_nor, vdi_uvc, vdi_col, vdi_mat,
	};
	enum vd_o : size_t { // VertDeform Offsets
		vd_pos = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->pos))),
		vd_nor = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->nor))),
		vd_uvc = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->uvc))),
		vd_col = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->col))),
		vd_mat = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->mat))),
	};
	enum vt_i : btui32 { // VertTerrain Indices
		VT_I_POS, VT_I_NOR, VT_I_UVC, VT_I_COL,
	};
	enum vt_o : size_t { // VertTerrain Offsets
		VT_O_POS = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->pos))),
		VT_O_NOR = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->nor))),
		VT_O_UVC = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->uvc))),
		VT_O_COL = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->col))),
	};
	#endif // DEF_ARCHIVER
}