//TOP INCLUDE
#include "graphics.hpp"
//#include "graphics_shadersource_gl.h"

//would like to drop this include
#include "glm/gtc\matrix_transform.hpp"
//i dont know what the point of using this one is
#include "glm/gtc\type_ptr.hpp"

#include "maths.hpp"

#include "render.h"

//-------------------------------- VERTEX ATTRIBUTES

// This looks ridiculous, but the 'offsetof' macro was on the fritz in release builds with MSC for some reason.
enum v_i : lui32 { // Vert Indices
	VI_POS, VI_NOR, VI_UVC, VI_COL,
};
enum v_o : size_t { // Vert Offsets
	#ifdef __GNUC__
	VO_POS = offsetof(Vertex, pos),
	VO_NOR = offsetof(Vertex, nor),
	VO_UVC = offsetof(Vertex, uvc),
	VO_COL = offsetof(Vertex, col),
	#elif defined _MSC_VER
	VO_POS = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->pos))),
	VO_NOR = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->nor))),
	VO_UVC = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->uvc))),
	VO_COL = ((size_t)&reinterpret_cast<char const volatile&>((((struct Vertex*)0)->col))),
	#endif
};
enum vb_i : lui32 { // VertBlend Indices
	vbi_pos_a, vbi_pos_b, vbi_nor_a, vbi_nor_b, vbi_uvc, vbi_col,
};
enum vb_o : size_t { // VertBlend Offsets
	#ifdef __GNUC__
	vb_pos_a = offsetof(VertexBlend, pos_a),
	vb_pos_b = offsetof(VertexBlend, pos_b),
	vb_nor_a = offsetof(VertexBlend, nor_a),
	vb_nor_b = offsetof(VertexBlend, nor_b),
	vb_uvc = offsetof(VertexBlend, uvc),
	vb_col = offsetof(VertexBlend, col),
	#elif defined _MSC_VER
	vb_pos_a = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->pos_a))),
	vb_pos_b = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->pos_b))),
	vb_nor_a = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->nor_a))),
	vb_nor_b = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->nor_b))),
	vb_uvc = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->uvc))),
	vb_col = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexBlend*)0)->col))),
	#endif
};
enum vd_i : lui32 { // VertDeform Indices
	vdi_pos, vdi_nor, vdi_uvc, vdi_col, vdi_mat,
};
enum vd_o : size_t { // VertDeform Offsets
	#ifdef __GNUC__
	vd_pos = offsetof(VertexDeform, pos),
	vd_nor = offsetof(VertexDeform, nor),
	vd_uvc = offsetof(VertexDeform, uvc),
	vd_col = offsetof(VertexDeform, col),
	vd_mat = offsetof(VertexDeform, mat),
	#elif defined _MSC_VER
	vd_pos = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->pos))),
	vd_nor = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->nor))),
	vd_uvc = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->uvc))),
	vd_col = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->col))),
	vd_mat = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexDeform*)0)->mat))),
	#endif
};
enum vt_i : lui32 { // VertTerrain Indices
	VT_I_POS, VT_I_NOR, VT_I_UVC, VT_I_TXTR,
};
enum vt_o : size_t { // VertTerrain Offsets
	#ifdef __GNUC__
	VT_O_POS = offsetof(VertexTerrain, pos),
	VT_O_NOR = offsetof(VertexTerrain, nor),
	VT_O_UVC = offsetof(VertexTerrain, uvc),
	VT_O_TXTR = offsetof(VertexTerrain, txtr),
	#elif defined _MSC_VER
	VT_O_POS = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->pos))),
	VT_O_NOR = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->nor))),
	VT_O_UVC = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->uvc))),
	VT_O_TXTR = ((size_t)&reinterpret_cast<char const volatile&>((((struct VertexTerrain*)0)->txtr))),
	#endif
};

#define FILE_VERSION_MB 0x0u
#define FILE_VERSION_M 0x0u
#define FILE_VERSION_TEX 0x0u

typedef lui16 version_t;

namespace graphics
{
	// TODO: MOVE TO MATHS!!!
	m::Vector3 operator*(const m::Vector3& vector, const Matrix4x4& matrix) {
		return m::Vector3(
			(vector.x * matrix[0][0] + vector.y * matrix[0][1] + vector.z * matrix[0][2] + matrix[0][3]), // X
			(vector.x * matrix[1][0] + vector.y * matrix[1][1] + vector.z * matrix[1][2] + matrix[1][3]), // Y
			(vector.x * matrix[2][0] + vector.y * matrix[2][1] + vector.z * matrix[2][2] + matrix[2][3])  // Z
		);
	}

	/*lui8 font_kerning[] =
	{
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	3u,	6u,	7u,	8u,	7u,	7u,	3u,	4u,	4u,	5u,	6u,	3u,	6u,	3u,	4u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	3u,	3u,	7u,	6u,	7u,	6u,
		8u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	6u,	6u,	7u,	6u,	8u,	7u,	7u,
		7u,	7u,	7u,	7u,	6u,	7u,	7u,	8u,	7u,	7u,	7u,	4u,	4u,	4u,	7u,	7u,
		3u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	6u,	5u,	7u,	4u,	8u,	7u,	7u,
		7u,	7u,	7u,	7u,	5u,	7u,	7u,	8u,	7u,	7u,	7u,	4u,	2u,	4u,	6u,	7u,

		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	};*/

	/*lui8 font_kerning[] =
	{
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		7,	3,	6,	7,	8,	7,	7,	3,	4,	4,	5,	6,	3,	6,	3,	4,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	3,	3,	7,	6,	7,	6,
		8,	7,	7,	7,	7,	7,	7,	7,	7,	6,	6,	7,	6,	8,	7,	7,
		7,	7,	7,	7,	6,	7,	7,	8,	7,	7,	7,	4,	4,	4,	7,	7,
		3,	7,	7,	7,	7,	7,	6,	7,	7,	6,	5,	7,	6,	8,	7,	7,
		7,	7,	7,	7,	6,	7,	7,	8,	7,	7,	7,	4,	2,	4,	6,	7,

		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7
	};*/

	lui8 font_kerning[] =
	{
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		5,	/* */	6,	/*!*/	4,	/*"*/	0,	/* */	0,	/* */	0,	/* */	0,	/* */	2,	/*'*/
		7,	/*(*/	7,	/*)*/	0,	/* */	0,	/* */	2,	/*,*/	0,	/*-*/	3,	/*.*/	0,	/*/*/
		8,	/*0*/	8,	/*1*/	8,	/*2*/	8,	/*3*/	8,	/*4*/	8,	/*5*/	8,	/*6*/	8,	/*7*/
		8,	/*8*/	8,	/*9*/	6,	/* */	10,	/* */	10,	/* */	10,	/* */	10,	/* */	10,	/* */
		10,	/* */	10,	/*A*/	10,	/*B*/	10,	/*C*/	10,	/*D*/	10,	/*E*/	10,	/*F*/	10,	/*G*/
		10,	/*H*/	5,	/*I*/	5,	/*J*/	10,	/*K*/	10,	/*L*/	13,	/*M*/	10,	/*N*/	10,	/*O*/
		10,	/* */	10,	/*Q*/	10,	/*R*/	10,	/*S*/	9,	/*T*/	10,	/*U*/	10,	/*V*/	13,	/*W*/
		10,	/*X*/	10,	/*Y*/	10,	/*Z*/	10,	/* */	10,	/* */	10,	/* */	10,	/* */	10,	/* */
		8,	/* */	8,	/*a*/	8,	/*b*/	8,	/*c*/	8,	/*d*/	8,	/*e*/	8,	/*f*/	8,	/*g*/
		8,	/*h*/	3,	/*i*/	4,	/*j*/	8,	/*k*/	4,	/*l*/	11,	/*m*/	8,	/*n*/	8,	/*o*/
		8,	/*p*/	8,	/*q*/	8,	/*r*/	8,	/*s*/	6,	/*t*/	8,	/*u*/	8,	/*v*/	11,	/*w*/
		8,	/*x*/	8,	/*y*/	8,	/*z*/	8,	/* */	8,	/* */	8,	/* */	8,	/* */	8,	/* */

		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
		7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7
	};

	/*lui8 font_kerning[] =
	{
		0u,	0u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,

		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	};*/

	struct Graphics
	{
		lui32 frameSizeX = 320u;
		lui32 frameSizeY = 240u;
		lui32 GUIframeSizeX = 320u;
		lui32 GUIframeSizeY = 240u;
		Shader shaders[S_COUNT];
		bool bEditMode = false;
		bool bSplitScreen = false;
		lf32 fCameraFOV, fCameraNearClip, fCameraFarClip;
	};
	Graphics* gPtr;

	lui32 FrameSizeX() { return gPtr->frameSizeX; }
	lui32 FrameSizeY() { return gPtr->frameSizeY; }

	void SetFrameSize(lui32 x, lui32 y) {
		if (!gPtr->bEditMode) {
			lui32 scale = x / SCREEN_UPSCALE_THRESHOLD + 1u;
			gPtr->frameSizeY = y / scale;
			#ifndef DEF_NMP
			if (gPtr->bSplitScreen) {
				gPtr->frameSizeX = x / (scale * 2u);
				x /= 2;
			}
			else
				#endif
			{
				gPtr->frameSizeX = x / scale;
			}
			gPtr->shaders[S_POST].Use();
			gPtr->shaders[S_POST].SetFloat(gPtr->shaders[S_POST].fWindowX, (GLfloat)x);
			gPtr->shaders[S_POST].SetFloat(gPtr->shaders[S_POST].fWindowY, (GLfloat)y);
		}
		else
		{
			gPtr->frameSizeX = x;
			gPtr->frameSizeY = y;
		}
		gPtr->GUIframeSizeX = gPtr->frameSizeX;
		gPtr->GUIframeSizeY = gPtr->frameSizeY;
	}
	void SetGUIFrameSize(lui32 x, lui32 y) {
		gPtr->GUIframeSizeX = x;
		gPtr->GUIframeSizeY = y;
	}

	glm::mat4 mat_proj;
	glm::mat4 mat_view;

	// TEMP TEMP TEMP TEMP
	// (narrator voice) it wasn't temporary
	GUIBitmap guibmp;
	GUIBox guibox;
	GUIText guitxt;

	//________________________________________________________________________________________________________________________________
	// INITIALIZATION ----------------------------------------------------------------------------------------------------------------

	void MakeShaderSrc(char** outstring, lui32 num, ...) {
		lui32 ssize = 0u;
		// Iterate through the strings
		va_list args;
		va_start(args, num);
		for (lui32 i = 0; i < num; ++i) {
			char* workingstr = va_arg(args, char*);
			lui32 oldsize = ssize;
			ssize += (lui32)strlen(workingstr);
			*outstring = (char*)realloc(*outstring, ssize + 1);
			memset(&(*outstring)[oldsize], 0, ssize - oldsize);
			strcat(*outstring, workingstr);
			//memcpy(&(*outstring)[oldsize], workingstr, ssize - oldsize); // dont know why this doesnt work
		}
		va_end(args);
	}

	void Init(bool editmode, bool splitscreen, lf32 camerafov,
		lf32 cameranearclip, lf32 camerafarclip, lui32 winx, lui32 winy) {
		gPtr = new Graphics();

		guibmp.Init();
		guibox.Init();
		guitxt.Init();

		#include "graphics_shadersource_gl.h"

		// Make vertex shader sources

		char* vert_3d = nullptr;
		char* vert_3d_blend = nullptr;
		char* vert_3d_deform = nullptr;
		char* vert_3d_terrain = nullptr;
		MakeShaderSrc(&vert_3d, 2, SSRC_VERSION, SSRC_VERT_3D);
		MakeShaderSrc(&vert_3d_blend, 2, SSRC_VERSION, SSRC_VERT_3D_BLEND);
		MakeShaderSrc(&vert_3d_deform, 2, SSRC_VERSION, SSRC_VERT_3D_DEFORM);
		MakeShaderSrc(&vert_3d_terrain, 2, SSRC_VERSION, SSRC_VERT_3D_TERRAIN);

		// Make fragment shader sources

		char* frag_3d = nullptr;
		char* frag_3d_chara = nullptr;
		char* frag_3d_terrain = nullptr;
		char* frag_3d_meat = nullptr;
		MakeShaderSrc(&frag_3d, 4, SSRC_VERSION, SSRC_VAR_FRAG_3D, SSRC_FNC_SHADOW, SSRC_MAIN_FRAG_3D);
		MakeShaderSrc(&frag_3d_chara, 5, SSRC_VERSION, SSRC_VAR_FRAG_3D, SHADER_VAR_FRAG_3D_CHARA, SSRC_FNC_SHADOW, SSRC_MAIN_FRAG_3D_CHARA);
		MakeShaderSrc(&frag_3d_terrain, 4, SSRC_VERSION, SSRC_VAR_FRAG_3D_TERRAIN, SSRC_FNC_SHADOW, SSRC_MAIN_FRAG_3D_TERRAIN);
		MakeShaderSrc(&frag_3d_meat, 4, SSRC_VERSION, SSRC_VAR_FRAG_3D_MEAT, SSRC_FNC_SHADOW, SSRC_MAIN_FRAG_3D_MEAT);

		// Compile shaders

		gPtr->shaders[S_SOLID].Init(vert_3d, frag_3d);
		gPtr->shaders[S_SOLID_CHARA].Init(vert_3d, frag_3d_chara);

		gPtr->shaders[S_SOLID_BLEND].Init(vert_3d_blend, frag_3d);
		gPtr->shaders[S_SOLID_BLEND_CHARA].Init(vert_3d_blend, frag_3d_chara);

		gPtr->shaders[S_SOLID_DEFORM].Init(vert_3d_deform, frag_3d);
		gPtr->shaders[S_SOLID_DEFORM_CHARA].Init(vert_3d_deform, frag_3d_chara);

		gPtr->shaders[S_SOLID_TERRAIN].Init(vert_3d_terrain, frag_3d_terrain);

		gPtr->shaders[S_MEAT].Init(vert_3d, frag_3d_meat);

		// Free the shader sources

		free(vert_3d);
		free(vert_3d_blend);
		free(vert_3d_deform);
		free(vert_3d_terrain);

		free(frag_3d);
		free(frag_3d_chara);
		free(frag_3d_terrain);
		free(frag_3d_meat);

		gPtr->shaders[S_GUI].Init(SHADER_VERT_GUI, SHADER_FRAG_GUI);
		#if !DEF_DEPTH_BUFFER_RW
		gPtr->shaders[S_POST].Init(SHADER_VERT_FRAMEBUFFER, SHADER_FRAG_FRAMEBUFFER);
		//gPtr->shaders[S_POST].Init(SHADER_VERT_FRAMEBUFFER, SHADER_FRAG_FRAMEBUFFER_EFFECT);
		#else
		gPtr->shaders[S_POST].Init(SHADER_VERT_FRAMEBUFFER, SHADER_FRAG_FRAMEBUFFER_DEPTH_BUFFER);
		#endif
		
		gPtr->bEditMode = editmode;
		gPtr->bSplitScreen = splitscreen;
		gPtr->fCameraFOV = camerafov;
		gPtr->fCameraNearClip = cameranearclip;
		gPtr->fCameraFarClip = camerafarclip;
		SetFrameSize(winx, winy);

		glEnable(GL_CULL_FACE); // Enable face culling
		glCullFace(GL_FRONT); // Set culling mode
	}

	void End() {
		delete gPtr;
		gPtr = nullptr;
	}

	void SetSplitScreen(bool b, lui32 winx, lui32 winy) {
		gPtr->bSplitScreen = b;
		SetFrameSize(winx, winy);
	}

	//________________________________________________________________________________________________________________________________
	// MATRIX ------------------------------------------------------------------------------------------------------------------------

	FRow4 operator*(const FRow4& row, const lf32 mult) {
		return FRow4(row[0] * mult, row[1] * mult, row[2] * mult, row[3] * mult);
	};
	FRow4 operator+(const FRow4& row_a, const FRow4& row_b) {
		return FRow4(row_a[0] + row_b[0], row_a[1] + row_b[1], row_a[2] + row_b[2], row_a[3] + row_b[3]);
	};

	Matrix4x4 MatrixTranslate(Matrix4x4 const& m, m::Vector3 const& v) {
		Matrix4x4 result = m;
		result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
		return result;
		//matrix[3] = matrix[0] * pos.x + matrix[1] * pos.y + matrix[2] * pos.z + matrix[3];
	}
	Matrix4x4 MatrixRotate(Matrix4x4 const& m, lf32 angle, m::Vector3 const& v) {
		lf32 const c = cos(angle);
		lf32 const s = sin(angle);

		m::Vector3 axis = m::Normalize(v);
		m::Vector3 temp = (1 - c) * axis;

		Matrix4x4 rot;
		rot[0][0] = c + temp.x * axis.x;
		rot[0][1] = temp.x * axis.y + s * axis.z;
		rot[0][2] = temp.x * axis.z - s * axis.y;

		rot[1][0] = temp.y * axis.x - s * axis.z;
		rot[1][1] = c + temp.y * axis.y;
		rot[1][2] = temp.y * axis.z + s * axis.x;

		rot[2][0] = temp.z * axis.x + s * axis.y;
		rot[2][1] = temp.z * axis.y - s * axis.x;
		rot[2][2] = c + temp.z * axis.z;

		Matrix4x4 result;
		result[0] = m[0] * rot[0][0] + m[1] * rot[0][1] + m[2] * rot[0][2];
		result[1] = m[0] * rot[1][0] + m[1] * rot[1][1] + m[2] * rot[1][2];
		result[2] = m[0] * rot[2][0] + m[1] * rot[2][1] + m[2] * rot[2][2];
		result[3] = m[3];
		return result;
	}
	Matrix4x4 MatrixScale(Matrix4x4 const& m, m::Vector3 const& v) {
		Matrix4x4 Result;
		Result[0] = m[0] * v.x;
		Result[1] = m[1] * v.y;
		Result[2] = m[2] * v.z;
		Result[3] = m[3];
		return Result;
	}
	Matrix4x4 MatrixLookAt(Matrix4x4 const& matrix, m::Vector3 const& src, m::Vector3 const& targ, m::Vector3 const& up) {
		m::Vector3 f = m::Normalize(targ - src);
		m::Vector3 s = m::Normalize(m::Cross(f, up));
		m::Vector3 u = m::Cross(s, f);

		Matrix4x4 matr;
		matr[0][0] = s.x;
		matr[1][0] = s.y;
		matr[2][0] = s.z;
		matr[0][1] = u.x;
		matr[1][1] = u.y;
		matr[2][1] = u.z;
		matr[0][2] = -f.x;
		matr[1][2] = -f.y;
		matr[2][2] = -f.z;
		matr[3][0] = -m::Dot(s, src);
		matr[3][1] = -m::Dot(u, src);
		matr[3][2] = m::Dot(f, src);
		return matr;
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);
	}
	void MatrixTransformXFlip(Matrix4x4& matrix, m::Vector3 const& pos) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, lf32 yaw) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		lf32 c = cos(yaw); lf32 s = sin(yaw);
		Matrix4x4 m = matrix;
		matrix[0] = m[0] * c + m[2] * -s;
		matrix[1] = m[1] * (c + (1 - c));
		matrix[2] = m[0] * s + m[2] * c;
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, lf32 yaw, lf32 pitch) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix (yaw)
		lf32 c = cos(yaw); lf32 s = sin(yaw);
		Matrix4x4 m = matrix;
		matrix[0] = m[0] * c + m[2] * -s;
		matrix[1] = m[1] * (c + (1 - c));
		matrix[2] = m[0] * s + m[2] * c;

		// Rotate matrix (pitch)
		c = cos(pitch); s = sin(pitch);
		m = matrix;
		matrix[0] = m[0] * (c + (1 - c));
		matrix[1] = m[1] * c + m[2] * s;
		matrix[2] = m[1] * -s + m[2] * c;
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0)) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir);
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		#if DEF_SWR
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = vSide.z;
		matrix[1][0] = vLoUp.x; matrix[1][1] = vLoUp.y; matrix[1][2] = vLoUp.z;
		matrix[2][0] = vForw.x; matrix[2][1] = vForw.y; matrix[2][2] = vForw.z;
		#else
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vLoUp.x; matrix[1][1] = vLoUp.y; matrix[1][2] = -vLoUp.z;
		matrix[2][0] = vForw.x; matrix[2][1] = vForw.y; matrix[2][2] = -vForw.z;
		#endif
	}
	void MatrixTransformForwardUp(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0)) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir) * -1.f;
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		#if DEF_SWR
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = vSide.z;
		matrix[1][0] = vForw.x; matrix[1][1] = vForw.y; matrix[1][2] = vForw.z;
		matrix[2][0] = -vLoUp.x; matrix[2][1] = -vLoUp.y; matrix[2][2] = -vLoUp.z;
		#else
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vForw.x; matrix[1][1] = vForw.y; matrix[1][2] = -vForw.z;
		matrix[2][0] = -vLoUp.x; matrix[2][1] = -vLoUp.y; matrix[2][2] = vLoUp.z;
		#endif
	}
	void MatrixTransformXFlip(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0)) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir);
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vLoUp.x; matrix[1][1] = vLoUp.y; matrix[1][2] = -vLoUp.z;
		matrix[2][0] = vForw.x; matrix[2][1] = vForw.y; matrix[2][2] = -vForw.z;

		matrix[0] = matrix[0] * -1.f;
	}
	void MatrixTransformXFlipForwardUp(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0)) {
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir);
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = -vForw.x; matrix[1][1] = -vForw.y; matrix[1][2] = vForw.z;
		matrix[2][0] = vLoUp.x; matrix[2][1] = vLoUp.y; matrix[2][2] = -vLoUp.z;

		matrix[0] = matrix[0] * -1.f;
	}
	Matrix4x4 MatrixGetMirror(Matrix4x4& matrix) {
		Matrix4x4 rmat = matrix;
		rmat[0] = rmat[0] * -1.f;
		return rmat;
	}
	m::Vector3 MatrixGetPosition(Matrix4x4& matrix) {
		return m::Vector3(matrix[3][0], matrix[3][1], -matrix[3][2]);
	}
	m::Vector3 MatrixGetForward(Matrix4x4& matrix) {
		return m::Vector3(matrix[2][0], matrix[2][1], -matrix[2][2]);
	}
	m::Vector3 MatrixGetRight(Matrix4x4& matrix) {
		return m::Vector3(matrix[0][0], matrix[0][1], -matrix[0][2]);
	}
	m::Vector3 MatrixGetUp(Matrix4x4& matrix) {
		return m::Vector3(matrix[1][0], matrix[1][1], -matrix[1][2]);
	}
	m::Vector3 MatrixOrientVector(Matrix4x4& matrix, m::Vector3 vector) {
		return
			graphics::MatrixGetRight(matrix) * vector.x +
			graphics::MatrixGetUp(matrix) * vector.y +
			graphics::MatrixGetForward(matrix) * vector.z;
	}

	m::Vector3 view;
	m::Vector3 focus;
	m::Vector3 GetViewPos() {
		return view;
	}
	m::Vector3 GetFocalCenter() {
		return focus;
	}
	void SetMatProj(lf32 fovMult) {
		mat_proj = glm::perspective(glm::radians(gPtr->fCameraFOV) * fovMult, (float)FrameSizeX() / (float)FrameSizeY(), gPtr->fCameraNearClip * fovMult, gPtr->fCameraFarClip * fovMult);	}
	void SetMatView(void* t, void* p, void* t2) {
		view = *(m::Vector3*)p * m::Vector3(1.f, 1.f, -1.f);
		mat_view = glm::lookAt(*(glm::vec3*)p * m::Vector3(1.f, 1.f, -1.f), *(glm::vec3*)t * m::Vector3(1.f, 1.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
	}
	// Lightsource
	void SetMatProjLight() {
		mat_proj = glm::ortho(-SHADOW_WIDTH, SHADOW_WIDTH, -SHADOW_WIDTH, SHADOW_WIDTH, 0.f, SHADOW_FAR);
	}
	void SetMatViewLight(float x, float y, float z, float vx, float vy, float vz) {
		mat_view = glm::lookAt(glm::vec3(x - (vx * SHADOW_HALF), y - (vy * SHADOW_HALF), z - (vz * SHADOW_HALF)),
			glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::mat4 GetMatProj() {
		return mat_proj;
	}
	glm::mat4 GetMatView() {
		return mat_view;
	}

	//________________________________________________________________________________________________________________________________
	// UTILITY FUNCTIONS -------------------------------------------------------------------------------------------------------------

	void SetFrontFace() {
		glFrontFace(GL_CW);
	}
	void SetFrontFaceInverse() {
		glFrontFace(GL_CCW);
	}
	void SetRenderSolid() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	void SetRenderWire() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// good, but replace the std::vectors
	void BindBuffers(std::vector<Vertex> &vertices, std::vector<lui32> &indices, GLuint VAO, GLuint VBO, GLuint EBO) {
		glBindVertexArray(VAO); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(lui32), &indices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VI_POS); // Set Vertex positions
		glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
		glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
		glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
		glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
		glEnableVertexAttribArray(VI_COL); // Set Vertex colour
		glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);
	}

	//________________________________________________________________________________________________________________________________
	// SHADER ------------------------------------------------------------------------------------------------------------------------

	void Shader::Init(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {
		const char* vShaderCode = vertexSource;
		const char* fShaderCode = fragmentSource;
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometrySource != nullptr) {
			const char * gShaderCode = geometrySource;
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometrySource != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometrySource != nullptr)
			glDeleteShader(geometry);

		// Search for all shader properties
		for (lui32 i = 0u; i < LOCATION_COUNT; ++i) {
			location[i] = glGetUniformLocation(ID, names[i]);
		}
	}
	void Shader::Use() {
		glUseProgram(ID);
	}
	void Shader::SetBool(const PIndex index, bool value) const {
		glUniform1i(location[index], (int)value);
	}
	void Shader::SetInt(const PIndex index, int value) const {
		glUniform1i(location[index], value);
	}
	void Shader::SetUint(const PIndex index, unsigned int value) const {
		glUniform1ui(location[index], value);
	}
	void Shader::SetFloat(const PIndex index, float value) const {
		glUniform1f(location[index], value);
	}
	void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void Shader::setVec2(const std::string &name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void Shader::setVec3(const PIndex index, const glm::vec3 &value) const {
		glUniform3fv(location[index], 1, &value[0]);
	}
	void Shader::setVec3(const PIndex index, float x, float y, float z) const {
		glUniform3f(location[index], x, y, z);
	}
	void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::setMat4(const PIndex index, const Matrix4x4& mat) const {
		glUniformMatrix4fv(location[index], 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::SetTexture(const PIndex index, GLuint texture, TIndex textureIndex) {
		glActiveTexture(temp[textureIndex]); // active proper texture unit before binding
		//glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
		glUniform1i(location[index], textureIndex);
		glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture
	}
	void Shader::CheckCompileErrors(GLuint shader, const std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	Shader& GetShader(eShader id) {
		return gPtr->shaders[id];
	}

	//________________________________________________________________________________________________________________________________
	// TEXTURE -----------------------------------------------------------------------------------------------------------------------

	void FMNearest() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	void FMNearestMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		#if !DEF_CUSTOM_MIPMAP
		glGenerateMipmap(GL_TEXTURE_2D);
		#endif
	}
	void FMLinear() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	void FMLinearMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#if !DEF_CUSTOM_MIPMAP
		glGenerateMipmap(GL_TEXTURE_2D);
		#endif
	}
	void FMFoliage() {
		#if DEF_OLDSKOOL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#endif
	}
	void(*SetFilterMode[])() = { FMNearest, FMNearestMip, FMLinear, FMLinearMip, FMFoliage };
	void EMRepeat() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	void EMRepeatMirror() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	void EMClamp() { // Also can use GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	void EMRepeatXClampY() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	void EMClampXRepeatY() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	void(*SetEdgeMode[])() = { EMRepeat, EMRepeatMirror, EMClamp, EMRepeatXClampY, EMClampXRepeatY };

	#if DEF_SWR
	void Texture::Init(lui16 sx, lui16 sy, colour col) {
		glGenTextures(1, &glID); // Initialize GL texture

		width = sx;
		height = sy;
		// Read pixel buffer
		buffer2 = new colour[width * height];
		for (lui32 i = 0u; i < (lui32)(width * height); ++i) {
			buffer2[i] = col;
		}
	}
	void Texture::SetPixel(lui16 x, lui16 y, colour c) {
		buffer2[x * width + y] = c;
	}
	void Texture::SetPixelChannelR(lui16 x, lui16 y, lui8 val) {
		buffer2[x * width + y].r = val;
	}
	void Texture::SetPixelChannelG(lui16 x, lui16 y, lui8 val) {
		buffer2[x * width + y].g = val;
	}
	void Texture::SetPixelChannelB(lui16 x, lui16 y, lui8 val) {
		buffer2[x * width + y].b = val;
	}
	void Texture::SetPixelChannelA(lui16 x, lui16 y, lui8 val) {
		buffer2[x * width + y].a = val;
	}
	colour Texture::GetPixel(lui16 x, lui16 y) {
		return buffer2[x * width + y];
	}
	void Texture::InitGLTest() {
		glGenTextures(1, &glID);
	}
	void Texture::ReBindGL(TextureFilterMode filter_mode, TextureEdgeMode edge_mode) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer2);

		SetFilterMode[filter_mode]();
		SetEdgeMode[edge_mode]();
	}
	#endif

	void Texture::LoadFile(void* File) {
		glGenTextures(1, &glID);

		FILE* file = (FILE*)File;

		// Read version
		version_t v;
		fread(&v, sizeof(version_t), 1, file);
		TextureFilterMode fm;
		TextureEdgeMode em;
		fread(&fm, 1, 1, file);
		fread(&em, 1, 1, file);
		// Read dimensions
		fread(&width, sizeof(lui16), 1, file);
		fread(&height, sizeof(lui16), 1, file);
		// Read pixel buffer
		#if DEF_SWR
		buffer2 = new colour[width * height];
		fread(buffer2, sizeof(graphics::colour), width * height, file);
		#else
		graphics::colour* buffer = new colour[width * height];
		fread(buffer, sizeof(graphics::colour), width * height, file);
		#endif

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		#if DEF_SWR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer2);
		#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		#endif
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width / 2, height / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_mip);

		// Custom Mip test
		#if DEF_CUSTOM_MIPMAP
		if (fm == eLINEAR_MIPMAP || fm == eNEAREST_MIPMAP) {
			graphics::colour* buffer_mip01 = new colour[width / 2 * height / 2];
			graphics::colour* buffer_mip02 = new colour[width / 4 * height / 4];
			graphics::colour* buffer_mip03 = new colour[width / 8 * height / 8];
			graphics::colour* buffer_mip04 = new colour[width / 16 * height / 16];
			graphics::colour* buffer_mip05 = new colour[width / 32 * height / 32];
			graphics::colour* buffer_mip06 = new colour[width / 64 * height / 64];
			graphics::colour* buffer_mip07 = new colour[width / 128 * height / 128];
			graphics::colour* buffer_mip08 = new colour[width / 256 * height / 256];
			graphics::colour* buffer_mip09 = new colour[width / 512 * height / 512];
			graphics::colour* buffer_mip10 = new colour[width / 1024 * height / 1024];
			graphics::colour* buffer_mip11 = new colour[width / 2048 * height / 2048];
			graphics::colour* buffer_mip12 = new colour[width / 4096 * height / 4096];
			graphics::colour* buffer_array[]{ buffer,
				buffer_mip01, buffer_mip02, buffer_mip03, buffer_mip04, buffer_mip05, buffer_mip06,
				buffer_mip07, buffer_mip08, buffer_mip09, buffer_mip10, buffer_mip11, buffer_mip12 };
			int division = 2;
			for (int i = 1; i < 13; ++i) {
				for (int x = 0; x < width / division; ++x) {
					for (int y = 0; y < height / division; ++y) {
						buffer_array[i][x * (width / division) + y].r = buffer[(x * division) * width + (y * division)].r;
						buffer_array[i][x * (width / division) + y].g = buffer[(x * division) * width + (y * division)].g;
						buffer_array[i][x * (width / division) + y].b = buffer[(x * division) * width + (y * division)].b;
						buffer_array[i][x * (width / division) + y].a = buffer[(x * division) * width + (y * division)].a;
					}
				}
				glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width / division, height / division, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_array[i]);
				division *= 2;
			}
			for (int i = 1; i < 13; ++i)
				delete[] buffer_array[i];
		}
		else if (fm == eFOLIAGE) {
			graphics::colour* buffer_mip01 = new colour[width / 2 * height / 2];
			graphics::colour* buffer_mip02 = new colour[width / 4 * height / 4];
			graphics::colour* buffer_mip03 = new colour[width / 8 * height / 8];
			graphics::colour* buffer_mip04 = new colour[width / 16 * height / 16];
			graphics::colour* buffer_mip05 = new colour[width / 32 * height / 32];
			graphics::colour* buffer_mip06 = new colour[width / 64 * height / 64];
			graphics::colour* buffer_mip07 = new colour[width / 128 * height / 128];
			graphics::colour* buffer_mip08 = new colour[width / 256 * height / 256];
			graphics::colour* buffer_mip09 = new colour[width / 512 * height / 512];
			graphics::colour* buffer_mip10 = new colour[width / 1024 * height / 1024];
			graphics::colour* buffer_mip11 = new colour[width / 2048 * height / 2048];
			graphics::colour* buffer_mip12 = new colour[width / 4096 * height / 4096];
			graphics::colour* buffer_array[]{ buffer,
				buffer_mip01, buffer_mip02, buffer_mip03, buffer_mip04, buffer_mip05, buffer_mip06,
				buffer_mip07, buffer_mip08, buffer_mip09, buffer_mip10, buffer_mip11, buffer_mip12 };
			int division = 2;
			for (int i = 1; i < 13; ++i) {
				for (int x = 0; x < width / division; ++x) {
					for (int y = 0; y < height / division; ++y) {
						buffer_array[i][x * (width / division) + y].r = buffer[(x * division) * width + (y * division)].r;
						buffer_array[i][x * (width / division) + y].g = buffer[(x * division) * width + (y * division)].g;
						buffer_array[i][x * (width / division) + y].b = buffer[(x * division) * width + (y * division)].b;
						// Get minimum alpha
						#if DEF_CUSTOM_MIPMAP_FOLIAGE_MIN
						buffer_array[i][x * (width / division) + y].a = m::Min<lui8>(4u,
							#else
						buffer_array[i][x * (width / division) + y].a = m::Max<lui8>(4u,
							#endif
							buffer_array[i - 1][(x * 2)     * (width / (division / 2)) + (y * 2)].a,
							buffer_array[i - 1][(x * 2 + 1) * (width / (division / 2)) + (y * 2)].a,
							buffer_array[i - 1][(x * 2)     * (width / (division / 2)) + (y * 2 + 1)].a,
							buffer_array[i - 1][(x * 2 + 1) * (width / (division / 2)) + (y * 2 + 1)].a);
					}
				}
				glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width / division, height / division, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_array[i]);
				division *= 2;
			}
			for (int i = 1; i < 13; ++i)
				delete[] buffer_array[i];
		}
		#endif

		#if DEF_OLDSKOOL
		if (fm == eLINEAR) fm = eNEAREST;
		if (fm == eLINEAR_MIPMAP) fm = eNEAREST_MIPMAP;
		#endif
		SetFilterMode[fm]();
		SetEdgeMode[em]();

		#if !DEF_SWR
		delete[] buffer;
		#endif
	}
	void Texture::InitRenderBuffer(GLuint fbuf, int x, int y, bool linear) {
		// Bind the framebuffer so we can set this texture to belong to it
		glBindFramebuffer(GL_FRAMEBUFFER, fbuf);

		width = x;
		height = y;

		// The texture we're going to render to
		//unsigned int rendertexture;
		glGenTextures(1, &glID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#if DEF_MULTISAMPLE // Multisampled
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#if DEF_HDR // HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA16F, x, y, GL_TRUE); //create a blank image
		#else // Not HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA, x, y, GL_TRUE); //create a blank image
		#endif
		#else // Not Multisampled
		glBindTexture(GL_TEXTURE_2D, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#if DEF_HDR // HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, x, y, 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
		#else // Not HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //create a blank image
		#endif
		#endif
		EMClamp();
		linear ? FMLinear() : FMNearest();
		//attach this texture to the framebuffer
		#if DEF_MULTISAMPLE
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, glID, 0);
		#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glID, 0);
		#endif
	}
	void Texture::InitIntermediateTest(GLuint fbuf) {
		// Bind the framebuffer so we can set this texture to belong to it
		glBindFramebuffer(GL_FRAMEBUFFER, fbuf);
		// create a color attachment texture
		glGenTextures(1, &glID);
		glBindTexture(GL_TEXTURE_2D, glID);
		#if DEF_HDR // HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
		#else // Not HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		#if DEF_LINEAR_FB
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		#endif
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glID, 0);	// we only need a color buffer
		#if DEF_MULTISAMPLE
		glEnable(GL_MULTISAMPLE); // Enable multisampling for anti-aliasing
		#if DEF_MULTISAMPLE_ATOC
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); // Use multisampling for texture alpha
		#endif
		#endif
	}
	void Texture::InitDepthBufferRW(GLuint fbuf, int x, int y, bool linear) {
		// Bind the framebuffer so we can set this texture to belong to it
		glBindFramebuffer(GL_FRAMEBUFFER, fbuf);

		width = x;
		height = y;

		glGenTextures(1, &glID);

		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (linear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		//attach this texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glID, 0);
	}
	void Texture::InitDepthBufferW(GLuint fbuf, int x, int y, bool linear) {
		// Bind the framebuffer so we can set this texture to belong to it
		glBindFramebuffer(GL_FRAMEBUFFER, fbuf);

		width = x;
		height = y;

		//render buffer object (depth)
		glGenRenderbuffers(1, &glID);
		glBindRenderbuffer(GL_RENDERBUFFER, glID);
		#if DEF_MULTISAMPLE
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, DEF_MULTISAMPLE_DEPTH, GL_DEPTH24_STENCIL8, x, y);
		#else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);
		#endif

		//Once we've allocated enough memory for the renderbuffer object we can unbind the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//bind rbo to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glID);
	}
	void Texture::InitShadowBuffer(GLuint fbuf) {
		// Bind the framebuffer so we can set this texture to belong to it
		glBindFramebuffer(GL_FRAMEBUFFER, fbuf);

		//glGenFramebuffers(1, &framebuffer_shadow);
		glGenTextures(1, &glID);
		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_RESOLUTION, SHADOW_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Texture::Unload() {
		glDeleteTextures(1, &glID);
		#if DEF_SWR
		delete[] buffer2;
		#endif
	}

	//________________________________________________________________________________________________________________________________
	// MESH --------------------------------------------------------------------------------------------------------------------------

	void Mesh::LoadFile(void* file, bool clearmem) {
		//std::cout << "Loading " << fn << "... ";

		//Vertex* vces; // Vertices
		//lui32* ices; // Indices

		//-------------------------------- OPEN FILE

		FILE* in = (FILE*)file;

		//Vert* vces; // Vertices
		//size_t vces_size;
		//lui32* ices; // Indices
		//size_t ices_size;

		version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

		//-------------------------------- READ VERTICES

		fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
		vces = (Vertex*)malloc(sizeof(Vertex) * vces_size); // Allocate buffer to hold our vertices
		fread(&vces[0], sizeof(Vertex), vces_size, in); // Read vertices

		//-------------------------------- READ INDICES

		fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
		ices = (lui32*)malloc(sizeof(lui32) * ices_size); // Allocate buffer to hold our indicess
		fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

		//glID = (GLuint)ices_size; // Set number of indices used in Draw()

		//-------------------------------- INITIALIZE OPENGL BUFFER

		glGenVertexArrays(1, &vao); // Create vertex buffer
		glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VI_POS); // Set Vertex positions
		glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
		glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
		glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
		glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
		glEnableVertexAttribArray(VI_COL); // Set Vertex colour
		glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);

		glBindVertexArray(0); // Bind default vertex array

		if (clearmem) {
			free(vces);
			vces = nullptr;
			free(ices);
			ices = nullptr;
		}

		std::cout << "Generated Mesh!" << std::endl;
	}
	void Mesh::Unload() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		if (vces != nullptr) free(vces);
		if (ices != nullptr) free(ices);
	}

	//________________________________________________________________________________________________________________________________
	// MESH BLEND --------------------------------------------------------------------------------------------------------------------

	void MeshBlend::LoadFile(void* file) {
		//std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = (FILE*)file;

		VertexBlend* vces; // Vertices
		size_t vces_size;
		lui32* ices; // Indices

		version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

		//-------------------------------- READ VERTICES

		fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
		vces = (VertexBlend*)malloc(sizeof(VertexBlend) * vces_size); // Allocate buffer to hold our vertices
		fread(&vces[0], sizeof(VertexBlend), vces_size, in); // Read vertices

		//-------------------------------- READ INDICES

		fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
		ices = (lui32*)malloc(sizeof(lui32) * ices_size); // Allocate buffer to hold our indicess
		fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

		ices_size = (GLuint)ices_size; // Set number of indices used in Draw()

		//-------------------------------- INITIALIZE OPENGL BUFFER

		glGenVertexArrays(1, &vao); // Create vertex buffer
		glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexBlend), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(vbi_pos_a); // Set Vertex positions
		glVertexAttribPointer(vbi_pos_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_pos_a);
		glEnableVertexAttribArray(vbi_pos_b);
		glVertexAttribPointer(vbi_pos_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_pos_b);

		glEnableVertexAttribArray(vbi_nor_a); // Set Vertex normals
		glVertexAttribPointer(vbi_nor_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_nor_a);
		glEnableVertexAttribArray(vbi_nor_b);
		glVertexAttribPointer(vbi_nor_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_nor_b);

		glEnableVertexAttribArray(vbi_uvc); // Set Vertex texture coords
		glVertexAttribPointer(vbi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_uvc);

		glEnableVertexAttribArray(vbi_col); // Set Vertex colour
		glVertexAttribPointer(vbi_col, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_col);

		glBindVertexArray(0); // Bind default vertex array
		free(vces);
		free(ices);

		std::cout << "Generated Mesh Blend!" << std::endl;
	}
	void MeshBlend::Unload() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	//________________________________________________________________________________________________________________________________
	// MESH SET ----------------------------------------------------------------------------------------------------------------------
	
	void MeshSet::LoadFile(void* file)
	{
		//-------------------------------- OPEN FILE

		FILE* in = (FILE*)file;

		#if DEF_MESHSET_BLEND
		VertexBlend* vces[MESHSET_MAX_COUNT]; // Vertices
		#else
		Vertex* vces[MESHSET_MAX_COUNT]; // Vertices
		#endif
		lui32 vces_size;
		lui32* ices; // Indices
		//lui32 ices_size;

		version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

		while (true) {
			lui8 temp;
			fread(&temp, 1, 1, in);
			// If there is a mesh here
			if (temp == 1u) {
				// Read these once only
				if (count == 0u) {
					// Read vertex/index counts
					fread(&ices_size, 4, 1, in);
					fread(&vces_size, 4, 1, in);

					#if DEF_MESHSET_BLEND
					vces[count] = (VertexBlend*)malloc(sizeof(VertexBlend) * vces_size); // Allocate buffer to hold our vertices
					#else
					vces[count] = (Vertex*)malloc(sizeof(Vertex) * vces_size); // Allocate buffer to hold our vertices
					#endif
					ices = (lui32*)malloc(sizeof(lui32) * ices_size); // Allocate buffer to hold our indicess

					// read other stuff
					for (int i = 0; i < ices_size; i++) {
						fread(&ices[i], 4, 1, in);
					}
					for (int i = 0; i < vces_size; i++) {
						fread(&vces[count][i].uvc, sizeof(vec2), 1, in);
						fread(&vces[count][i].col, sizeof(vec4), 1, in); // Colour is useless, OBJ meshes won't have them anyway
					}
				}
				else {
					#if DEF_MESHSET_BLEND
					vces[count] = (VertexBlend*)malloc(sizeof(VertexBlend) * vces_size); // Allocate buffer to hold our vertices
					#else
					vces[count] = (Vertex*)malloc(sizeof(Vertex) * vces_size); // Allocate buffer to hold our vertices
					#endif
					// Copy duplicated values (decompression)
					memcpy(&vces[count][0], &vces[0][0], sizeof(Vertex) * vces_size);
				}
				// Read every time
				for (int i = 0; i < vces_size; i++) {
					#if DEF_MESHSET_BLEND
					fread(&vces[count][i].pos_a, sizeof(vec3), 1, in);
					fread(&vces[count][i].nor_a, sizeof(vec3), 1, in);
					#else
					fread(&vces[count][i].pos, sizeof(vec3), 1, in);
					fread(&vces[count][i].nor, sizeof(vec3), 1, in);
					#endif
				}
				++count;
			}
			// No meshes left
			else {
				break;
			}
		}

		//-------------------------------- SET BLEND VALUES

		#if DEF_MESHSET_BLEND
		for (int i = 0; i < count; ++i) {
			int i_next = (i + 1) % count;
			for (int j = 0; j < vces_size; ++j) {
				vces[i][j].pos_b = vces[i_next][j].pos_a;
				vces[i][j].nor_b = vces[i_next][j].nor_a;
			}
		}
		#endif

		//-------------------------------- INITIALIZE OPENGL BUFFERS

		for (int i = 0; i < count; ++i) {
			glGenVertexArrays(1, &vao[i]); // Create vertex buffer
			glGenBuffers(1, &vbo[i]); glGenBuffers(1, &ebo[i]); // Generate vertex and element buffer

			glBindVertexArray(vao[i]); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo[i]); // Create vertex buffer in opengl
			#if DEF_MESHSET_BLEND
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexBlend), &vces[i][0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			#else
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[i][0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			#endif
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			#if DEF_MESHSET_BLEND
			glEnableVertexAttribArray(vbi_pos_a); // Set Vertex positions
			glVertexAttribPointer(vbi_pos_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_pos_a);
			glEnableVertexAttribArray(vbi_pos_b);
			glVertexAttribPointer(vbi_pos_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_pos_b);
			glEnableVertexAttribArray(vbi_nor_a); // Set Vertex normals
			glVertexAttribPointer(vbi_nor_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_nor_a);
			glEnableVertexAttribArray(vbi_nor_b);
			glVertexAttribPointer(vbi_nor_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_nor_b);
			glEnableVertexAttribArray(vbi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(vbi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_uvc);
			glEnableVertexAttribArray(vbi_col); // Set Vertex colour
			glVertexAttribPointer(vbi_col, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)vb_col);
			#else
			glEnableVertexAttribArray(VI_POS); // Set Vertex positions
			glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
			glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
			glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
			glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
			glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
			glEnableVertexAttribArray(VI_COL); // Set Vertex colour
			glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);
			#endif
		}

		glBindVertexArray(0); // Bind default vertex array

		for (int i = 0; i < count; ++i)
			free(vces[i]);
		free(ices);

		std::cout << "Generated Mesh Set!" << std::endl;
	}
	void MeshSet::Unload()
	{
		for (int i = 0; i < count; ++i) {
			glDeleteVertexArrays(1, &vao[i]);
			glDeleteBuffers(1, &vbo[i]);
			glDeleteBuffers(1, &ebo[i]);
		}
	}

	//________________________________________________________________________________________________________________________________
	// MESH DEFORM -------------------------------------------------------------------------------------------------------------------

	void MeshDeform::LoadFile(void* file) {
		//std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = (FILE*)file;

		VertexDeform* vces; // Vertices
		size_t vces_size;
		lui32* ices; // Indices

		version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

		//-------------------------------- READ VERTICES

		fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
		vces = (VertexDeform*)malloc(sizeof(VertexDeform) * vces_size); // Allocate buffer to hold our vertices
		fread(&vces[0], sizeof(VertexDeform), vces_size, in); // Read vertices

		//-------------------------------- READ INDICES

		fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
		ices = (lui32*)malloc(sizeof(lui32) * ices_size); // Allocate buffer to hold our indicess
		fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

		ices_size = (GLuint)ices_size; // Set number of indices used in Draw()

		//-------------------------------- INITIALIZE OPENGL BUFFER

		glGenVertexArrays(1, &vao); // Create vertex buffer
		glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexDeform), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(vdi_pos); // Set Vertex positions
		glVertexAttribPointer(vdi_pos, 3u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_pos);
		glEnableVertexAttribArray(vdi_nor); // Set Vertex normals
		glVertexAttribPointer(vdi_nor, 3u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_nor);
		glEnableVertexAttribArray(vdi_uvc); // Set Vertex texture coords
		glVertexAttribPointer(vdi_uvc, 2u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_uvc);
		glEnableVertexAttribArray(vdi_col); // Set Vertex colour
		glVertexAttribPointer(vdi_col, 4u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_col);
		// Set the matrix value array
		for (lui32 i = 0u; i < MD_MATRIX_COUNT; ++i) {
			glEnableVertexAttribArray(vdi_mat + i); // Set Vertex colour
			glVertexAttribPointer(vdi_mat + i, 1u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)(vd_mat + (sizeof(lf32) * i)));
		}

		glBindVertexArray(0); // Bind default vertex array
		free(vces);
		free(ices);

		std::cout << "Generated Mesh Deform!" << std::endl;
	}
	void MeshDeform::Unload() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	//________________________________________________________________________________________________________________________________
	// COMPOSITE MESH ----------------------------------------------------------------------------------------------------------------

	CompositeMesh::CompositeMesh() {
		//int i = 0;
	}
	CompositeMesh::~CompositeMesh() {
		if (vces != nullptr)
			delete[] vces;
		if (ices != nullptr)
			delete[] ices;
	}
	void CompositeMesh::Draw(unsigned int tex, unsigned int shd) {
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, (GLsizei)ices_size, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}
	// TODO: add matrix offset
	// read/search: c++ multiply vector3 x matrix4x4
	void CompositeMesh::AddMesh(Mesh* mesh, Matrix4x4 position) {
		//
		size_t old_vces_size = vces_size;
		size_t old_ices_size = ices_size;
		//
		vces_size += mesh->VcesSize();
		ices_size += mesh->IcesSize();
		//
		Vertex* vces_new = new Vertex[vces_size];
		lui32* ices_new = new lui32[ices_size];
		// copy existing vertices into the new buffer
		for (int i = 0; i < old_vces_size; ++i)
			vces_new[i] = vces[i];
		for (int i = 0; i < old_ices_size; ++i)
			ices_new[i] = ices[i];
		// Copy in the things from the new mesh
		for (size_t i = old_vces_size; i < vces_size; ++i) {
			vces_new[i] = mesh->Vces()[i - old_vces_size];
			// temp
			m::Vector3 vector = m::Vector3(vces_new[i].pos.x, vces_new[i].pos.y, vces_new[i].pos.z);
			vector = vector * position;
			vces_new[i].pos.x = vector.x;
			vces_new[i].pos.y = vector.y;
			vces_new[i].pos.z = vector.z;
		}
		for (size_t i = old_ices_size; i < ices_size; ++i) {
			ices_new[i] = mesh->Ices()[i - old_ices_size] + (lui32)old_vces_size;
		}
		// Clear the old buffers from memory
		delete[] vces; vces = vces_new;
		delete[] ices; ices = ices_new;

		//-------------------------------- INITIALIZE OPENGL BUFFER

		if (old_vces_size == 0) {
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}

		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VI_POS); // Set Vertex positions
		glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
		glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
		glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
		glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
		glEnableVertexAttribArray(VI_COL); // Set Vertex colour
		glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);

		glBindVertexArray(0); // Bind default vertex array
	}

	void CompositeMesh::AddMesh(Mesh* mesh, m::Vector3 position, MeshOrientation ori) {
		//
		size_t old_vces_size = vces_size;
		size_t old_ices_size = ices_size;
		//
		vces_size += mesh->VcesSize();
		ices_size += mesh->IcesSize();
		//
		Vertex* vces_new = new Vertex[vces_size];
		lui32* ices_new = new lui32[ices_size];
		// copy existing vertices into the new buffer
		for (int i = 0; i < old_vces_size; ++i)
			vces_new[i] = vces[i];
		for (int i = 0; i < old_ices_size; ++i)
			ices_new[i] = ices[i];
		// Copy in the things from the new mesh
		for (size_t i = old_vces_size; i < vces_size; ++i) {
			vces_new[i] = mesh->Vces()[i - old_vces_size];
			// temp
			m::Vector3 vector = m::Vector3(vces_new[i].pos.x, vces_new[i].pos.y, vces_new[i].pos.z);
			m::Vector3 normal = m::Vector3(vces_new[i].nor.x, vces_new[i].nor.y, vces_new[i].nor.z);

			//vector.y += m::Lerp(height_s, height_n, vces_2[i].pos.z * 2.f) + m::Lerp(height_w, height_e, vces_2[i].pos.x * 2.f);
			//vector.y *= 0.5f;
			//vector.z += vces_2[i].pos.x;

			// BILINEAR
			/*
			out_height = m::Lerp(
			m::Lerp((lf32)eCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].height, (lf32)eCells[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y].height, abs(csinf.offsetx)),
			m::Lerp((lf32)eCells[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y].height, (lf32)eCells[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y].height, abs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
			*/

			switch (ori) {
			case graphics::CompositeMesh::eNORTH:
				vces_new[i].pos.x = vector.x + position.x;
				vces_new[i].pos.y = vector.y + position.y;
				vces_new[i].pos.z = vector.z + position.z;
				break;
			case graphics::CompositeMesh::eSOUTH:
				vces_new[i].pos.x = -vector.x + position.x;
				vces_new[i].pos.y = vector.y + position.y;
				vces_new[i].pos.z = -vector.z + position.z;
				vces_new[i].nor.x = -normal.x;
				vces_new[i].nor.z = -normal.z;
				break;
			case graphics::CompositeMesh::eEAST:
				vces_new[i].pos.x = vector.z + position.x;
				vces_new[i].pos.y = vector.y + position.y;
				vces_new[i].pos.z = -vector.x + position.z;
				vces_new[i].nor.x = normal.z;
				vces_new[i].nor.z = -normal.x;
				break;
			case graphics::CompositeMesh::eWEST:
				vces_new[i].pos.x = -vector.z + position.x;
				vces_new[i].pos.y = vector.y + position.y;
				vces_new[i].pos.z = vector.x + position.z;
				vces_new[i].nor.x = -normal.z;
				vces_new[i].nor.z = normal.x;
				break;
			default:
				break;
			}
		}
		for (size_t i = old_ices_size; i < ices_size; ++i)
			ices_new[i] = mesh->Ices()[i - old_ices_size] + (lui32)old_vces_size;
		// Clear the old buffers from memory
		delete[] vces; vces = vces_new;
		delete[] ices; ices = ices_new;

		//-------------------------------- INITIALIZE OPENGL BUFFER

		if (old_vces_size == 0) {
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}
	}

	void CompositeMesh::ReBindGL() {
		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VI_POS); // Set Vertex positions
		glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
		glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
		glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
		glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
		glEnableVertexAttribArray(VI_COL); // Set Vertex colour
		glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);

		glBindVertexArray(0); // Bind default vertex array
	}
	void CompositeMesh::Unload() {
		if (vces != nullptr) delete[] vces;
		if (vces != nullptr) delete[] ices;
	}

	//________________________________________________________________________________________________________________________________
	// TERRAIN MESH ------------------------------------------------------------------------------------------------------------------

	void TerrainMesh::Draw() {
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, (GLsizei)ices_size, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}

	void TerrainMesh::GenerateFromHMap(
		lui8(&hmap)[WORLD_SIZE][WORLD_SIZE],
		lui8(&MATMAP)[WORLD_SIZE][WORLD_SIZE]) {

		int tile_radius = 128;

		vces_size = (tile_radius * tile_radius);
		ices_size = 6u * (tile_radius * tile_radius);

		vces = (VertexTerrain*)malloc(sizeof(VertexTerrain) * vces_size);
		ices = (lui32*)malloc(sizeof(lui32) * ices_size);

		//lf32 uvscale = 0.125f;
		lf32 uvscale = 0.25f;
		int v = 0;
		int i = 0;
		for (int x = 1024 - (tile_radius / 2); x < 1024 + (tile_radius / 2); ++x) {
			for (int y = 1024 - (tile_radius / 2); y < 1024 + (tile_radius / 2); ++y, v += 1, i += 6) {
				// Copy in the things from the new mesh
				vces[v].pos.x = (lf32)x;
				vces[v].pos.z = (lf32)y;
				vces[v].pos.y = (((lf32)hmap[x][y]) + (lf32)hmap[x][y]) / 2 / TERRAIN_HEIGHT_DIVISION;
				vces[v].uvc.x = vces[v].pos.x * uvscale;
				vces[v].uvc.y = vces[v].pos.z * uvscale;

				vces[v].nor.y = 1.f; vces[v].nor.x = 0.f; vces[v].nor.z = 1.f;
				vces[v].nor.x += (lf32)(hmap[x][y] - hmap[x + 1][y]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
				vces[v].nor.x -= (lf32)(hmap[x][y] - hmap[x - 1][y]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
				vces[v].nor.z += (lf32)(hmap[x][y] - hmap[x][y + 1]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
				vces[v].nor.z -= (lf32)(hmap[x][y] - hmap[x][y - 1]) / (TERRAIN_HEIGHT_DIVISION * 2.f);

				// normalize
				lf32 nlen = sqrt(vces[v].nor.x * vces[v].nor.x + vces[v].nor.y * vces[v].nor.y + vces[v].nor.z * vces[v].nor.z);
				if (nlen != 0) {
					vces[v].nor.x = vces[v].nor.x / nlen;
					vces[v].nor.y = vces[v].nor.y / nlen;
					vces[v].nor.z = vces[v].nor.z / nlen;
				}

				vces[v].txtr[0] = 0.f;
				vces[v].txtr[1] = 0.f;
				vces[v].txtr[2] = 0.f;
				vces[v].txtr[3] = 0.f;
				vces[v].txtr[4] = 0.f;
				vces[v].txtr[5] = 0.f;
				vces[v].txtr[6] = 0.f;
				vces[v].txtr[7] = 0.f;

				vces[v].txtr[MATMAP[x][y]] = 1.f;

				// r--r+1
				// |\ |
				// | \|
				// 0--1

				if (x != 1023 + (tile_radius / 2) && y != 1023 + (tile_radius / 2)) {
					bool triangulate_alternate = false;

					// Triangulate based on longest height difference
					#if DEF_TERRAIN_USE_EROSION_TRIANGULATION
					if (abs((int)hmap[x][y] - (int)hmap[x + 1][y + 1]) < abs((int)hmap[x + 1][y] - (int)hmap[x][y + 1]))
						triangulate_alternate = true;
					#else
					if (abs((int)hmap[x][y] - (int)hmap[x + 1][y + 1]) > abs((int)hmap[x + 1][y] - (int)hmap[x][y + 1]))
						triangulate_alternate = true;
					#endif

					if (triangulate_alternate) {
						ices[i + 0u] = v + 0u;
						ices[i + 1u] = v + tile_radius;
						ices[i + 2u] = v + 1u;
						ices[i + 3u] = v + 1u;
						ices[i + 4u] = v + tile_radius;
						ices[i + 5u] = v + tile_radius + 1u;
					}
					else
					{
						ices[i + 0u] = v + 0u;
						ices[i + 1u] = v + tile_radius;
						ices[i + 2u] = v + tile_radius + 1u;
						ices[i + 3u] = v + tile_radius + 1u;
						ices[i + 4u] = v + 1u;
						ices[i + 5u] = v + 0u;
					}
				}
				else
				{
					ices[i + 0u] = 0u;
					ices[i + 1u] = 0u;
					ices[i + 2u] = 0u;
					ices[i + 3u] = 0u;
					ices[i + 4u] = 0u;
					ices[i + 5u] = 0u;
				}
			}
		}

		//-------------------------------- INITIALIZE OPENGL BUFFER

		// Initial check prevents regeneration memory leak
		if (vao == UI32_NULL) glGenVertexArrays(1, &vao); // Create vertex buffer
		if (vbo == UI32_NULL) glGenBuffers(1, &vbo); // Generate vertex buffer
		if (ebo == UI32_NULL) glGenBuffers(1, &ebo); // Generate element buffer

		ReBindGL();

		// Free vertex and index buffers!
		free((void*)vces);
		free((void*)ices);
	}

	enum TerrainGenData : lui8
	{
		eTGEN_FACE_N_COR_E = 1u,
		eTGEN_FACE_N_COR_W = 1u << 1u,
		eTGEN_FACE_E_COR_N = 1u << 2u,
		eTGEN_FACE_E_COR_S = 1u << 3u,
	};

	void TerrainMesh::GenerateComplexEnv(
		lui8(&hmap)[WORLD_SIZE][WORLD_SIZE],
		lui8(&MATMAP)[WORLD_SIZE][WORLD_SIZE],
		lui32* flags,
		lui32 flag_invisible,
		lui8(&hmap_ne)[WORLD_SIZE][WORLD_SIZE],
		lui8(&hmap_nw)[WORLD_SIZE][WORLD_SIZE],
		lui8(&hmap_se)[WORLD_SIZE][WORLD_SIZE],
		lui8(&hmap_sw)[WORLD_SIZE][WORLD_SIZE]) {

		int tile_radius = 128;

		lui8 tgendata[128][128];
		memset(tgendata, 0u, 128 * 128);

		#define BVGET(x, y, flag) (flags[x * WORLD_SIZE + y] & flag)

		int face_VertexCount = 0;
		int face_IndexCount = 0;
		int edge_VertexCount = 0;
		int edge_IndexCount = 0;
		{ // Scope
			int datax = 0;
			// Count edges that need tris
			for (int x = 1024 - (tile_radius / 2); x < 1024 + (tile_radius / 2); ++x, ++datax) {
				int datay = 0;
				for (int y = 1024 - (tile_radius / 2); y < 1024 + (tile_radius / 2); ++y, ++datay) {
					bool bInvisible = BVGET(x, y, flag_invisible) == 1;

					// if solid face
					if (!bInvisible) {
						face_VertexCount += 4;
						face_IndexCount += 6;
					}

					// (Ramp ends only need 1 additional triangle, so each corner separation is counted separately)
					//-------------------------------- North edge check
					if (hmap_ne[x][y] != hmap_se[x][y + 1])
						tgendata[datax][datay] |= eTGEN_FACE_N_COR_E;
					if (hmap_nw[x][y] != hmap_sw[x][y + 1])
						tgendata[datax][datay] |= eTGEN_FACE_N_COR_W;
					// If both corners require new faces
					if ((tgendata[datax][datay] & eTGEN_FACE_N_COR_E) != 0 && (tgendata[datax][datay] & eTGEN_FACE_N_COR_W) != 0) {
						edge_IndexCount += 6;
						edge_VertexCount += 4;
					}
					// If only one corner requires a face
					else if ((tgendata[datax][datay] & eTGEN_FACE_N_COR_E) != 0 || (tgendata[datax][datay] & eTGEN_FACE_N_COR_W) != 0) {
						edge_IndexCount += 3;
						edge_VertexCount += 3;
					}
					//-------------------------------- East edge check
					if (hmap_ne[x][y] != hmap_nw[x + 1][y])
						tgendata[datax][datay] |= eTGEN_FACE_E_COR_N;
					if (hmap_se[x][y] != hmap_sw[x + 1][y])
						tgendata[datax][datay] |= eTGEN_FACE_E_COR_S;
					// If both corners require new faces
					if ((tgendata[datax][datay] & eTGEN_FACE_E_COR_N) != 0 && (tgendata[datax][datay] & eTGEN_FACE_E_COR_S) != 0) {
						edge_IndexCount += 6;
						edge_VertexCount += 4;
					}
					// If only one corner requires a face
					else if ((tgendata[datax][datay] & eTGEN_FACE_E_COR_N) != 0 || (tgendata[datax][datay] & eTGEN_FACE_E_COR_S) != 0) {
						edge_IndexCount += 3;
						edge_VertexCount += 3;
					}
				}
			}
		}

		// Assign memory to vertex and index counts
		/*vces_size = 4u * (tile_radius * tile_radius) + edge_VertexCount;
		ices_size = 6u * (tile_radius * tile_radius) + edge_IndexCount;*/
		vces_size = face_VertexCount + edge_VertexCount;
		ices_size = face_IndexCount + edge_IndexCount;

		vces = (VertexTerrain*)malloc(sizeof(VertexTerrain) * vces_size);
		ices = (lui32*)malloc(sizeof(lui32) * ices_size);

		int v = 0;
		int i = 0;
		int datax = 0;
		for (int x = 1024 - (tile_radius / 2); x < 1024 + (tile_radius / 2); ++x, ++datax) {
			int datay = 0;
			for (int y = 1024 - (tile_radius / 2); y < 1024 + (tile_radius / 2); ++y, ++datay) {
				//-------------------------------- Generate top face
				// if top face is visible
				if (!BVGET(x, y, flag_invisible)) {
					//ne
					vces[v].pos.x = (lf32)x + 0.5f;
					vces[v].pos.z = (lf32)y + 0.5f;
					vces[v].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 1u].pos.x = (lf32)x - 0.5f;
					vces[v + 1u].pos.z = (lf32)y + 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_nw[x][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 2u].pos.x = (lf32)x + 0.5f;
					vces[v + 2u].pos.z = (lf32)y - 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_se[x][y] / TERRAIN_HEIGHT_DIVISION;
					//sw
					vces[v + 3u].pos.x = (lf32)x - 0.5f;
					vces[v + 3u].pos.z = (lf32)y - 0.5f;
					vces[v + 3u].pos.y = (lf32)hmap_sw[x][y] / TERRAIN_HEIGHT_DIVISION;

					for (lui32 i = 0; i < 4; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.x * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.z * TERRAIN_UV_SCALE;
						vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;
						//vces[v + i].nor.x += (lf32)(hmap[x][y] - hmap[x + 1][y]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
						//vces[v + i].nor.x -= (lf32)(hmap[x][y] - hmap[x - 1][y]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
						//vces[v + i].nor.z += (lf32)(hmap[x][y] - hmap[x][y + 1]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
						//vces[v + i].nor.z -= (lf32)(hmap[x][y] - hmap[x][y - 1]) / (TERRAIN_HEIGHT_DIVISION * 2.f);
					}

					// normalize
					lf32 nlen = sqrt(vces[v].nor.x * vces[v].nor.x + vces[v].nor.y * vces[v].nor.y + vces[v].nor.z * vces[v].nor.z);
					if (nlen != 0) {
						vces[v].nor.x = vces[v].nor.x / nlen;
						vces[v].nor.y = vces[v].nor.y / nlen;
						vces[v].nor.z = vces[v].nor.z / nlen;
					}

					for (int i = 0; i < 4; ++i) {
						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x - 1][y]] = 1.f; //nw
					vces[v + 2].txtr[MATMAP[x][y - 1]] = 1.f; //se
					vces[v + 3].txtr[MATMAP[x - 1][y - 1]] = 1.f; //sw

					bool triangulate_alternate = false;
					// | FALSE | TRUE
					// | 1--0  | 1--0
					// | | /|  | |\ |
					// | |/ |  | | \|
					// | 3--2  | 3--2

					// Triangulate based on longest height difference
					#if DEF_TERRAIN_USE_EROSION_TRIANGULATION
					//if (abs((int)hmap[x][y] - (int)hmap[x + 1][y + 1]) < abs((int)hmap[x + 1][y] - (int)hmap[x][y + 1]))
					if (abs((int)hmap_ne[x][y] - (int)hmap_sw[x][y]) < abs((int)hmap_nw[x][y] - (int)hmap_se[x][y]))
						triangulate_alternate = true;
					#else
					if (abs((int)hmap[x][y] - (int)hmap[x + 1][y + 1]) > abs((int)hmap[x + 1][y] - (int)hmap[x][y + 1]))
						triangulate_alternate = true;
					#endif

					if (triangulate_alternate) {
						ices[i + 0u] = v + 0u; ices[i + 1u] = v + 1u; ices[i + 2u] = v + 2u;
						ices[i + 3u] = v + 1u; ices[i + 4u] = v + 3u; ices[i + 5u] = v + 2u;
					}
					else {
						ices[i + 0u] = v + 0u; ices[i + 1u] = v + 1u; ices[i + 2u] = v + 3u;
						ices[i + 3u] = v + 0u; ices[i + 4u] = v + 3u; ices[i + 5u] = v + 2u;
					}

					i += 6;
					v += 4;
				}

				// Edge faces

				//-------------------------------- Generate north face
				// If both corners require new faces
				if ((tgendata[datax][datay] & eTGEN_FACE_N_COR_E) != 0 && (tgendata[datax][datay] & eTGEN_FACE_N_COR_W) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 1u].pos.x = (lf32)x - 0.5f; vces[v + 1u].pos.z = (lf32)y + 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_nw[x][y] / TERRAIN_HEIGHT_DIVISION;
					//ne
					vces[v + 2u].pos.x = (lf32)x + 0.5f; vces[v + 2u].pos.z = (lf32)y + 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_se[x][y + 1] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 3u].pos.x = (lf32)x - 0.5f; vces[v + 3u].pos.z = (lf32)y + 0.5f;
					vces[v + 3u].pos.y = (lf32)hmap_sw[x][y + 1] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 4; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.x * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;

						if (vces[v + 0u].pos.y < vces[v + 2u].pos.y) {
							vces[v + i].nor.y = 0.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = -1.f;
						}
						else {
							vces[v + i].nor.y = 0.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 1.f;
						}

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x - 1][y]] = 1.f; //nw
					vces[v + 2].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 3].txtr[MATMAP[x - 1][y]] = 1.f; //nw

					ices[i + 0u] = v + 0u;
					ices[i + 1u] = v + 2u;
					ices[i + 2u] = v + 1u;
					ices[i + 3u] = v + 1u;
					ices[i + 4u] = v + 2u;
					ices[i + 5u] = v + 3u;

					i += 6;
					v += 4;
				}
				// If only one corner requires a face
				else if ((tgendata[datax][datay] & eTGEN_FACE_N_COR_E) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 1u].pos.x = (lf32)x - 0.5f; vces[v + 1u].pos.z = (lf32)y + 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_nw[x][y] / TERRAIN_HEIGHT_DIVISION;
					//ne
					vces[v + 2u].pos.x = (lf32)x + 0.5f; vces[v + 2u].pos.z = (lf32)y + 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_se[x][y + 1] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 3; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.x * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;
						vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x - 1][y]] = 1.f; //nw
					vces[v + 2].txtr[MATMAP[x][y]] = 1.f; //ne

					ices[i + 0u] = v + 0u;
					ices[i + 1u] = v + 2u;
					ices[i + 2u] = v + 1u;

					i += 3;
					v += 3;
				}
				else if ((tgendata[datax][datay] & eTGEN_FACE_N_COR_W) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 1u].pos.x = (lf32)x - 0.5f; vces[v + 1u].pos.z = (lf32)y + 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_nw[x][y] / TERRAIN_HEIGHT_DIVISION;
					//nw
					vces[v + 2u].pos.x = (lf32)x - 0.5f; vces[v + 2u].pos.z = (lf32)y + 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_sw[x][y + 1] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 3; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.x * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;
						vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x - 1][y]] = 1.f; //nw
					vces[v + 2].txtr[MATMAP[x - 1][y]] = 1.f; //nw

					ices[i + 0u] = v + 0u;
					ices[i + 1u] = v + 2u;
					ices[i + 2u] = v + 1u;

					i += 3;
					v += 3;
				}
				///*
				//-------------------------------- Generate east face
				// If both corners require new faces
				if ((tgendata[datax][datay] & eTGEN_FACE_E_COR_N) != 0 && (tgendata[datax][datay] & eTGEN_FACE_E_COR_S) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 1u].pos.x = (lf32)x + 0.5f; vces[v + 1u].pos.z = (lf32)y - 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_se[x][y] / TERRAIN_HEIGHT_DIVISION;
					//ne
					vces[v + 2u].pos.x = (lf32)x + 0.5f; vces[v + 2u].pos.z = (lf32)y + 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_nw[x + 1][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 3u].pos.x = (lf32)x + 0.5f; vces[v + 3u].pos.z = (lf32)y - 0.5f;
					vces[v + 3u].pos.y = (lf32)hmap_sw[x + 1][y] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 4; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.z * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;
						//vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;

						if (vces[v + 0u].pos.y < vces[v + 2u].pos.y) {
							vces[v + i].nor.y = 0.f; vces[v + i].nor.x = -1.f; vces[v + i].nor.z = 0.f;
						}
						else {
							vces[v + i].nor.y = 0.f; vces[v + i].nor.x = 1.f; vces[v + i].nor.z = 0.f;
						}

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x][y - 1]] = 1.f; //se
					vces[v + 2].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 3].txtr[MATMAP[x][y - 1]] = 1.f; //se

					ices[i + 0u] = v + 2u;
					ices[i + 1u] = v + 0u;
					ices[i + 2u] = v + 3u;
					ices[i + 3u] = v + 3u;
					ices[i + 4u] = v + 0u;
					ices[i + 5u] = v + 1u;

					i += 6;
					v += 4;
				}
				// If only one corner requires a face
				else if ((tgendata[datax][datay] & eTGEN_FACE_E_COR_N) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 1u].pos.x = (lf32)x + 0.5f; vces[v + 1u].pos.z = (lf32)y - 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_se[x][y] / TERRAIN_HEIGHT_DIVISION;
					//ne
					vces[v + 2u].pos.x = (lf32)x + 0.5f; vces[v + 2u].pos.z = (lf32)y + 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_nw[x + 1][y] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 3; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.z * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;
						vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x][y - 1]] = 1.f; //se
					vces[v + 2].txtr[MATMAP[x][y]] = 1.f; //ne

					ices[i + 0u] = v + 0u;
					ices[i + 1u] = v + 1u;
					ices[i + 2u] = v + 2u;

					i += 3;
					v += 3;
				}
				else if ((tgendata[datax][datay] & eTGEN_FACE_E_COR_S) != 0) {
					//ne
					vces[v + 0u].pos.x = (lf32)x + 0.5f; vces[v + 0u].pos.z = (lf32)y + 0.5f;
					vces[v + 0u].pos.y = (lf32)hmap_ne[x][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 1u].pos.x = (lf32)x + 0.5f; vces[v + 1u].pos.z = (lf32)y - 0.5f;
					vces[v + 1u].pos.y = (lf32)hmap_se[x][y] / TERRAIN_HEIGHT_DIVISION;
					//se
					vces[v + 2u].pos.x = (lf32)x + 0.5f; vces[v + 2u].pos.z = (lf32)y - 0.5f;
					vces[v + 2u].pos.y = (lf32)hmap_sw[x + 1][y] / TERRAIN_HEIGHT_DIVISION;

					for (int i = 0; i < 3; ++i) {
						vces[v + i].uvc.x = vces[v + i].pos.z * TERRAIN_UV_SCALE;
						vces[v + i].uvc.y = vces[v + i].pos.y * TERRAIN_UV_SCALE;
						vces[v + i].nor.y = 1.f; vces[v + i].nor.x = 0.f; vces[v + i].nor.z = 0.f;

						vces[v + i].txtr[0] = 0.f; vces[v + i].txtr[1] = 0.f;
						vces[v + i].txtr[2] = 0.f; vces[v + i].txtr[3] = 0.f;
						vces[v + i].txtr[4] = 0.f; vces[v + i].txtr[5] = 0.f;
						vces[v + i].txtr[6] = 0.f; vces[v + i].txtr[7] = 0.f;
					}
					vces[v].txtr[MATMAP[x][y]] = 1.f; //ne
					vces[v + 1].txtr[MATMAP[x][y - 1]] = 1.f; //se
					vces[v + 2].txtr[MATMAP[x][y]] = 1.f; //ne

					ices[i + 0u] = v + 0u;
					ices[i + 1u] = v + 1u;
					ices[i + 2u] = v + 2u;

					i += 3;
					v += 3;
				}
				//*/
			}
		}

		//-------------------------------- INITIALIZE OPENGL BUFFER

		// Initial check prevents regeneration memory leak
		if (vao == UI32_NULL) glGenVertexArrays(1, &vao); // Create vertex buffer
		if (vbo == UI32_NULL) glGenBuffers(1, &vbo); // Generate vertex buffer
		if (ebo == UI32_NULL) glGenBuffers(1, &ebo); // Generate element buffer

		ReBindGL();

		// Free vertex and index buffers!
		free((void*)vces);
		free((void*)ices);
	}

	void TerrainMesh::ReBindGL() {
		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexTerrain), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(lui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VT_I_POS); // Set Vertex positions
		glVertexAttribPointer(VT_I_POS, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (void*)VT_O_POS);
		glEnableVertexAttribArray(VT_I_NOR); // Set Vertex normals
		glVertexAttribPointer(VT_I_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (void*)VT_O_NOR);
		glEnableVertexAttribArray(VT_I_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VT_I_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (void*)VT_O_UVC);
		// Set the texture blend value array
		for (lui32 i = 0u; i < 8u; ++i) {
			glEnableVertexAttribArray(VT_I_TXTR + i); // Set Vertex colour
			glVertexAttribPointer(VT_I_TXTR + i, 1u, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (void*)(VT_O_TXTR + (sizeof(lf32) * i)));
		}

		//glEnableVertexAttribArray(VT_I_TXTR); // Set Vertex colour
		//glVertexAttribPointer(VT_I_TXTR, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (void*)VT_O_TXTR);

		glBindVertexArray(0); // Bind default vertex array
	}

	//________________________________________________________________________________________________________________________________
	// GUI BITMAP --------------------------------------------------------------------------------------------------------------------

	GUIBitmap::GUIBitmap() {
	}
	void GUIBitmap::Init() {
		// set up vertex data (and buffer(s)) and configure vertex attributes
		//float vertices[] = {
		//	// positions          // colors           // texture coords
		//	1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		//	1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		//	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		//	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		//};
		float vertices[] = { //invert Y
							 // positions          // colors           // texture coords
			1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
			1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // bottom right
			-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
			-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left 
		};
		//ccw
		unsigned int indices[] = {
			0, 3, 1, // first triangle
			1, 3, 2  // second triangle
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	void GUIBitmap::SetTexture(GLuint tex) {
		tid = tex;
	}
	void GUIBitmap::Draw(int posx, int posy, int w, int h, lf32 opacity) {
		// create transformations
		glm::mat4 transform = glm::mat4(1.0f);
		//scale to screen space

		transform = glm::scale(transform, glm::vec3(2.f / gPtr->GUIframeSizeX, 2.f / gPtr->GUIframeSizeY, 1.0f));
		transform = glm::translate(transform, glm::vec3(posx, posy, 0));
		transform = glm::scale(transform, glm::vec3(w / 2, h / 2, 1.0f));

		Shader* s = &gPtr->shaders[S_GUI];

		s->Use();
		s->SetFloat(Shader::Opacity, opacity);
		s->setMat4(Shader::GUITransform, *(graphics::Matrix4x4*)&transform);
		s->setMat4(Shader::GUIOffset, *(graphics::Matrix4x4*)&glm::mat4(1.f));

		//s->setMat4(Shader::GUITransform, *(graphics::Matrix4x4*)&transform);

		//....................................... DRAW

		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, tid);
		s->Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}












	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// (copied from ltr, yet to be optimized, integrated or fixed

	Vertex createvert(float x, float y, float z, float r, float g, float b, float u, float v) {
		Vertex v2;

		v2.pos.x = x;
		v2.pos.y = y;
		v2.pos.z = z;

		v2.col.x = r;
		v2.col.y = g;
		v2.col.z = b;

		v2.uvc.x = u;
		v2.uvc.y = v;

		return v2;
	}
	struct quad
	{
		float x1, x2, y1, y2, u1, u2, v1, v2;
		quad(float _x1, float _x2, float _y1, float _y2, float _u1, float _u2, float _v1, float _v2) {
			x1 = _x1; x2 = _x2;
			y1 = _y1; y2 = _y2;
			u1 = _u1; u2 = _u2;
			v1 = _v1; v2 = _v2;
		}
	};
	void AddQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, quad p, int index) {
		// Create vertices
		vertices.push_back(createvert(p.x1, p.y2, 0.f, 1.f, 1.f, 1.f, p.u1, p.v1)); // Top left
		vertices.push_back(createvert(p.x2, p.y2, 0.f, 1.f, 1.f, 1.f, p.u2, p.v1)); // Top right
		vertices.push_back(createvert(p.x1, p.y1, 0.f, 1.f, 1.f, 1.f, p.u1, p.v2)); // Bottom left
		vertices.push_back(createvert(p.x2, p.y1, 0.f, 1.f, 1.f, 1.f, p.u2, p.v2)); // Bottom right
		// Create indices
		indices.push_back(0 + index); // Top Left Tri
		indices.push_back(2 + index); // -
		indices.push_back(1 + index); // -
		indices.push_back(3 + index); // Bottom Right Tri
		indices.push_back(1 + index); // -
		indices.push_back(2 + index); // -
	}

	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************

	void GUIBox::Init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIBox::End() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		initialized = false;
	}
	void GUIBox::ReGen(li16 _xA, li16 _xB, li16 _yA, li16 _yB, lui16 mg, lui16 bl) {
		if (!initialized) Init();

		// create transformations
		transform = glm::mat4(1.0f);

		//manual vector version
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		li16 xA = _xA - bl;
		li16 xB = _xB + bl;
		li16 yA = _yA - bl;
		li16 yB = _yB + bl;

		AddQuad(vertices, indices, quad(xA, xA + mg, yA, yA + mg, 0.f, 0.5f, 0.5f, 1.f), 0); // Down Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yA, yA + mg, 0.5f, 0.5f, 0.5f, 1.f), 4); // Down Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yA, yA + mg, 0.5f, 1.f, 0.5f, 1.f), 8); // Down Right
		AddQuad(vertices, indices, quad(xA, xA + mg, yA + mg, yB - mg, 0.f, 0.5f, 0.5f, 0.5f), 12); // Mid Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yA + mg, yB - mg, 0.5f, 0.5f, 0.5f, 0.5f), 16); // Mid Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yA + mg, yB - mg, 0.5f, 1.f, 0.5f, 0.5f), 20); // Mid Right
		AddQuad(vertices, indices, quad(xA, xA + mg, yB - mg, yB, 0.f, 0.5f, 0.f, 0.5f), 24); // Up Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yB - mg, yB, 0.5f, 0.5f, 0.f, 0.5f), 28); // Up Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yB - mg, yB, 0.5f, 1.f, 0.f, 0.5f), 32); // Up Right

		BindBuffers(vertices, indices, VAO, VBO, EBO);

		// create transformations
		transform = glm::mat4(1.0f);
		//scale to screen space
		transform = glm::scale(transform, glm::vec3(2.f / gPtr->GUIframeSizeX, 2.f / gPtr->GUIframeSizeY, 1.0f));
	}
	void GUIBox::Draw(Texture* t, lf32 opacity) {
		Shader* s = &gPtr->shaders[S_GUI];

		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, t->glID);
		s->Use();
		s->SetFloat(Shader::Opacity, opacity);
		s->setMat4(Shader::GUITransform, *(graphics::Matrix4x4*)&transform);
		s->setMat4(Shader::GUIOffset, *(graphics::Matrix4x4*)&glm::mat4(1));

		// Draw

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6 * 9, GL_UNSIGNED_INT, 0);
	}

	void GUIText::Init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIText::End() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		initialized = false;
	}
	void GUIText::SetOffset(int X, int Y) {
		// create transformations
		offset = glm::mat4(1.0f);
		//add offset
		offset = glm::translate(offset, glm::vec3(X, Y, 0));
	}

	void GUIText::GetTextBounds() {

	}

	//backup
	//void GUIText::ReGen(char* c, unsigned int stringlength, li16 _xa, li16 _xb, li16 _y)

	void GUIText::ReGen(char* _c, li16 _xa, li16 _xb, li16 _y, TextAlignMode align) {
		// this is necessary atm because we modify the string :(
		char* c = new char[strlen(_c) + 1];
		memcpy(c, _c, strlen(_c) + 1);

		if (!initialized) Init();
		int ch = 12;
		int letterspace = 1;
		
		// create transformations
		transform = glm::mat4(1.0f);
		offset = glm::mat4(1.0f);
		// Set initial vars
		cnum = 0;
		xa = _xa; xb = _xb; y = _y;
		sizex = 0u;
		sizey = ch;
		
		//manual vector version
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		//character height
		int x = xa; int y2 = y - ch - 2;
		lui32 stringlength = (lui32)strlen(c);
		int lastLineWidth = 0;
		int width = 0;
		int lines = 1;

		// print text
		if (stringlength > 0) {
			lines++;
			int last_space = 0;
			//insert linebreaks
			for (int i = 0; i <= stringlength; i++) {
				// Convert ascii linebreaks
				if (c[i] == CHARCODE_ASCII::CR || c[i] == CHARCODE_ASCII::LF) {
					c[i] = CHARCODE::LB;
					x = xa;
					y2 -= ch;
					lines++;
					sizey += ch;
					lastLineWidth = 0;
				}
				else {
					if (c[i] == CHARCODE_ASCII::space) {
						last_space = i;
					}

					// line break if we've gone past the edge
					if (x + font_kerning[c[i]] + 2 > xb && last_space != 0) {
						c[last_space] = CHARCODE::LB;
						x = xa;
						y2 -= ch;
						lines++;
						sizey += ch;
						lastLineWidth = 0;
					}

					cnum++;
					x += font_kerning[c[i]] + letterspace;
					width += font_kerning[c[i]] + letterspace;
					lastLineWidth += font_kerning[c[i]] + letterspace;
					if (lastLineWidth > sizex)
						//sizex += font_kerning[c[i]] + letterspace;
						sizex = lastLineWidth;
				}
			}

			x = xa; y2 = y - ch - 2;

			// why even reset this?
			cnum = 0;
			width = 0;
			lastLineWidth = 0;

			// calc offsets before resetting sizes
			int offsetx = 0;
			//int offsety;
			if (align == eTEXTALIGN_MID) offsetx = -(sizex / 2);
			
			// why even reset this?
			sizex = 0u;
			sizey = 0u;
			sizey += ch;

			// print
			for (int i = 0; i <= stringlength; i++) {
				if (c[i] == CHARCODE::LB) {
					x = xa;
					y2 -= ch;
					lines++;
					sizey += ch;
					lastLineWidth = 0;
				}
				else {
					// Set UV coords
					if (c[i] != CHARCODE::space) {
						int u = c[i] % 16;
						int v = floor(c[i] / 16);
						AddQuad(vertices, indices, quad(x + offsetx, x + 16 + offsetx, y2, y2 + 16,
							(float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f),
							cnum * 4);
						cnum++;
					}
					x += font_kerning[c[i]] + letterspace;
					width += font_kerning[c[i]] + letterspace;
					lastLineWidth += font_kerning[c[i]] + letterspace;
					if (lastLineWidth > sizex)
						sizex += font_kerning[c[i]] + letterspace;
				}
			}
		}
		else
		{
			//set UV coords
			int u = 0xa1 % 16;
			int v = floor(0xa1 / 16);
			AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16,
				(float)(u) / 16.f, (float)(u + 1) / 16.f,
				(float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
		}

		delete[] c;

		BindBuffers(vertices, indices, VAO, VBO, EBO);

		// create transformations
		transform = glm::mat4(1.0f);

		//scale to screen space
		transform = glm::scale(transform, glm::vec3(2.f / gPtr->GUIframeSizeX, 2.f / gPtr->GUIframeSizeY, 1.0f));
	}
	//the 'scroll' boolean feels kind of clunky but it will work for now
	void GUIText::Draw(Texture* t, lf32 opacity) {
		Shader* s = &gPtr->shaders[S_GUI];

		s->Use();
		s->SetFloat(Shader::Opacity, opacity);
		s->setMat4(Shader::GUITransform, *(graphics::Matrix4x4*)&transform);
		s->setMat4(Shader::GUIOffset, *(graphics::Matrix4x4*)&offset);

		// Draw

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, cnum * 6, GL_UNSIGNED_INT, 0);
	}

	void DrawGUITexture(Texture* texture, li32 x, li32 y, li32 w, li32 h, lf32 opacity) {
		guibmp.SetTexture(texture->glID);
		guibmp.Draw(x, y, w, h, opacity);
	}
	void DrawGUIBox(Texture* texture, li16 xa, li16 xb, li16 ya, li16 yb, lui16 margin_size, lui16 bleed_size) {
		guibox.ReGen(xa, xb, ya, yb, margin_size, bleed_size);
		guibox.Draw(texture);
	}
	void DrawGUIText(char* string, Texture* texture, li32 xa, li32 xb, li32 y) {
		guitxt.ReGen(string, xa, xb, y);
		guitxt.Draw(texture);
	}
}

__forceinline void DrawMeshGL(lui32 mdl, lui32 ices_size, lui32 tex) {
	glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
	glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
	glBindVertexArray(mdl); // Bind vertex array
	glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0); // Draw call
	glBindVertexArray(0); // Return to default
}

void DrawMesh(graphics::Mesh& mdl, graphics::Texture tex,
	ShaderStyle charashader, graphics::Matrix4x4 matrix) {
	#if DEF_SWR
	RenderDrawMesh(&mdl, &tex, matrix);
	#else
	graphics::Shader* shd = nullptr;
	switch (charashader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_CHARA);
		break;
	};

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// Render the mesh
	DrawMeshGL(mdl.vao, mdl.IcesSize(), tex.glID);
	#endif
}

void DrawParticles(graphics::Mesh& mdl, graphics::Texture tex,
	graphics::Matrix4x4* matrix, lui32 count) {
	graphics::Shader* shd = &graphics::GetShader(graphics::S_SOLID);

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// Render the mesh
	DrawMeshGL(mdl.vao, mdl.IcesSize(), tex.glID);
}

void DrawBlendMesh(graphics::MeshBlend& mdl, lf32 bs, graphics::Texture tex, ShaderStyle charashader, graphics::Matrix4x4 matrix) {
	graphics::Shader* shd = nullptr;
	switch (charashader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID_BLEND);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_BLEND_CHARA);
		break;
	};

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, matrix);
	// Set blend state
	shd->SetFloat(shd->fBlendState, bs);

	// Render the mesh
	DrawMeshGL(mdl.vao, mdl.IcesSize(), tex.glID);
}

void DrawMeshSet(graphics::MeshSet& meshset, lf32 anim_value,
	graphics::Texture texture, ShaderStyle shader, graphics::Matrix4x4 matrix) {
	graphics::Shader* shd = nullptr;
	#if DEF_MESHSET_BLEND
	switch (shader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID_BLEND);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_BLEND_CHARA);
		break;
	};
	#else
	switch (shader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_CHARA);
		break;
	};
	#endif

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	#if DEF_MESHSET_BLEND
	// Render the mesh
	// normalize anim value
	anim_value = fmodf(anim_value, (lf32)meshset.count); // Loop animation if we must
	//lf32 av2 = anim_value * (lf32)meshset.count;
	li32 avframe = floorf(anim_value); // calculate which mesh frame
	lf32 avmod = fmodf(anim_value, 1.f); // calculate blend state
	// Set blend state
	shd->SetFloat(shd->fBlendState, avmod);
	DrawMeshGL(meshset.vao[avframe], meshset.ices_size, texture.glID);
	#else
	DrawMeshGL(meshset.vao[(li32)floorf(anim_value) % meshset.count], meshset.ices_size, texture.glID);
	#endif
}

void DrawMeshDeform(graphics::MeshDeform& mdl, graphics::Texture tex,
	ShaderStyle charashader, lui32 matrix_count,
	graphics::Matrix4x4 transform_a = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_b = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_c = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_d = graphics::Matrix4x4()) {
	// Get the shader reference
	graphics::Shader* shd = nullptr;// = &graphics::GetShader(graphics::S_SOLID_DEFORM);
	switch (charashader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID_DEFORM);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA);
		break;
	};

	// Enable the shader
	shd->Use();

	// Set shader properties
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->SetUint(shd->uiMatrixCount, matrix_count);
	if (matrix_count >= 1u) shd->setMat4(shd->matModelA, transform_a);
	if (matrix_count >= 2u) shd->setMat4(shd->matModelB, transform_b);
	if (matrix_count >= 3u) shd->setMat4(shd->matModelC, transform_c);
	if (matrix_count == 4u) shd->setMat4(shd->matModelD, transform_d);

	// Render the mesh
	DrawMeshGL(mdl.vao, mdl.IcesSize(), tex.glID);
}

void DrawCompositeMesh(graphics::CompositeMesh& mdl, graphics::Texture tex,
	ShaderStyle charashader, graphics::Matrix4x4 matrix) {
	graphics::Shader* shd = nullptr;
	switch (charashader) {
	case SS_NORMAL:
		shd = &graphics::GetShader(graphics::S_SOLID);
		break;
	case SS_CHARA:
		shd = &graphics::GetShader(graphics::S_SOLID_CHARA);
		break;
	};

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// Render the mesh
	mdl.Draw(tex.glID, shd->ID);
}

void DrawTerrainMesh(graphics::TerrainMesh mdl,
	graphics::Texture tex_a, graphics::Texture tex_b,
	graphics::Texture tex_c, graphics::Texture tex_d,
	graphics::Texture tex_e, graphics::Texture tex_f,
	graphics::Texture tex_g, graphics::Texture tex_h,
	graphics::Matrix4x4 matrix) {
	graphics::Shader* shd = &graphics::GetShader(graphics::S_SOLID_TERRAIN);
	//graphics::Shader* shd = &graphics::GetShader(graphics::S_SOLID);

	// Enable the shader
	shd->Use();

	// Set matrices on shader
	glm::mat4 gmatp = graphics::GetMatProj();
	glm::mat4 gmatv = graphics::GetMatView();
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&gmatp);
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&gmatv);
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// terrain textures
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain1, tex_a.glID, graphics::Shader::TXTR_TERRAIN1);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain2, tex_b.glID, graphics::Shader::TXTR_TERRAIN2);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain3, tex_c.glID, graphics::Shader::TXTR_TERRAIN3);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain4, tex_d.glID, graphics::Shader::TXTR_TERRAIN4);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain5, tex_e.glID, graphics::Shader::TXTR_TERRAIN5);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain6, tex_f.glID, graphics::Shader::TXTR_TERRAIN6);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain7, tex_g.glID, graphics::Shader::TXTR_TERRAIN7);
	graphics::GetShader(graphics::S_SOLID_TERRAIN).SetTexture(graphics::Shader::texTerrain8, tex_h.glID, graphics::Shader::TXTR_TERRAIN8);

	// Render the mesh
	mdl.Draw();
}
