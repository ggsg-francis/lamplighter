#pragma once
//opengl stuff
#include <glm\glm.hpp>
#include <glad\glad.h> // include glad to get all the required OpenGL headers
//assimp stuff
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags
//#include <assimp/config.h>		//why not config?
//c++ stuff
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "tga_reader.h"

namespace m
{
	class Vector3;
}

namespace CHARCODE_ASCII
{
	enum ASCII : btui8
	{
		NUL, // Null
		SOH, // Start of Heading
		STX, // Start of Text
		ETX, // End of Text
		EOT, // End of Transmission
		ENQ, // Enquiry
		ACK, // Acknowledgement
		BEL, // Bell
		BS, // Backspace
		HT, // Horizontal Tab
		LF, // Line Feed (Used as line break in this engine)
		VT, // Vertical Tab
		FF, // Form Feed
		CR, // Carriage Return
		SO, // Shift Out
		SI, // Shift In
		DLE, // Data Link Escape
		DC1, // Device Control 1
		DC2, // Device Control 2
		DC3, // Device Control 3
		DC4, // Device Control 4
		NAK, // Negative Acknowledgement
		SYN, // Synchronous Idle
		ETB, // End of Transmission Block
		CAN, // Cancel
		EM, // End of Medium
		SUB, // Substitute
		ESC, // Escape
		FS, // File Separator
		GS, // Group Separator
		RS, // Record Separator
		US, // Unit Separator
		space = 32u, // Space
		DEL = 127u, // Delete
	};
}
namespace CHARCODE
{
	// ASCII Standard, but with unneeded characters swapped for rendering based purposes
	enum LTRCODE : btui8
	{
		NUL, // Null
		LB, // Line Break
		HS, // Highlight Start
		HE, // Highlight End
		IS, // Italic Start
		IE, // Italic End
		// Tons of space here for new stuff
		space = 32u, // Space
		// All same as ASCII
		unused = 127u, // Not used yet
	};
}

namespace graphics
{
	struct Matrix3x3
	{
		btf32 m[3][3];
	};

	struct FRow4
	{
		btf32 e[4]; // Elements
		FRow4(btf32 a, btf32 b, btf32 c, btf32 d)
		{
			e[0] = a; e[1] = b; e[2] = c; e[3] = d;
		};
		btf32& operator[](const bti32 index) { return e[index]; };
		btf32 const& operator[](const bti32 index) const { return e[index]; };
	};
	struct Matrix4x4
	{
		//FRow4 v[4]{ FRow4(1.f, 0.f, 0.f, 0.f), FRow4(0.f, 1.f, 0.f, 0.f), FRow4(0.f, 0.f, 1.f, 0.f), FRow4(0.f, 0.f, 0.f, 1.f) };
		// Initialize with mirrored Z axis
		FRow4 v[4]{ FRow4(1.f, 0.f, 0.f, 0.f), FRow4(0.f, 1.f, 0.f, 0.f), FRow4(-0.f, -0.f, -1.f, -0.f), FRow4(0.f, 0.f, 0.f, 1.f) };
		FRow4& operator[](const bti32 index) { return v[index]; };
		FRow4 const& operator[](const bti32 index) const { return v[index]; };
	};

	// Translate matrix (copied from glm)
	Matrix4x4 MatrixTranslate(Matrix4x4 const& MATRIX, m::Vector3 const& VECTOR);
	// Rotate matrix (copied from glm)
	Matrix4x4 MatrixRotate(Matrix4x4 const& MATRIX, btf32 ANGLE, m::Vector3 const& VECTOR);
	// Scale matrix (copied from glm)
	Matrix4x4 MatrixScale(Matrix4x4 const& MATRIX, m::Vector3 const& VECTOR);
	// Rotate matrix to face direction (this function does not work, currently)
	Matrix4x4 MatrixPointDirection(Matrix4x4 const& MATRIX, m::Vector3 const& FORWARD_VECTOR, m::Vector3 const& UP_VECTOR);
	// Generate model location matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR);
	// Generate model location and yaw matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, btf32 YAW);
	// Generate model location and yaw/pitch matrix
	void MatrixTransform(Matrix4x4& MATRIX, m::Vector3 const& POSITION_VECTOR, btf32 YAW, btf32 PITCH);
	
	// Vertex structure
	struct vert {
		glm::vec3 pos; // Position
		glm::vec3 nor; // Normal
		glm::vec2 uvc; // UV Coords (TexCoords)
		glm::vec4 col; // Vertex Colour
	};

	// Vertex structure used for blending between two meshes
	struct vert_blend {
		glm::vec3 pos_a; // Position A
		glm::vec3 pos_b; // Position B
		glm::vec3 nor_a; // Normal A
		glm::vec3 nor_b; // Normal B
		glm::vec2 uvc; // UV Coords (TexCoords)
	};

	namespace vns
	{
		enum v_i : btui32 { // Vert Indices
			vi_pos, vi_nor, vi_uvc, vi_col,
		};
		enum v_o : size_t { // Vert Offsets
			v_pos = offsetof(vert, pos),
			v_nor = offsetof(vert, nor),
			v_uvc = offsetof(vert, uvc),
			v_col = offsetof(vert, col),
		};
		enum vb_i : btui32 { // VertBlend Indices
			vbi_pos_a, vbi_pos_b, vbi_nor_a, vbi_nor_b, vbi_uvc,
		};
		enum vb_o : size_t { // VertBlend Offsets
			vb_pos_a = offsetof(vert_blend, pos_a),
			vb_pos_b = offsetof(vert_blend, pos_b),
			vb_nor_a = offsetof(vert_blend, nor_a),
			vb_nor_b = offsetof(vert_blend, nor_b),
			vb_uvc = offsetof(vert_blend, uvc),
		};
	}

	struct colour
	{
		btui8 r = 0ui8;
		btui8 g = 0ui8;
		btui8 b = 0ui8;
		btui8 a = 0ui8;
		colour(btui8 x = 0ui8, btui8 y = 0ui8, btui8 z = 0ui8, btui8 w = 0ui8) : r{ x }, g{ y }, b{ z }, a{ w } {}
	};

	void Init();

	void SetMatProj(btf32 FOV_MULT = 1.f);
	void SetMatView(void* t);
	void SetMatProjLight();
	void SetMatViewLight(float x, float y, float z, float vx, float vy, float vz);

	glm::mat4 GetMatProj();
	glm::mat4 GetMatView();


	void SetFrontFace();
	void SetFrontFaceInverse();

	enum BlendMode : btui8
	{
		eSOLID,
		eTRANSPARENT,
		eADDITIVE,
	};

	class Shader
	{
	public:
		unsigned int ID;
		Shader();
		// constructor generates the shader on the fly
		Shader(const char*, const char*, const char* = nullptr);
		// Set this shader as current for rendering
		void Use();
		// Utility uniform functions
		void SetBool(const std::string &name, bool value) const;
		void SetInt(const std::string &name, int value) const;
		void SetUint(const std::string &name, unsigned int value) const;
		void SetFloat(const std::string &name, float value) const;
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec2(const std::string &name, float x, float y) const;
		void setVec3(const std::string &name, const glm::vec3 &value) const;
		void setVec3(const std::string &name, float x, float y, float z) const;
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w);
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		void setMat4(const std::string &name, const glm::mat4 &mat) const;
		void setMat4(const std::string &name, const Matrix4x4 &mat) const;
	private:
		// Utility function for checking shader compilation/linking errors
		void CheckCompileErrors(GLuint shader, std::string type);
	};

	enum TextureFilterMode : btui8
	{
		eNEAREST,
		eNEAREST_MIPMAP,
		eLINEAR,
		eLINEAR_MIPMAP,
	};
	enum TextureEdgeMode : btui8
	{
		eREPEAT,
		eREPEAT_MIRROR,
		eCLAMP,
		eREPEAT_X_CLAMP_Y,
		eCLAMP_X_REPEAT_Y,
	};

	struct TextureBase
	{
		GLuint glID;
		btui16 width;
		btui16 height;

		TextureFilterMode filter;
		TextureEdgeMode edge;

		void SetMode(TextureFilterMode _filter, TextureEdgeMode _edge)
		{
			filter = _filter; edge = _edge;
		}
	};

	struct ModifiableTexture : TextureBase
	{
		colour* buffer;

		~ModifiableTexture();
		void Init(btui16 SIZE_X, btui16 SIZE_Y, colour COLOUR);
		void LoadFile(char* FILENAME);
		void SetPixel(btui16 PIXEL_X, btui16 PIXEL_Y, colour COLOUR);
		void SetPixelChannelR(btui16 PIXEL_X, btui16 PIXEL_Y, btui8 VALUE);
		void SetPixelChannelG(btui16 PIXEL_X, btui16 PIXEL_Y, btui8 VALUE);
		void SetPixelChannelB(btui16 PIXEL_X, btui16 PIXEL_Y, btui8 VALUE);
		void SetPixelChannelA(btui16 PIXEL_X, btui16 PIXEL_Y, btui8 VALUE);
		colour GetPixel(btui16 PIXEL_X, btui16 PIXEL_Y);
		void ReBindGL(TextureFilterMode FILTER_MODE, TextureEdgeMode EDGE_MODE);
	};

	struct Texture : TextureBase
	{
		void LoadFile(char* FILENAME);
		// these were a quick hack at the time and honestly i don't know!
		void InitRenderTexture(int WIDTH, int HEIGHT, bool LINEAR);
		void InitDepthTexture(int WIDTH, int HEIGHT, bool LINEAR);
		void InitShadowTexture(int WIDTH, int HEIGHT, bool LINEAR);
		void InitRenderBuffer(int WIDTH, int HEIGHT, bool LINEAR);
	};

	class Mesh
	{
	public:
		GLuint glID;
		GLuint vao; // Vertex Array Object
		void Draw(unsigned int TEXTURE, unsigned int SHADER);
		void LoadFile(char* FILENAME);
		// add void unload?
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
	};

	class MeshBlend
	{
	public:
		GLuint glID;
		GLuint vao; // Vertex Array Object
		void Draw(unsigned int TEXTURE, unsigned int SHADER);
		void LoadFile(char* FILENAME);
		// add void unload?
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
	};

	class GUIBitmap
	{
	public:
		GLuint tid;
		Shader* s;
		unsigned int VBO, VAO, EBO;
		GUIBitmap();
		void Init();
		void SetTexture(GLuint TEXTURE);
		void SetShader(Shader* SHADER);
		//missing parameters obviously
		void Draw(int posx, int posy, int WIDTH, int HEIGHT);
	};

	extern Shader shader_solid; // Used for drawing static meshes
	extern Shader shader_scroll; // Used for drawing static meshes
	extern Shader shader_blend; // Used for drawing blended meshes
	extern Shader shader_terrain; // Used for drawing objects adjusted to the world heightmap
	extern Shader shader_shadow; // Used for drawing shadows
	extern Shader shader_sky; // Used for drawing... the sky
	extern Shader shader_gui; // GUI shader
	extern Shader shader_post; // Framebuffer postprocessing shader




	void DrawGUITexture(Texture* texture, Shader* shader, bti32 x, bti32 y, bti32 w, bti32 h);
















	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
	// (copied from ltr, yet to be optimized, integrated or fixed)

	#define INPUT_LEN_TEMP 256

	class GUIBox
	{
	private:
		bool initialized = false;
	protected:
		glm::mat4 transform;
		unsigned int VBO, VAO, EBO;
	public:
		void Init();
		void ReGen(bti16 XA, bti16 XB, bti16 YA, bti16 YB, btui16 MARGIN_SIZE, btui16 BLEED_SIZE = 0ui16);
		void Draw(Shader* SHADER, Texture* TEXTURE);
	};

	class GUIText
	{
	private:
		bool initialized = false;
	protected:
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 offset = glm::mat4(1.0f);
		unsigned int VBO, VAO, EBO;
		//NUMBER OF VERTICES
		int cnum;
		float xa, xb, y;
	public:
		btui16 sizex, sizey;
		void Init();
		//set boundaries of the text box
		void SetOffset(int X, int Y);
		void GetTextBounds();
		void ReGen(char* STRING, unsigned int LENGTH, bti16 XA, bti16 XB, bti16 Y);
		void Draw(Shader* SHADER, Texture* TEXTURE);
		int width;
		int lastLineWidth;
		//number of lines
		int lines = 1;
	};
}