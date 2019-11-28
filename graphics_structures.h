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
	struct Vert {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for blending between two meshes
	struct VertBlend {
		vec3 pos_a; // Position A
		vec3 pos_b; // Position B
		vec3 nor_a; // Normal A
		vec3 nor_b; // Normal B
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
	};

	// Vertex structure used for a rigged mesh
	struct VertDeform {
		vec3 pos; // Position
		vec3 nor; // Normal
		vec2 uvc; // UV Coords (TexCoords)
		vec4 col; // Vertex Colour
		btf32 mat[MD_MATRIX_COUNT];
	};

	//-------------------------------- VERTEX ATTRIBUTES

	#ifndef DEF_ARCHIVER
	enum v_i : btui32 { // Vert Indices
		VI_POS, VI_NOR, VI_UVC, VI_COL,
	};
	enum v_o : size_t { // Vert Offsets
		VO_POS = offsetof(Vert, pos),
		VO_NOR = offsetof(Vert, nor),
		VO_UVC = offsetof(Vert, uvc),
		VO_COL = offsetof(Vert, col),
	};
	enum vb_i : btui32 { // VertBlend Indices
		vbi_pos_a, vbi_pos_b, vbi_nor_a, vbi_nor_b, vbi_uvc, vbi_col,
	};
	enum vb_o : size_t { // VertBlend Offsets
		vb_pos_a = offsetof(VertBlend, pos_a),
		vb_pos_b = offsetof(VertBlend, pos_b),
		vb_nor_a = offsetof(VertBlend, nor_a),
		vb_nor_b = offsetof(VertBlend, nor_b),
		vb_uvc = offsetof(VertBlend, uvc),
		vb_col = offsetof(VertBlend, col),
	};
	enum vd_i : btui32 { // VertDeform Indices
		vdi_pos, vdi_nor, vdi_uvc, vdi_col, vdi_mat,
	};
	enum vd_o : size_t { // VertDeform Offsets
		vd_pos = offsetof(VertDeform, pos),
		vd_nor = offsetof(VertDeform, nor),
		vd_uvc = offsetof(VertDeform, uvc),
		vd_col = offsetof(VertDeform, col),
		vd_mat = offsetof(VertDeform, mat),
	};
	#endif // DEF_ARCHIVER
}