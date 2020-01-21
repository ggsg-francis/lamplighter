//TOP INCLUDE
#include "graphics.hpp"

//would like to drop this include
#include <glm\gtc\matrix_transform.hpp>
//i dont know what the point of using this one is
#include <glm\gtc\type_ptr.hpp>

//#include "global.h"
#include "cfg.h"

#include "maths.hpp"
#include "Transform.h"

extern TGA_ORDER *TGA_READER_ARGB;
extern TGA_ORDER *TGA_READER_ABGR;

#define FILE_VERSION_MB 0x0u
#define FILE_VERSION_M 0x0u
#define FILE_VERSION_TEX 0x0u

typedef btui16 version_t;

namespace graphics
{
	// TODO: MOVE TO MATHS!!!
	m::Vector3 operator*(const m::Vector3& vector, const Matrix4x4& matrix)
	{
		return m::Vector3(
			(vector.x * matrix[0][0] + vector.y * matrix[0][1] + vector.z * matrix[0][2] + matrix[0][3]), // X
			(vector.x * matrix[1][0] + vector.y * matrix[1][1] + vector.z * matrix[1][2] + matrix[1][3]), // Y
			(vector.x * matrix[2][0] + vector.y * matrix[2][1] + vector.z * matrix[2][2] + matrix[2][3])  // Z
		);
	}

	/*btui8 font_kerning[] =
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

	btui8 font_kerning[] =
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
	};

	/*btui8 font_kerning[] =
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
		btui32 frameSizeX = 320u;
		btui32 frameSizeY = 240u;
		Shader shaders[S_COUNT];
	};
	Graphics* gPtr;

	btui32 FrameSizeX() { return gPtr->frameSizeX; }
	btui32 FrameSizeY() { return gPtr->frameSizeY; }

	void SetFrameSize(btui32 x, btui32 y)
	{
		if (!cfg::bEditMode)
		{
			btui32 scale = x / SCREEN_UPSCALE_THRESHOLD + 1u;
			btui32 scale_half = scale * 2u;
			gPtr->frameSizeX = cfg::iWinX / scale_half;
			gPtr->frameSizeY = cfg::iWinY / scale;
		}
		else
		{
			gPtr->frameSizeX = cfg::iWinX;
			gPtr->frameSizeY = cfg::iWinY;
		}

		gPtr->shaders[S_POST].Use();
		gPtr->shaders[S_POST].SetFloat(gPtr->shaders[S_POST].fWindowX, (GLfloat)x * 0.5f);
		gPtr->shaders[S_POST].SetFloat(gPtr->shaders[S_POST].fWindowY, (GLfloat)y);
	}

	glm::mat4 mat_proj;
	glm::mat4 mat_view;

	// TEMP TEMP TEMP TEMP
	GUIBitmap guibmp;

	//________________________________________________________________________________________________________________________________
	//-------------------------------- INITIALIZATION

	void Init()
	{
		gPtr = new Graphics();

		guibmp.Init();

		gPtr->shaders[S_SOLID].Init("shaders/vert_3d.glsl", "shaders/frag_solid.glsl");
		gPtr->shaders[S_SOLID_CHARA].Init("shaders/vert_3d.glsl", "shaders/frag_solid_chara.glsl");

		gPtr->shaders[S_SOLID_BLEND].Init("shaders/vert_3d_blend.glsl", "shaders/frag_solid.glsl");
		gPtr->shaders[S_SOLID_BLEND_CHARA].Init("shaders/vert_3d_blend.glsl", "shaders/frag_solid_chara.glsl");

		gPtr->shaders[S_SOLID_DEFORM].Init("shaders/vert_3d_deform.glsl", "shaders/frag_solid_chara.glsl");
		
		gPtr->shaders[S_MEAT].Init("shaders/vert_3d.glsl", "shaders/frag_meat.glsl");

		gPtr->shaders[S_GUI].Init("shaders/gui_vert.glsl", "shaders/gui_frag.glsl");
		gPtr->shaders[S_POST].Init("shaders/fb_vert.glsl", "shaders/fb_frag.glsl");

		SetFrameSize(cfg::iWinX, cfg::iWinY);
	}

	void End()
	{
		delete gPtr;
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- MATRIX

	FRow4 operator*(const FRow4& row, const btf32 mult)
	{
		return FRow4(row[0] * mult, row[1] * mult, row[2] * mult, row[3] * mult);
	};
	FRow4 operator+(const FRow4& row_a, const FRow4& row_b)
	{
		return FRow4(row_a[0] + row_b[0], row_a[1] + row_b[1], row_a[2] + row_b[2], row_a[3] + row_b[3]);
	};

	Matrix4x4 MatrixTranslate(Matrix4x4 const& m, m::Vector3 const& v)
	{
		Matrix4x4 result = m;
		result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
		return result;
		//matrix[3] = matrix[0] * pos.x + matrix[1] * pos.y + matrix[2] * pos.z + matrix[3];
	}
	Matrix4x4 MatrixRotate(Matrix4x4 const& m, btf32 angle, m::Vector3 const& v)
	{
		btf32 const c = cos(angle);
		btf32 const s = sin(angle);

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
	Matrix4x4 MatrixScale(Matrix4x4 const& m, m::Vector3 const& v)
	{
		Matrix4x4 Result;
		Result[0] = m[0] * v.x;
		Result[1] = m[1] * v.y;
		Result[2] = m[2] * v.z;
		Result[3] = m[3];
		return Result;
	}
	Matrix4x4 MatrixLookAt(Matrix4x4 const& matrix, m::Vector3 const& src, m::Vector3 const& targ, m::Vector3 const& up)
	{
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
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);
	}
	void MatrixTransformXFlip(Matrix4x4& matrix, m::Vector3 const& pos)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, btf32 yaw)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		btf32 c = cos(yaw); btf32 s = sin(yaw);
		Matrix4x4 m = matrix;
		matrix[0] = m[0] * c + m[2] * -s;
		matrix[1] = m[1] * (c + (1 - c));
		matrix[2] = m[0] * s + m[2] * c;
	}
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, btf32 yaw, btf32 pitch)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix (yaw)
		btf32 c = cos(yaw); btf32 s = sin(yaw);
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
	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0))
	{
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
	}
	void MatrixTransformForwardUp(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0))
	{
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
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vForw.x; matrix[1][1] = vForw.y; matrix[1][2] = -vForw.z;
		matrix[2][0] = -vLoUp.x; matrix[2][1] = -vLoUp.y; matrix[2][2] = vLoUp.z;

	}
	void MatrixTransformXFlip(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0))
	{
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

	m::Vector3 view;
	m::Vector3 GetViewPos()
	{
		return view;
	}
	void SetMatProj(btf32 fovMult)
	{
		cfg::bEditMode ?
			mat_proj = glm::perspective(glm::radians(cfg::fCameraFOV), (float)FrameSizeX() / (float)FrameSizeY(), cfg::fCameraNearClip * fovMult, cfg::fCameraFarClip * fovMult) :
			mat_proj = glm::perspective(glm::radians(cfg::fCameraFOV), (float)FrameSizeX() / (float)FrameSizeY(), cfg::fCameraNearClip * fovMult, cfg::fCameraFarClip * fovMult);
	}
	void SetMatView(void* t)
	{
		// this is not.... good.....
		#define T ((Transform3D*)t)
		view = m::Vector3((T->pos_glm + m::RotateVector(m::Vector3(0.f, 0.18f, 0.2f), T->GetRotation())) * glm::vec3(1.f, 1.f, -1.f));
		mat_view = glm::lookAt((glm::vec3)view, (glm::vec3)view + (T->GetForward()) * glm::vec3(1.f, 1.f, -1.f), (glm::vec3)T->GetUp() * glm::vec3(1.f, 1.f, -1.f));

		//view = m::Vector3((T->pos_glm * glm::vec3(1.f, 1.f, -1.f)));
		//mat_view = glm::lookAt((glm::vec3)view + (T->GetForward() * -15.f * glm::vec3(1.f, 1.f, -1.f)), (glm::vec3)view, glm::vec3(0.f, 1.f, 0.f));
		#undef T
	}
	// Lightsource
	void SetMatProjLight()
	{
		mat_proj = glm::ortho(-LIGHT_WIDTH, LIGHT_WIDTH, -LIGHT_WIDTH, LIGHT_WIDTH, 0.f, LIGHT_FAR);
	}
	void SetMatViewLight(float x, float y, float z, float vx, float vy, float vz)
	{
		mat_view = glm::lookAt(glm::vec3(x - (vx * LIGHT_HALF), y - (vy * LIGHT_HALF), z - (vz * LIGHT_HALF)),
			glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void SetMatViewEditor(void* t)
	{
		// this is not.... good.....
		#define T ((Transform3D*)t)
		mat_view = glm::lookAt((T->pos_glm + (T->GetForward() * -2.f)) * glm::vec3(1.f, 1.f, -1.f),
			T->pos_glm * glm::vec3(1.f, 1.f, -1.f),
			(glm::vec3)T->GetUp() * glm::vec3(1.f, 1.f, -1.f));
		#undef T
	}

	glm::mat4 GetMatProj()
	{
		return mat_proj;
	}
	glm::mat4 GetMatView()
	{
		return mat_view;
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- UTILITY FUNCTIONS

	void SetFrontFace()
	{
		glFrontFace(GL_CW);
	}
	void SetFrontFaceInverse()
	{
		glFrontFace(GL_CCW);
	}

	// good, but replace the std::vectors
	void BindBuffers(std::vector<Vertex> &vertices, std::vector<btui32> &indices, GLuint VAO, GLuint VBO, GLuint EBO)
	{
		glBindVertexArray(VAO); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(btui32), &indices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VI_POS); // Set Vertex positions
		glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
		glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
		glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
		glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
		glEnableVertexAttribArray(VI_COL); // Set Vertex colour
		glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);
	}

	void DrawGUITexture(Texture* texture, bti32 x, bti32 y, bti32 w, bti32 h)
	{
		Shader* shader = &gPtr->shaders[S_GUI];

		// TEEEEMMP TEMP TEMP TEMP
		guibmp.SetShader(shader);
		guibmp.SetTexture(texture->glID);
		guibmp.Draw(x, y, w, h);
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- SHADER

	Shader::Shader()
	{
	}
	void Shader::Init(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// if geometry shader path is present, also load a geometry shader
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		//int success;
		//char infoLog[512];
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
		if (geometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
		///*
		std::cout << "SHADER VERT: " << vertexPath << " FRAG: " << fragmentPath << std::endl;
		for (btui32 i = 0u; i < LOCATION_COUNT; ++i)
		{
		location[i] = glGetUniformLocation(ID, names[i]);
		if (location[i] != -1)
			std::cout << "Got shader property ID: " << location[i] << " STR: " << names[i] << std::endl;
		}//*/
		/*
		location[matModel] = glGetUniformLocation(ID, "matm");
		location[uiMatrixCount] = glGetUniformLocation(ID, "mc");
		location[matModelA] = glGetUniformLocation(ID, "matma");
		location[matModelB] = glGetUniformLocation(ID, "matmb");
		location[matModelC] = glGetUniformLocation(ID, "matmc");
		location[matModelD] = glGetUniformLocation(ID, "matmd");
		location[matView] = glGetUniformLocation(ID, "matv");
		location[matProject] = glGetUniformLocation(ID, "matp");
		location[fBlendState] = glGetUniformLocation(ID, "blendState");
		location[fWindowX] = glGetUniformLocation(ID, "wx");
		location[fWindowY] = glGetUniformLocation(ID, "wy");
		location[fTime] = glGetUniformLocation(ID, "ft");
		location[matLightProj] = glGetUniformLocation(ID, "lightProj");
		location[vecPCam] = glGetUniformLocation(ID, "pcam");
		location[vecVSun] = glGetUniformLocation(ID, "vsun");
		location[matTransform] = glGetUniformLocation(ID, "transform");
		location[fLit_TEMP] = glGetUniformLocation(ID, "lit");
		location[texShadowMap] = glGetUniformLocation(ID, "tshadow");
		//*/
	}
	void Shader::Use()
	{
		glUseProgram(ID);
	}
	void Shader::SetBool(const PIndex index, bool value) const
	{
		glUniform1i(location[index], (int)value);
	}
	void Shader::SetInt(const PIndex index, int value) const
	{
		glUniform1i(location[index], value);
	}
	void Shader::SetUint(const PIndex index, unsigned int value) const
	{
		glUniform1ui(location[index], value);
	}
	void Shader::SetFloat(const PIndex index, float value) const
	{
		glUniform1f(location[index], value);
	}
	void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void Shader::setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void Shader::setVec3(const PIndex index, const glm::vec3 &value) const
	{
		glUniform3fv(location[index], 1, &value[0]);
	}
	void Shader::setVec3(const PIndex index, float x, float y, float z) const
	{
		glUniform3f(location[index], x, y, z);
	}
	void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::setMat4(const PIndex index, const Matrix4x4& mat) const
	{
		glUniformMatrix4fv(location[index], 1, GL_FALSE, &mat[0][0]);
	}
	void Shader::SetTexture(const PIndex index, GLuint texture, TIndex textureIndex)
	{
		glActiveTexture(temp[textureIndex]); // active proper texture unit before binding
		//glActiveTexture(GL_TEXTURE5); // active proper texture unit before binding
		glUniform1i(location[index], textureIndex);
		glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture
	}
	void Shader::CheckCompileErrors(GLuint shader, const std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	Shader& GetShader(eShader id)
	{
		return gPtr->shaders[id];
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- TEXTURE

	ModifiableTexture::~ModifiableTexture()
	{
		delete[] buffer;
		//glDeleteTextures(1, &glID);
	}
	void ModifiableTexture::Init(btui16 sx, btui16 sy, colour col)
	{
		glGenTextures(1, &glID); // Initialize GL texture

		width = sx;
		height = sy;
		// Read pixel buffer
		buffer = new colour[width * height];
		for (btui32 i = 0ui32; i < (btui32)(width * height); ++i)
		{
			buffer[i] = col;
		}
	}
	void ModifiableTexture::LoadFile(char * fn)
	{
		glGenTextures(1, &glID); // Initialize GL texture
		FILE *file = fopen(fn, "rb"); // Open the file
		if (file)
		{
			// Seek the beginning of the file
			fseek(file, 0, SEEK_SET);
			// Read version
			version_t v;
			fread(&v, sizeof(version_t), 1, file);
			TextureFilterMode fm;
			TextureEdgeMode em;
			fread(&fm, 1, 1, file);
			fread(&em, 1, 1, file);
			// Read dimensions
			fread(&width, sizeof(btui16), 1, file);
			fread(&height, sizeof(btui16), 1, file);
			// Read pixel buffer
			buffer = new colour[width * height];
			//if (sizeof(graphics::colour) * width * height > sizeof(buffer)) return;
			fread(buffer, sizeof(graphics::colour), width * height, file);

			fclose(file);
		}
	}
	void ModifiableTexture::SetPixel(btui16 x, btui16 y, colour c)
	{
		buffer[x * width + y] = c;
	}
	void ModifiableTexture::SetPixelChannelR(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].r = val;
	}
	void ModifiableTexture::SetPixelChannelG(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].g = val;
	}
	void ModifiableTexture::SetPixelChannelB(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].b = val;
	}
	void ModifiableTexture::SetPixelChannelA(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].a = val;
	}
	colour ModifiableTexture::GetPixel(btui16 x, btui16 y)
	{
		return buffer[x * width + y];
	}
	void FMNearest() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	void FMNearestMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	void FMLinear() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	void FMLinearMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	void(*SetFilterMode[])() = { FMNearest, FMNearestMip, FMLinear, FMLinearMip };
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
	void ModifiableTexture::ReBindGL(TextureFilterMode filter_mode, TextureEdgeMode edge_mode)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		SetFilterMode[filter_mode]();
		SetEdgeMode[edge_mode]();
	}

	void Texture::LoadFile(char * fn)
	{
		glGenTextures(1, &glID);

		FILE *file = fopen(fn, "rb"); // Open the file
		if (file)
		{
			// Seek the beginning of the file
			fseek(file, 0, SEEK_SET);
			// Read version
			version_t v;
			fread(&v, sizeof(version_t), 1, file);
			TextureFilterMode fm;
			TextureEdgeMode em;
			fread(&fm, 1, 1, file);
			fread(&em, 1, 1, file);
			// Read dimensions
			fread(&width, sizeof(btui16), 1, file);
			fread(&height, sizeof(btui16), 1, file);
			// Read pixel buffer
			graphics::colour* buffer = new colour[width * height];
			fread(buffer, sizeof(graphics::colour), width * height, file);

			fclose(file);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, glID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

			SetFilterMode[fm]();
			SetEdgeMode[em]();

			delete[] buffer;
		}
	}
	void Texture::InitRenderTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		// The texture we're going to render to
		//unsigned int rendertexture;
		glGenTextures(1, &glID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#ifdef DEF_MULTISAMPLE // Multisampled
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#ifdef DEF_HDR // HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA16F, x, y, GL_TRUE); //create a blank image
		#else // Not HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA, x, y, GL_TRUE); //create a blank image
		#endif
		#else // Not Multisampled
		glBindTexture(GL_TEXTURE_2D, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#ifdef DEF_HDR // HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, x, y, 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
		#else // Not HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //create a blank image
		#endif
		#endif
		EMClamp();
		linear ? FMLinear() : FMNearest();
		//attach this texture to the framebuffer
		#ifdef DEF_MULTISAMPLE
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, glID, 0);
		#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glID, 0);
		#endif
	}
	void Texture::InitDepthTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		glGenTextures(1, &glID);

		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (linear)
		{
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
	void Texture::InitShadowTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		glGenTextures(1, &glID);
		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		if (linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		//attach this texture to the framebuffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glID, 0);

		//We only need the depth information when rendering the scene from the light's perspective
		//so there is no need for a color buffer. A framebuffer object however is not complete without
		//a color buffer so we need to explicitly tell OpenGL we're not going to render any color data.
		//We do this by setting both the read and draw buffer to GL_NONE with glDrawBuffer and glReadbuffer.

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	void Texture::InitDepthBuffer(int x, int y, bool linear)
	{
		width = x;
		height = y;

		//render buffer object (depth)
		//unsigned int depthbuffer;
		glGenRenderbuffers(1, &glID);
		glBindRenderbuffer(GL_RENDERBUFFER, glID);
		#ifdef DEF_MULTISAMPLE
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, DEF_MULTISAMPLE_DEPTH, GL_DEPTH24_STENCIL8, x, y);
		#else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);
		#endif

		//Once we've allocated enough memory for the renderbuffer object we can unbind the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//bind rbo to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glID);
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- MESH

	void Mesh::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}
	void Mesh::LoadFile(char* fn, bool clearmem)
	{
		std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			//Vert* vces; // Vertices
			//size_t vces_size;
			//btui32* ices; // Indices
			//size_t ices_size;

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			//-------------------------------- READ VERTICES

			fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
			vces = (Vertex*)malloc(sizeof(Vertex) * vces_size); // Allocate buffer to hold our vertices
			fread(&vces[0], sizeof(Vertex), vces_size, in); // Read vertices

			//-------------------------------- READ INDICES

			fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
			ices = (btui32*)malloc(sizeof(btui32) * ices_size); // Allocate buffer to hold our indicess
			fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

			fclose(in);

			//glID = (GLuint)ices_size; // Set number of indices used in Draw()

			//-------------------------------- INITIALIZE OPENGL BUFFER

			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

			glBindVertexArray(vao); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			glEnableVertexAttribArray(VI_POS); // Set Vertex positions
			glVertexAttribPointer(VI_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_POS);
			glEnableVertexAttribArray(VI_NOR); // Set Vertex normals
			glVertexAttribPointer(VI_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_NOR);
			glEnableVertexAttribArray(VI_UVC); // Set Vertex texture coords
			glVertexAttribPointer(VI_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_UVC);
			glEnableVertexAttribArray(VI_COL); // Set Vertex colour
			glVertexAttribPointer(VI_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VO_COL);

			glBindVertexArray(0); // Bind default vertex array

			if (clearmem)
			{
				free(vces);
				free(ices);
			}

			std::cout << "Generated Mesh!" << std::endl;
		}
	}
	void Mesh::Unload()
	{
		free(vces);
		free(ices);
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- MESH BLEND

	void MeshBlend::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, tex);

		// draw mesh
		glBindVertexArray(vao);
		//glDrawElements(GL_TRIANGLES, ices.size(), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}
	void MeshBlend::LoadFile(char* fn)
	{
		std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			VertexBlend* vces; // Vertices
			size_t vces_size;
			btui32* ices; // Indices

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			//-------------------------------- READ VERTICES

			fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
			vces = (VertexBlend*)malloc(sizeof(VertexBlend) * vces_size); // Allocate buffer to hold our vertices
			fread(&vces[0], sizeof(VertexBlend), vces_size, in); // Read vertices

			//-------------------------------- READ INDICES

			fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
			ices = (btui32*)malloc(sizeof(btui32) * ices_size); // Allocate buffer to hold our indicess
			fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

			fclose(in);

			ices_size = (GLuint)ices_size; // Set number of indices used in Draw()

			//-------------------------------- INITIALIZE OPENGL BUFFER

			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

			glBindVertexArray(vao); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexBlend), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			glEnableVertexAttribArray(graphics::vbi_pos_a); // Set Vertex positions
			glVertexAttribPointer(graphics::vbi_pos_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_pos_a);
			glEnableVertexAttribArray(graphics::vbi_pos_b);
			glVertexAttribPointer(graphics::vbi_pos_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_pos_b);

			glEnableVertexAttribArray(graphics::vbi_nor_a); // Set Vertex normals
			glVertexAttribPointer(graphics::vbi_nor_a, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_nor_a);
			glEnableVertexAttribArray(graphics::vbi_nor_b);
			glVertexAttribPointer(graphics::vbi_nor_b, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_nor_b);

			glEnableVertexAttribArray(graphics::vbi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(graphics::vbi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_uvc);

			glEnableVertexAttribArray(graphics::vbi_col); // Set Vertex colour
			glVertexAttribPointer(graphics::vbi_col, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBlend), (void*)graphics::vb_col);

			glBindVertexArray(0); // Bind default vertex array
			free(vces);
			free(ices);

			std::cout << "Generated Mesh Blend!" << std::endl;
		}
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- MESH DEFORM

	void MeshDeform::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, tex);

		// draw mesh
		glBindVertexArray(vao);
		//glDrawElements(GL_TRIANGLES, ices.size(), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	void MeshDeform::LoadFile(char* fn)
	{
		std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			VertexDeform* vces; // Vertices
			size_t vces_size;
			btui32* ices; // Indices

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			//-------------------------------- READ VERTICES

			fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
			vces = (VertexDeform*)malloc(sizeof(VertexDeform) * vces_size); // Allocate buffer to hold our vertices
			fread(&vces[0], sizeof(VertexDeform), vces_size, in); // Read vertices

			//-------------------------------- READ INDICES

			fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
			ices = (btui32*)malloc(sizeof(btui32) * ices_size); // Allocate buffer to hold our indicess
			fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

			fclose(in);

			ices_size = (GLuint)ices_size; // Set number of indices used in Draw()

			//-------------------------------- INITIALIZE OPENGL BUFFER

			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

			glBindVertexArray(vao); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(VertexDeform), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			glEnableVertexAttribArray(vdi_pos); // Set Vertex positions
			glVertexAttribPointer(vdi_pos, 3u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_pos);
			glEnableVertexAttribArray(vdi_nor); // Set Vertex normals
			glVertexAttribPointer(vdi_nor, 3u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_nor);
			glEnableVertexAttribArray(vdi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(vdi_uvc, 2u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_uvc);
			glEnableVertexAttribArray(vdi_col); // Set Vertex colour
			glVertexAttribPointer(vdi_col, 4u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)vd_col);
			// Set the matrix value array
			for (btui32 i = 0u; i < MD_MATRIX_COUNT; ++i)
			{
				glEnableVertexAttribArray(vdi_mat + i); // Set Vertex colour
				glVertexAttribPointer(vdi_mat + i, 1u, GL_FLOAT, GL_FALSE, sizeof(VertexDeform), (void*)(vd_mat + (sizeof(btf32) * i)));
			}

			glBindVertexArray(0); // Bind default vertex array
			free(vces);
			free(ices);

			std::cout << "Generated Mesh Deform!" << std::endl;
		}
	}
	
	//________________________________________________________________________________________________________________________________
	//-------------------------------- COMPOSITE MESH

	CompositeMesh::CompositeMesh()
	{
		//int i = 0;
	}
	CompositeMesh::~CompositeMesh()
	{
		if (vces != nullptr)
			delete[] vces;
		if (ices != nullptr)
			delete[] ices;
	}
	void CompositeMesh::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}
	// TODO: add matrix offset
	// read/search: c++ multiply vector3 x matrix4x4
	void CompositeMesh::AddMesh(Mesh* mesh, Matrix4x4 position)
	{
		glm::vec3 vector;
		glm::mat4x4 matr;

		//glm::vec3 vc2 = vector * matr;

		//
		size_t old_vces_size = vces_size;
		size_t old_ices_size = ices_size;
		//
		vces_size += mesh->VcesSize();
		ices_size += mesh->IcesSize();
		//
		Vertex* vces_2 = new Vertex[vces_size];
		btui32* ices_2 = new btui32[ices_size];
		// copy existing vertices into the new buffer
		for (int i = 0; i < old_vces_size; ++i)
			vces_2[i] = vces[i];
		for (int i = 0; i < old_ices_size; ++i)
			ices_2[i] = ices[i];
		// Copy in the things from the new mesh
		for (int i = old_vces_size; i < vces_size; ++i)
		{
			vces_2[i] = mesh->Vces()[i - old_vces_size];
			// temp
			m::Vector3 vector = m::Vector3(vces_2[i].pos.x, vces_2[i].pos.y, vces_2[i].pos.z);
			vector = vector * position;
			vces_2[i].pos.x = vector.x;
			vces_2[i].pos.y = vector.y;
			vces_2[i].pos.z = vector.z;
		}
		for (int i = old_ices_size; i < ices_size; ++i)
			ices_2[i] = mesh->Ices()[i - old_vces_size] + (btui32)old_vces_size;
		// Clear the old buffers from memory
		delete[] vces; vces = vces_2;
		delete[] ices; ices = ices_2;

		//-------------------------------- INITIALIZE OPENGL BUFFER

		if (old_vces_size == 0)
		{
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}

		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

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

	void CompositeMesh::AddMesh(Mesh* mesh, m::Vector3 position)
	{
		glm::vec3 vector;
		glm::mat4x4 matr;

		//glm::vec3 vc2 = vector * matr;

		//
		size_t old_vces_size = vces_size;
		size_t old_ices_size = ices_size;
		//
		vces_size += mesh->VcesSize();
		ices_size += mesh->IcesSize();
		//
		Vertex* vces_2 = new Vertex[vces_size];
		btui32* ices_2 = new btui32[ices_size];
		// copy existing vertices into the new buffer
		for (int i = 0; i < old_vces_size; ++i)
			vces_2[i] = vces[i];
		for (int i = 0; i < old_ices_size; ++i)
			ices_2[i] = ices[i];
		// Copy in the things from the new mesh
		for (int i = old_vces_size; i < vces_size; ++i)
		{
			vces_2[i] = mesh->Vces()[i - old_vces_size];
			// temp
			m::Vector3 vector = m::Vector3(vces_2[i].pos.x, vces_2[i].pos.y, vces_2[i].pos.z);
			vector = vector + position;
			//vector = vector + position * m::Vector3(1.f, 0.f, 1.f);

			//vector.y += m::Lerp(height_s, height_n, vces_2[i].pos.z * 2.f) + m::Lerp(height_w, height_e, vces_2[i].pos.x * 2.f);
			//vector.y *= 0.5f;
			//vector.z += vces_2[i].pos.x;

			// BILINEAR
			/*
			out_height = m::Lerp(
			m::Lerp((btf32)eCells[csinf.c[eCELL_I].x][csinf.c[eCELL_I].y].height, (btf32)eCells[csinf.c[eCELL_X].x][csinf.c[eCELL_X].y].height, abs(csinf.offsetx)),
			m::Lerp((btf32)eCells[csinf.c[eCELL_Y].x][csinf.c[eCELL_Y].y].height, (btf32)eCells[csinf.c[eCELL_XY].x][csinf.c[eCELL_XY].y].height, abs(csinf.offsetx)),
			abs(csinf.offsety)) / TERRAIN_HEIGHT_DIVISION;
			*/

			vces_2[i].pos.x = vector.x;
			vces_2[i].pos.y = vector.y;
			vces_2[i].pos.z = vector.z;
		}
		for (int i = old_ices_size; i < ices_size; ++i)
			ices_2[i] = mesh->Ices()[i - old_vces_size] + (btui32)old_vces_size;
		// Clear the old buffers from memory
		delete[] vces; vces = vces_2;
		delete[] ices; ices = ices_2;

		//-------------------------------- INITIALIZE OPENGL BUFFER

		if (old_vces_size == 0)
		{
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}
	}

	void CompositeMesh::AddTerrainTile(btui16(&HEIGHTMAP)[WORLD_SIZE][WORLD_SIZE])
	{
		glm::vec3 vector;
		glm::mat4x4 matr;

		//glm::vec3 vc2 = vector * matr;

		int tile_radius = 128;

		vces_size = 4u * (tile_radius * tile_radius);
		ices_size = 6u * (tile_radius * tile_radius);
		//
		vces = new Vertex[vces_size];
		ices = new btui32[ices_size];

		btf32 uvscale = 0.5f;
		int v = 0;
		int i = 0;
		for (int x = 1024 - (tile_radius / 2); x < 1024 + (tile_radius / 2); ++x)
		{
			for (int y = 1024 - (tile_radius / 2); y < 1024 + (tile_radius / 2); ++y, v += 4, i += 6)
			{
				bool cliffNS = false;
				bool cliffEW = false;

				if (HEIGHTMAP[x][y] > HEIGHTMAP[x + 1][y] + 5ui16 || HEIGHTMAP[x][y] < HEIGHTMAP[x + 1][y] - 5ui16)
					cliffNS = true;
				if (HEIGHTMAP[x][y] > HEIGHTMAP[x][y + 1] + 5ui16 || HEIGHTMAP[x][y] < HEIGHTMAP[x][y + 1] - 5ui16)
					cliffEW = true;
				
				// Copy in the things from the new mesh
				vces[v + 0].pos.x = (btf32)x;
				vces[v + 0].pos.z = (btf32)y;
				//vces[v + 0].pos.y = (((btf32)HEIGHTMAP[x - 1][y - 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 0].pos.y = (((btf32)HEIGHTMAP[x][y])) / TERRAIN_HEIGHT_DIVISION;
				if (cliffEW)
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.x * uvscale;
					vces[v + 0].uvc.y = -vces[v + 0].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.z * uvscale;
					vces[v + 0].uvc.y = -vces[v + 0].pos.y * uvscale;
				}
				else
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.x * uvscale;
					vces[v + 0].uvc.y = vces[v + 0].pos.z * uvscale;
				}
				vces[v + 0].nor.y = 1.f;
				vces[v + 0].nor.x = 0.f;
				vces[v + 0].nor.z = 0.f;
				vces[v + 0].col.y = 1.f;

				vces[v + 1u].pos.x = (btf32)x;
				vces[v + 1u].pos.z = (btf32)y + 0.25f;
				//vces[v + 1u].pos.y = (((btf32)HEIGHTMAP[x - 1][y + 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 1u].pos.y = (((btf32)HEIGHTMAP[x][y + 1])) / TERRAIN_HEIGHT_DIVISION;
				if (cliffEW)
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.x * uvscale;
					vces[v + 1].uvc.y = -vces[v + 1].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.z * uvscale;
					vces[v + 1].uvc.y = -vces[v + 1].pos.y * uvscale;
				}
				else
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.x * uvscale;
					vces[v + 1].uvc.y = vces[v + 1].pos.z * uvscale;
				}
				vces[v + 1].nor.y = 1.f;
				vces[v + 1].nor.x = 0.f;
				vces[v + 1].nor.z = 0.f;
				vces[v + 1].col.y = 1.f;

				vces[v + 2u].pos.x = (btf32)x + 0.25f;
				vces[v + 2u].pos.z = (btf32)y;
				//vces[v + 2u].pos.y = (((btf32)HEIGHTMAP[x + 1][y - 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 2u].pos.y = (((btf32)HEIGHTMAP[x + 1][y])) / TERRAIN_HEIGHT_DIVISION;
				if (cliffEW)
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.x * uvscale;
					vces[v + 2].uvc.y = -vces[v + 2].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.z * uvscale;
					vces[v + 2].uvc.y = -vces[v + 2].pos.y * uvscale;
				}
				else
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.x * uvscale;
					vces[v + 2].uvc.y = vces[v + 2].pos.z * uvscale;
				}
				vces[v + 2].nor.y = 1.f;
				vces[v + 2].nor.x = 0.f;
				vces[v + 2].nor.z = 0.f;
				vces[v + 2].col.y = 1.f;

				vces[v + 3u].pos.x = (btf32)x + 0.25f;
				vces[v + 3u].pos.z = (btf32)y + 0.25f;
				//vces[v + 3u].pos.y = (((btf32)HEIGHTMAP[x + 1][y + 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 3u].pos.y = (((btf32)HEIGHTMAP[x + 1][y + 1])) / TERRAIN_HEIGHT_DIVISION;
				if (cliffEW)
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.x * uvscale;
					vces[v + 3].uvc.y = -vces[v + 3].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.z * uvscale;
					vces[v + 3].uvc.y = -vces[v + 3].pos.y * uvscale;
				}
				else
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.x * uvscale;
					vces[v + 3].uvc.y = vces[v + 3].pos.z * uvscale;
				}
				vces[v + 3].nor.y = 1.f;
				vces[v + 3].nor.x = 0.f;
				vces[v + 3].nor.z = 0.f;
				vces[v + 3].col.y = 1.f;

				ices[i + 0u] = v + 0u;
				ices[i + 1u] = v + 3u;
				ices[i + 2u] = v + 1u;
				ices[i + 3u] = v + 2u;
				ices[i + 4u] = v + 3u;
				ices[i + 5u] = v + 0u;
			}
		}

		//-------------------------------- INITIALIZE OPENGL BUFFER

		//if (old_vces_size == 0)
		{
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}
	}

	void CompositeMesh::ReBindGL()
	{
		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

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

	//________________________________________________________________________________________________________________________________
	//-------------------------------- TERRAIN MESH

	void MeshTerrain::Draw()
	{
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, ices_size, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}

	void MeshTerrain::GenerateFromHMap(
		btui16(&hmap)[WORLD_SIZE][WORLD_SIZE],
		btui8(&MATMAP)[WORLD_SIZE][WORLD_SIZE])
	{
		glm::vec3 vector;
		glm::mat4x4 matr;

		//glm::vec3 vc2 = vector * matr;

		int tile_radius = 128;

		vces_size = 4u * (tile_radius * tile_radius);
		ices_size = 6u * (tile_radius * tile_radius);
		//
		vces = new VertexTerrain[vces_size];
		ices = new btui32[ices_size];

		btf32 uvscale = 0.5f;
		int v = 0;
		int i = 0;
		btf32 tile_width = 1.f;
		for (int x = 1024 - (tile_radius / 2); x < 1024 + (tile_radius / 2); ++x)
		{
			for (int y = 1024 - (tile_radius / 2); y < 1024 + (tile_radius / 2); ++y, v += 4, i += 6)
			{
				bool cliffNS = false;
				bool cliffEW = false;

				if (hmap[x][y] > hmap[x + 1][y] + 5ui16 || hmap[x][y] < hmap[x + 1][y] - 5ui16)
					cliffNS = true;
				if (hmap[x][y] > hmap[x][y + 1] + 5ui16 || hmap[x][y] < hmap[x][y + 1] - 5ui16)
					cliffEW = true;

				// Copy in the things from the new mesh
				vces[v + 0].pos.x = (btf32)x;
				vces[v + 0].pos.z = (btf32)y;
				//vces[v + 0].pos.y = (((btf32)HEIGHTMAP[x - 1][y - 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 0].pos.y = (((btf32)hmap[x][y])) / TERRAIN_HEIGHT_DIVISION;
				/*if (cliffEW)
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.x * uvscale;
					vces[v + 0].uvc.y = -vces[v + 0].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.z * uvscale;
					vces[v + 0].uvc.y = -vces[v + 0].pos.y * uvscale;
				}
				else*/
				{
					vces[v + 0].uvc.x = vces[v + 0].pos.x * uvscale;
					vces[v + 0].uvc.y = vces[v + 0].pos.z * uvscale;
				}
				vces[v + 0].nor.y = 1.f;
				vces[v + 0].nor.x = 0.f;
				vces[v + 0].nor.z = 0.f;
				vces[v + 0].col.y = 1.f;

				vces[v + 1u].pos.x = (btf32)x;
				vces[v + 1u].pos.z = (btf32)y + tile_width;
				//vces[v + 1u].pos.y = (((btf32)HEIGHTMAP[x - 1][y + 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 1u].pos.y = (((btf32)hmap[x][y + 1])) / TERRAIN_HEIGHT_DIVISION;
				/*if (cliffEW)
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.x * uvscale;
					vces[v + 1].uvc.y = -vces[v + 1].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.z * uvscale;
					vces[v + 1].uvc.y = -vces[v + 1].pos.y * uvscale;
				}
				else*/
				{
					vces[v + 1].uvc.x = vces[v + 1].pos.x * uvscale;
					vces[v + 1].uvc.y = vces[v + 1].pos.z * uvscale;
				}
				vces[v + 1].nor.y = 1.f;
				vces[v + 1].nor.x = 0.f;
				vces[v + 1].nor.z = 0.f;
				vces[v + 1].col.y = 1.f;

				vces[v + 2u].pos.x = (btf32)x + tile_width;
				vces[v + 2u].pos.z = (btf32)y;
				//vces[v + 2u].pos.y = (((btf32)HEIGHTMAP[x + 1][y - 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 2u].pos.y = (((btf32)hmap[x + 1][y])) / TERRAIN_HEIGHT_DIVISION;
				/*if (cliffEW)
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.x * uvscale;
					vces[v + 2].uvc.y = -vces[v + 2].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.z * uvscale;
					vces[v + 2].uvc.y = -vces[v + 2].pos.y * uvscale;
				}
				else*/
				{
					vces[v + 2].uvc.x = vces[v + 2].pos.x * uvscale;
					vces[v + 2].uvc.y = vces[v + 2].pos.z * uvscale;
				}
				vces[v + 2].nor.y = 1.f;
				vces[v + 2].nor.x = 0.f;
				vces[v + 2].nor.z = 0.f;
				vces[v + 2].col.y = 1.f;

				vces[v + 3u].pos.x = (btf32)x + tile_width;
				vces[v + 3u].pos.z = (btf32)y + tile_width;
				//vces[v + 3u].pos.y = (((btf32)HEIGHTMAP[x + 1][y + 1] + (btf32)HEIGHTMAP[x][y]) * 0.5f) / TERRAIN_HEIGHT_DIVISION;
				vces[v + 3u].pos.y = (((btf32)hmap[x + 1][y + 1])) / TERRAIN_HEIGHT_DIVISION;
				/*if (cliffEW)
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.x * uvscale;
					vces[v + 3].uvc.y = -vces[v + 3].pos.y * uvscale;
				}
				else if (cliffNS)
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.z * uvscale;
					vces[v + 3].uvc.y = -vces[v + 3].pos.y * uvscale;
				}
				else*/
				{
					vces[v + 3].uvc.x = vces[v + 3].pos.x * uvscale;
					vces[v + 3].uvc.y = vces[v + 3].pos.z * uvscale;
				}
				vces[v + 3].nor.y = 1.f;
				vces[v + 3].nor.x = 0.f;
				vces[v + 3].nor.z = 0.f;
				vces[v + 3].col.y = 1.f;

				if (MATMAP[x][y] == 1ui8)
					vces[v + 0].col.x = 1.f;
				if (MATMAP[x][y + 1] == 1ui8)
					vces[v + 1].col.x = 1.f;
				if (MATMAP[x + 1][y] == 1ui8)
					vces[v + 2].col.x = 1.f;
				if (MATMAP[x + 1][y + 1] == 1ui8)
					vces[v + 3].col.x = 1.f;

				ices[i + 0u] = v + 0u;
				ices[i + 1u] = v + 3u;
				ices[i + 2u] = v + 1u;
				ices[i + 3u] = v + 2u;
				ices[i + 4u] = v + 3u;
				ices[i + 5u] = v + 0u;
			}
		}

		//-------------------------------- INITIALIZE OPENGL BUFFER

		//if (old_vces_size == 0)
		{
			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer
		}
	}

	void MeshTerrain::ReBindGL()
	{
		//-------------------------------- UPDATE OPENGL BUFFER

		glBindVertexArray(vao); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(Vertex), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(VT_I_POS); // Set Vertex positions
		glVertexAttribPointer(VT_I_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VT_O_POS);
		glEnableVertexAttribArray(VT_I_NOR); // Set Vertex normals
		glVertexAttribPointer(VT_I_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VT_O_NOR);
		glEnableVertexAttribArray(VT_I_UVC); // Set Vertex texture coords
		glVertexAttribPointer(VT_I_UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VT_O_UVC);
		glEnableVertexAttribArray(VT_I_COL); // Set Vertex colour
		glVertexAttribPointer(VT_I_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)VT_O_COL);

		glBindVertexArray(0); // Bind default vertex array
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- GUI BITMAP

	GUIBitmap::GUIBitmap()
	{
	}
	void GUIBitmap::Init()
	{
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
	void GUIBitmap::SetTexture(GLuint tex)
	{
		tid = tex;
	}
	void GUIBitmap::SetShader(Shader* shader)
	{
		s = shader;
	}
	void GUIBitmap::Draw(int posx, int posy, int w, int h)
	{
		// create transformations
		glm::mat4 transform = glm::mat4(1.0f);
		//scale to screen space

		transform = glm::scale(transform, glm::vec3(2.f / FrameSizeX(), 2.f / FrameSizeY(), 1.0f));
		transform = glm::translate(transform, glm::vec3(posx, posy, 0));
		transform = glm::scale(transform, glm::vec3(w / 2, h / 2, 1.0f));

		//scale 0.5 height for orthographic
		//transform = glm::scale(transform, glm::vec3(1.f, 0.5f, 1.0f));
		//rotate for ortho (clipping range problem but has depth buffer)
		//transform = glm::rotate(transform, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		//spin rotate
		//transform = glm::rotate(transform, (float)time, glm::vec3(0.0f, 0.0f, 1.0f));

		//transform = glm::perspective(glm::radians(45.0f), (float)winx / (float)winy, 1.0f, 10.0f);

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

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

	Vertex createvert(float x, float y, float z, float r, float g, float b, float u, float v)
	{
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
		quad(float _x1, float _x2, float _y1, float _y2, float _u1, float _u2, float _v1, float _v2)
		{
			x1 = _x1; x2 = _x2;
			y1 = _y1; y2 = _y2;
			u1 = _u1; u2 = _u2;
			v1 = _v1; v2 = _v2;
		}
	};
	void AddQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, quad p, int index)
	{
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

	void GUIBox::Init()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIBox::ReGen(bti16 _xA, bti16 _xB, bti16 _yA, bti16 _yB, btui16 mg, btui16 bl)
	{
		if (!initialized) Init();

		// create transformations
		transform = glm::mat4(1.0f);

		//manual vector version
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		bti16 xA = _xA - bl;
		bti16 xB = _xB + bl;
		bti16 yA = _yA - bl;
		bti16 yB = _yB + bl;

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
		transform = glm::scale(transform, glm::vec3(2.f / FrameSizeX(), 2.f / FrameSizeY(), 1.0f));
	}
	void GUIBox::Draw(Texture* t)
	{
		Shader* s = &gPtr->shaders[S_GUI];

		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, t->glID);
		s->Use();

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glm::mat4 mattemp = glm::mat4(1);

		// get matrix's uniform location and set matrix
		unsigned int offsetLoc = glGetUniformLocation(s->ID, "offset");
		glUniformMatrix4fv(offsetLoc, 1, GL_FALSE, glm::value_ptr(mattemp));

		// Draw

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6 * 9, GL_UNSIGNED_INT, 0);
	}

	void GUIText::Init()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIText::SetOffset(int X, int Y)
	{
		// create transformations
		offset = glm::mat4(1.0f);
		//add offset
		offset = glm::translate(offset, glm::vec3(X, Y, 0));
	}

	void GUIText::GetTextBounds()
	{

	}

	//backup
	//void GUIText::ReGen(char* c, unsigned int stringlength, bti16 _xa, bti16 _xb, bti16 _y)

	void GUIText::ReGen(char* c, bti16 _xa, bti16 _xb, bti16 _y)
	{
		if (!initialized) Init();

		xa = _xa; xb = _xb; y = _y;

		int letterspace = 1;

		//reset lines number
		lines = 1;

		// create transformations
		transform = glm::mat4(1.0f);

		//manual vector version
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		//character height
		int ch = 12;

		int x = xa; int y2 = y - ch - 2;

		cnum = 0;
		width = 0;
		lastLineWidth = 0;

		sizex = sizey = 0u;
		sizey += ch;

		btui32 stringlength = strlen(c);

		//print text
		if (stringlength > 0)
		{
			lines++;

			int last_space = 0;

			//insert linebreaks
			for (int i = 0; i <= stringlength; i++)
			{
				if (c[i] == CHARCODE::space)
				{
					last_space = i;
				}
				else
				{
					//line break if we've gone past the edge
					if (x + font_kerning[c[i]] + 2 > xb)
					{
						if (last_space != 0) c[last_space] = CHARCODE::LB;
						x = xa;
						y2 -= ch;
						lines++;
						sizey += ch;
						lastLineWidth = 0;
					}
					else
					{
						cnum++;
						x += font_kerning[c[i]] + letterspace;
						width += font_kerning[c[i]] + letterspace;
						lastLineWidth += font_kerning[c[i]] + letterspace;
						if (lastLineWidth > sizex)
							sizex += font_kerning[c[i]] + letterspace;
					}
				}
			}

			x = xa; y2 = y - ch - 2;

			cnum = 0;
			width = 0;
			lastLineWidth = 0;

			sizex = sizey = 0u;
			sizey += ch;

			// print
			for (int i = 0; i <= stringlength; i++)
			{
				//set UV coords
				int u = c[i] % 16;
				int v = floor(c[i] / 16);
				////line break if we've gone past the edge
				//if (c[i] == CHARCODE::LB)
				//{
				//	x = xa;
				//	y2 -= ch;
				//	lines++;
				//	sizey += ch;
				//	lastLineWidth = 0;
				//}
				//else
				//{
				//	AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
				//	cnum++;
				//	x += font_kerning[c[i]] + letterspace;
				//	width += font_kerning[c[i]] + letterspace;
				//	lastLineWidth += font_kerning[c[i]] + letterspace;
				//	if (lastLineWidth > sizex)
				//		sizex += font_kerning[c[i]] + letterspace;
				//}

				AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
				cnum++;
				x += font_kerning[c[i]] + letterspace;
				width += font_kerning[c[i]] + letterspace;
				lastLineWidth += font_kerning[c[i]] + letterspace;
				if (lastLineWidth > sizex)
					sizex += font_kerning[c[i]] + letterspace;
				if (c[i] == CHARCODE::LB)
				{
					x = xa;
					y2 -= ch;
					lines++;
					sizey += ch;
					lastLineWidth = 0;
				}
			}
		}
		else
		{
			//set UV coords
			int u = 0xa1 % 16;
			int v = floor(0xa1 / 16);
			AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
		}

		BindBuffers(vertices, indices, VAO, VBO, EBO);

		// create transformations
		transform = glm::mat4(1.0f);

		//scale to screen space
		transform = glm::scale(transform, glm::vec3(2.f / FrameSizeX(), 2.f / FrameSizeY(), 1.0f));
	}
	//the 'scroll' boolean feels kind of clunky but it will work for now
	void GUIText::Draw(Texture* t)
	{
		Shader* s = &gPtr->shaders[S_GUI];

		s->Use();

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// get matrix's uniform location and set matrix
		unsigned int offsetLoc = glGetUniformLocation(s->ID, "offset");
		glUniformMatrix4fv(offsetLoc, 1, GL_FALSE, glm::value_ptr(offset));

		//....................................... DRAW

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, cnum * 6, GL_UNSIGNED_INT, 0);
	}
}

void DrawMesh(btID id, graphics::Mesh& mdl, graphics::TextureBase tex, ShaderStyle charashader, graphics::Matrix4x4 matrix)
{
	graphics::Shader* shd = nullptr;
	switch (charashader)
	{
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
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&graphics::GetMatProj());
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&graphics::GetMatView());
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// Render the mesh
	mdl.Draw(tex.glID, shd->ID);
}

void DrawCompositeMesh(btID id, graphics::CompositeMesh& mdl, graphics::TextureBase tex, ShaderStyle charashader, graphics::Matrix4x4 matrix)
{
	graphics::Shader* shd = nullptr;
	switch (charashader)
	{
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
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&graphics::GetMatProj());
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&graphics::GetMatView());
	shd->setMat4(shd->matModel, *(graphics::Matrix4x4*)&matrix);

	// Render the mesh
	mdl.Draw(tex.glID, shd->ID);
}

void DrawBlendMesh(btID id, graphics::MeshBlend& mdl, btf32 bs, graphics::TextureBase tex, ShaderStyle charashader, graphics::Matrix4x4 matrix)
{
	graphics::Shader* shd = nullptr;
	switch (charashader)
	{
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
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&graphics::GetMatProj());
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&graphics::GetMatView());
	shd->setMat4(shd->matModel, matrix);
	// Set blend state
	shd->SetFloat(shd->fBlendState, bs);

	// Render the mesh
	mdl.Draw(tex.glID, shd->ID);
}

void DrawMeshDeform(
	btID id, graphics::MeshDeform& mdl, graphics::TextureBase tex,
	ShaderStyle charashader, btui32 matrix_count,
	graphics::Matrix4x4 transform_a = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_b = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_c = graphics::Matrix4x4(),
	graphics::Matrix4x4 transform_d = graphics::Matrix4x4())
{
	// Get the shader reference
	graphics::Shader* shd = &graphics::GetShader(graphics::S_SOLID_DEFORM);

	// Enable the shader
	shd->Use();

	// Set shader properties
	shd->setMat4(shd->matProject, *(graphics::Matrix4x4*)&graphics::GetMatProj());
	shd->setMat4(shd->matView, *(graphics::Matrix4x4*)&graphics::GetMatView());
	shd->SetUint(shd->uiMatrixCount, matrix_count);
	if (matrix_count >= 1u) shd->setMat4(shd->matModelA, transform_a);
	if (matrix_count >= 2u) shd->setMat4(shd->matModelB, transform_b);
	if (matrix_count >= 3u) shd->setMat4(shd->matModelC, transform_c);
	if (matrix_count == 4u) shd->setMat4(shd->matModelD, transform_d);

	// Render the mesh
	mdl.Draw(tex.glID, shd->ID);
}
