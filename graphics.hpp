#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphics_structures.h"

// TODO: shit
#ifdef __GNUC__
#include "global.h"
#endif

//opengl stuff
#include <glm/glm.hpp>
#include <glad/glad.h> // include glad to get all the required OpenGL headers
//assimp stuff
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
//#include <assimp/config.h> // Tutorial told me to include this but it doesn't seem to be necessary
//c++ stuff
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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
	btui32 FrameSizeX();
	btui32 FrameSizeY();
	void SetFrameSize(btui32 x, btui32 y);

	// TODO: MOVE TO MATHS!!!
	struct Matrix3x3
	{
		btf32 m[3][3];
	};
	// TODO: MOVE TO MATHS!!!
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
	// TODO: MOVE TO MATHS!!!
	struct Matrix4x4
	{
		//FRow4 v[4]{ FRow4(1.f, 0.f, 0.f, 0.f), FRow4(0.f, 1.f, 0.f, 0.f), FRow4(0.f, 0.f, 1.f, 0.f), FRow4(0.f, 0.f, 0.f, 1.f) };
		// Initialize with mirrored Z axis
		FRow4 v[4]{ FRow4(1.f, 0.f, 0.f, 0.f), FRow4(0.f, 1.f, 0.f, 0.f), FRow4(-0.f, -0.f, -1.f, -0.f), FRow4(0.f, 0.f, 0.f, 1.f) };
		void Initialize(bool FLIP = true)
		{
			v[0] = FRow4(1.f, 0.f, 0.f, 0.f);
			v[1] = FRow4(0.f, 1.f, 0.f, 0.f);
			if (FLIP)
				v[2] = FRow4(-0.f, -0.f, -1.f, -0.f);
			else
				v[2] = FRow4(0.f, 0.f, 1.f, 0.f);
			v[3] = FRow4(0.f, 0.f, 0.f, 1.f);
		}
		FRow4& operator[](const bti32 index) { return v[index]; };
		FRow4 const& operator[](const bti32 index) const { return v[index]; };
	};
	// TODO: MOVE TO MATHS!!!
	m::Vector3 operator*(const m::Vector3& VECTOR, const Matrix4x4& MATRIX);

	// TODO: CLEAR THE RETURNS ON ALL OF THESE

	// Translate matrix (copied from glm)
	Matrix4x4 MatrixTranslate(Matrix4x4 const& MATRIX, m::Vector3 const& VECTOR);
	// Rotate matrix (copied from glm)
	Matrix4x4 MatrixRotate(Matrix4x4 const& MATRIX, btf32 ANGLE, m::Vector3 const& VECTOR);
	// Scale matrix (copied from glm)
	Matrix4x4 MatrixScale(Matrix4x4 const& MATRIX, m::Vector3 const& VECTOR);
	// For generating a camera matrix (copied from glm)
	Matrix4x4 MatrixLookAt(Matrix4x4 const& MATRIX, m::Vector3 const& SOURCE_POINT, m::Vector3 const& TARGET_POINT, m::Vector3 const& UP_DIRECTION);
	// Generate model location matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR);
	// Generate model location matrix
	void MatrixTransformXFlip(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR);
	// Generate model location and yaw matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, btf32 YAW);
	// Generate model location and yaw/pitch matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, btf32 YAW, btf32 PITCH);
	// Generate model location and rotation matrix
	void MatrixTransform(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, m::Vector3 const& FORWARD_VECTOR, m::Vector3 const& UP_VECTOR);
	void MatrixTransformForwardUp(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, m::Vector3 const& FORWARD_VECTOR, m::Vector3 const& UP_VECTOR);
	void MatrixTransformXFlip(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, m::Vector3 const& FORWARD_VECTOR, m::Vector3 const& UP_VECTOR);
	void MatrixTransformXFlipForwardUp(Matrix4x4& OUT_MATRIX, m::Vector3 const& POSITION_VECTOR, m::Vector3 const& FORWARD_VECTOR, m::Vector3 const& UP_VECTOR);
	Matrix4x4 MatrixGetMirror(Matrix4x4& SRC_MATRIX);
	m::Vector3 MatrixGetPosition(Matrix4x4& SRC_MATRIX);
	m::Vector3 MatrixGetForward(Matrix4x4& SRC_MATRIX);
	m::Vector3 MatrixGetRight(Matrix4x4& SRC_MATRIX);
	m::Vector3 MatrixGetUp(Matrix4x4& SRC_MATRIX);

	struct colour
	{
		btui8 r = 0u;
		btui8 g = 0u;
		btui8 b = 0u;
		btui8 a = 0u;
		colour(btui8 x = 0u, btui8 y = 0u, btui8 z = 0u, btui8 w = 0u) : r{ x }, g{ y }, b{ z }, a{ w } {}
	};

	void Init();
	void End();

	m::Vector3 GetViewPos();
	m::Vector3 GetFocalCenter();
	void SetMatProj(btf32 FOV_MULT = 1.f);
	void SetMatView(void* t, void* p, void* t2);
	void SetMatProjLight();
	void SetMatViewLight(float x, float y, float z, float vx, float vy, float vz);

	glm::mat4 GetMatProj();
	glm::mat4 GetMatView();


	void SetFrontFace();
	void SetFrontFaceInverse();
	void SetRenderSolid();
	void SetRenderWire();

	enum BlendMode : btui8
	{
		eSOLID,
		eTRANSPARENT,
		eADDITIVE,
	};

	class Shader
	{
	public:
		GLuint ID;
		enum PIndex
		{
			matModel,
			uiMatrixCount, // Deform shader
			matModelA, // Deform shader
			matModelB, // Deform shader
			matModelC, // Deform shader
			matModelD, // Deform shader
			matProject,
			matView,
			fBlendState, // Blend shader
			fWindowX, // Screen shader
			fWindowY, // Screen shader
			fTime,
			matLightProj,
			vecPCam,
			vecVSun,
			matTransform, // GUI shader
			bLit_TEMP,
			texShadowMap,
			texTerrain1, // Terrain shader
			texTerrain2, // Terrain shader
			texTerrain3, // Terrain shader
			texTerrain4, // Terrain shader
			texTerrain5, // Terrain shader
			texTerrain6, // Terrain shader
			texTerrain7, // Terrain shader
			texTerrain8, // Terrain shader
			Opacity, // GUI shader
			Colour_A, // Chara shader
			Colour_B, // Chara shader
			Colour_C, // Chara shader
			Colour_Sunlight,
			Colour_Ambient,
			Colour_Fog,
			Colour_Lightmap,
			Fog_Density,
			GUITransform,
			GUIOffset,
			LOCATION_COUNT,
		};
	private:
		// This stuff all takes up memory, so it will get duplicated with every shader
		// TODO: move it all into a single 'graphics struct'
		const char* names[LOCATION_COUNT]
		{
			"matm",
			"mc",
			"matma",
			"matmb",
			"matmc",
			"matmd",
			"matp",
			"matv",
			"blendState",
			"wx",
			"wy",
			"ft",
			"lightProj",
			"pcam",
			"vsun",
			"transform",
			"lit",
			"tshadow",
			"tt1",
			"tt2",
			"tt3",
			"tt4",
			"tt5",
			"tt6",
			"tt7",
			"tt8",
			"opa",
			"c_a",
			"c_b",
			"c_c",
			"cSun",
			"cAmb",
			"cFog",
			"cLit",
			"fFogDens",
			"transform",
			"offset",
		};
	public:
		// To ensure that there is no conflic with texture locations
		enum TIndex : GLint
		{
			TXTR_DIFFUSE0,
			TXTR_UNUSED_1,
			TXTR_UNUSED_2,
			TXTR_UNKN_3, // Doesnt seem to be used for anything
			TXTR_UNKN_4, // Doesnt seem to be used for anything
			TXTR_SHADOWMAP,
			TXTR_TERRAIN1,
			TXTR_TERRAIN2,
			TXTR_TERRAIN3,
			TXTR_TERRAIN4,
			TXTR_TERRAIN5,
			TXTR_TERRAIN6,
			TXTR_TERRAIN7,
			TXTR_TERRAIN8,
		};
	private:
		const GLenum temp[32u]
		{
			GL_TEXTURE0,
			GL_TEXTURE1,
			GL_TEXTURE2,
			GL_TEXTURE3,
			GL_TEXTURE4,
			GL_TEXTURE5,
			GL_TEXTURE6,
			GL_TEXTURE7,
			GL_TEXTURE8,
			GL_TEXTURE9,
			GL_TEXTURE10,
			GL_TEXTURE11,
			GL_TEXTURE12,
			GL_TEXTURE13,
			GL_TEXTURE14,
			GL_TEXTURE15,
			GL_TEXTURE16,
			GL_TEXTURE17,
			GL_TEXTURE18,
			GL_TEXTURE19,
			GL_TEXTURE20,
			GL_TEXTURE21,
			GL_TEXTURE22,
			GL_TEXTURE23,
			GL_TEXTURE24,
			GL_TEXTURE25,
			GL_TEXTURE26,
			GL_TEXTURE27,
			GL_TEXTURE28,
			GL_TEXTURE29,
			GL_TEXTURE30,
			GL_TEXTURE31,
		};
		GLint location[LOCATION_COUNT];
	public:
		Shader();
		// constructor generates the shader on the fly
		void Init(const char*, const char*, const char* = nullptr);
		// Set this shader as current for rendering
		void Use();
		// Utility uniform functions
		void SetBool(const PIndex index, bool value) const;
		void SetInt(const PIndex index, int value) const;
		void SetUint(const PIndex index, unsigned int value) const;
		void SetFloat(const PIndex index, float value) const;
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec2(const std::string &name, float x, float y) const;
		void setVec3(const PIndex index, const glm::vec3 &value) const;
		void setVec3(const PIndex index, float x, float y, float z) const;
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w);
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		void setMat4(const PIndex index, const Matrix4x4 &mat) const;
		void SetTexture(const PIndex index, GLuint texture, TIndex textureIndex);
	private:
		// Utility function for checking shader compilation/linking errors
		void CheckCompileErrors(GLuint shader, std::string type);
	};

	enum eShader
	{
		S_SOLID, // Used for drawing static meshes
		S_SOLID_CHARA, // Used for drawing static meshes
		S_SOLID_BLEND, // Used for drawing blended meshes
		S_SOLID_BLEND_CHARA, // Used for drawing blended meshes
		S_SOLID_DEFORM, // Used for drawing deformed meshes
		S_SOLID_DEFORM_CHARA, // Used for drawing deformed meshes
		S_SOLID_TERRAIN, // Used for drawing the terrain mesh
		S_MEAT,
		S_GUI, // GUI shader
		S_POST, // Framebuffer postprocessing shader

		S_COUNT, // Number of shaders used in the program

		S_UTIL_FIRST_LIT = S_SOLID, // First solid shader, for iterating when setting shader parameters
		S_UTIL_LAST_LIT = S_MEAT, // Last solid shader, for iterating when setting shader parameters
	};
	Shader& GetShader(eShader SHADER_ID);

	enum TextureFilterMode : btui8
	{
		eNEAREST,
		eNEAREST_MIPMAP,
		eLINEAR,
		eLINEAR_MIPMAP,
		eFOLIAGE,
	};
	enum TextureEdgeMode : btui8
	{
		eREPEAT,
		eREPEAT_MIRROR,
		eCLAMP,
		eREPEAT_X_CLAMP_Y,
		eCLAMP_X_REPEAT_Y,
	};

	struct TextureBuffer
	{
		btui16 width;
		btui16 height;
		colour* buffer;
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
		// Load this texture from a file
		void LoadFile(void* ACV_FILE);
		// Generate as a read-write capable RGBA render texture
		void InitRenderBuffer(GLuint FRAMEBUFFER, int WIDTH, int HEIGHT, bool LINEAR);
		// Function that creates the mysterious 'intermediate' buffer
		void InitIntermediateTest(GLuint FRAMEBUFFER);
		// Generate as a read-write capable depth texture which can be used in a shader
		void InitDepthBufferRW(GLuint FRAMEBUFFER, int WIDTH, int HEIGHT, bool LINEAR);
		// Generate as a write-only depth texture
		void InitDepthBufferW(GLuint FRAMEBUFFER, int WIDTH, int HEIGHT, bool LINEAR);
		// Generate as a shadowmap render texture
		void InitShadowBuffer(GLuint FRAMEBUFFER);
		//
		void Unload();
	};

	// Vertices and indices are kept in RAM so it can be used to compose a compositemesh
	// this isn't ideal
	class Mesh
	{
	public:
		GLuint vao; // Vertex Array Object
		void LoadFile(void* ACV_FILE, bool CLEARMEM);
		void Unload();
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
		Vertex* vces; // Vertices
		btui32* ices; // Indices
		size_t vces_size;
		size_t ices_size;
	public:
		// Probably all temporary
		Vertex* Vces() { return vces; };
		btui32* Ices() { return ices; };
		size_t VcesSize() { return vces_size; };
		size_t IcesSize() { return ices_size; };
	};

	class MeshBlend
	{
	private:
		size_t ices_size;
	public:
		GLuint vao; // Vertex Array Object
		void LoadFile(void* ACV_FILE);
		void Unload();
		size_t IcesSize() { return ices_size; };
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
	};

	class MeshDeform
	{
	private:
		size_t ices_size;
	public:
		GLuint vao; // Vertex Array Object
		void LoadFile(void* acv_file);
		void Unload();
		size_t IcesSize() { return ices_size; };
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
	};

	// Multiple meshes combine powers to become one megamesh
	class CompositeMesh
	{
	public:
		enum MeshOrientation
		{
			eNORTH, eSOUTH, eEAST, eWEST,
		};
		GLuint vao; // Vertex Array Object
		CompositeMesh();
		~CompositeMesh();
		void Draw(unsigned int texture, unsigned int shader);
		void AddMesh(Mesh* mesh, Matrix4x4 position);
		void AddMesh(Mesh* mesh, m::Vector3 position, MeshOrientation orientation);
		void ReBindGL();
		void Unload();
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
		Vertex* vces; // Vertices
		btui32* ices; // Indices
		size_t vces_size;
		size_t ices_size;
	};

	// Like a composite mesh but uses a different vertex type, specifically represents terrain
	class TerrainMesh
	{
	public:
		void Draw();
		void GenerateFromHMap(btui8(&heightmap)[WORLD_SIZE][WORLD_SIZE], btui8(&matmap)[WORLD_SIZE][WORLD_SIZE]);
		void GenerateComplexEnv(
			btui8(&heightmap)[WORLD_SIZE][WORLD_SIZE],
			btui8(&matmap)[WORLD_SIZE][WORLD_SIZE],
			btui32* flags,
			btui32 flag_invisible,
			btui8(&heightmap_ne)[WORLD_SIZE][WORLD_SIZE],
			btui8(&heightmap_nw)[WORLD_SIZE][WORLD_SIZE],
			btui8(&heightmap_se)[WORLD_SIZE][WORLD_SIZE],
			btui8(&heightmap_sw)[WORLD_SIZE][WORLD_SIZE]);
	private:
		void ReBindGL();
		GLuint vao = UI32_NULL; // Vertex Array Object
		GLuint vbo = UI32_NULL; // Vertex Buffer Object
		GLuint ebo = UI32_NULL; // Element Buffer Object
		VertexTerrain* vces = nullptr; // Vertices
		btui32* ices = nullptr; // Indices
		size_t vces_size;
		size_t ices_size;
	public:
		size_t GetVertexCount() { return vces_size; };
		size_t GetIndexCount() { return ices_size; };
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
		//missing parameters obviously (it's obvious??)
		void Draw(int posx, int posy, int WIDTH, int HEIGHT);
	};

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
		void ReGen(bti16 XA, bti16 XB, bti16 YA, bti16 YB, btui16 MARGIN_SIZE, btui16 BLEED_SIZE = 0u);
		void Draw(Texture* TEXTURE);
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
		void ReGen(char* STRING, bti16 XA, bti16 XB, bti16 Y);
		void Draw(Texture* TEXTURE);
		int width;
		int lastLineWidth;
		//number of lines
		int lines = 1;
	};

	void DrawGUITexture(Texture* TEXTURE, bti32 X, bti32 Y, bti32 W, bti32 H, btf32 OPACITY = 1.f);
	void DrawGUIBox(Texture* TEXTURE, bti16 XA, bti16 XB, bti16 YA, bti16 YB, btui16 MARGIN_SIZE, btui16 BLEED_SIZE = 0u);
	void DrawGUIText(char* STRING, Texture* TEXTURE, bti32 XA, bti32 XB, bti32 Y);
}

enum ShaderStyle
{
	SS_NORMAL,
	SS_CHARA,
};

void DrawMesh(btID ID, graphics::Mesh& MESH, graphics::TextureBase TEXTURE,
	ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawParticles(graphics::Mesh& MESH, graphics::TextureBase TEXTURE,
	graphics::Matrix4x4* MATRIX, btui32 COUNT);

void DrawBlendMesh(btID ID, graphics::MeshBlend& MODEL, btf32 BLENDSTATE,
	graphics::TextureBase TEXTURE, ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawMeshDeform(btID ID, graphics::MeshDeform& MODEL,
	graphics::TextureBase TEXTURE, ShaderStyle SHADER, btui32 MATRIX_COUNT,
	graphics::Matrix4x4 MATRIX_A, graphics::Matrix4x4 MATRIX_B,
	graphics::Matrix4x4 MATRIX_C, graphics::Matrix4x4 MATRIX_D);

void DrawCompositeMesh(btID ID, graphics::CompositeMesh& MESH,
	graphics::TextureBase TEXTURE, ShaderStyle SHADER, graphics::Matrix4x4 MATRIX);

void DrawTerrainMesh(btID ID, graphics::TerrainMesh MESH,
	graphics::TextureBase TEXTURE_A, graphics::TextureBase TEXTURE_B,
	graphics::TextureBase TEXTURE_C, graphics::TextureBase TEXTURE_D,
	graphics::TextureBase TEXTURE_E, graphics::TextureBase TEXTURE_F,
	graphics::TextureBase TEXTURE_G, graphics::TextureBase TEXTURE_H,
	graphics::Matrix4x4 MATRIX);

#endif
