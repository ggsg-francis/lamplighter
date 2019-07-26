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

unsigned int TxLoadTGA(const char *path, const std::string &directory, bool gamma = false);
unsigned int TxLoadTGA(const char *path, bool gamma = false);

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| VERTEX

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

namespace graphics
{
	glm::mat4 GetMatProj();
	void SetMatProj(float ofx, float ofy, float ofh, float yaw, float pitch);
	glm::mat4 GetMatView();
	void SetMatView(float ofx, float ofy, float vx, float vy);

	namespace vns
	{
		enum v
		{
			// Indices
			vi_pos, vi_nor, vi_uvc, vi_col,
			// Offsets
			v_pos = offsetof(vert, pos),
			v_nor = offsetof(vert, nor),
			v_uvc = offsetof(vert, uvc),
			v_col = offsetof(vert, col),
		};

		enum vb
		{
			// Indices
			vbi_pos_a, vbi_pos_b, vbi_nor_a, vbi_nor_b, vbi_uvc,
			// Offsets
			vb_pos_a = offsetof(vert_blend, pos_a),
			vb_pos_b = offsetof(vert_blend, pos_b),
			vb_nor_a = offsetof(vert_blend, nor_a),
			vb_nor_b = offsetof(vert_blend, nor_b),
			vb_uvc = offsetof(vert_blend, uvc),
		};
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| TEXTURE

struct Texture {

	GLuint id = 0;
	unsigned int width;
	unsigned int height;
	//Create new texture from dimensions
	//void CreateNew(int X, int Y, bool LINEAR);
	void InitRenderTexture(int X, int Y, bool LINEAR);
	void InitDepthTexture(int X, int Y, bool LINEAR);
	void InitShadowTexture(int X, int Y, bool LINEAR);
	void InitRenderBuffer(int X, int Y, bool LINEAR);

	//unsigned int id;
	//get rid of these pleaz
	std::string type;
	std::string path;
};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER

class Shader
{
public:
	unsigned int ID;
	Shader();
	// constructor generates the shader on the fly
	Shader(const char*, const char*, const char* = nullptr);
	// activate the shader
	// ------------------------------------------------------------------------
	void Use();
	// utility uniform functions
	// ------------------------------------------------------------------------
	void SetBool(const std::string &name, bool value) const;
	// ------------------------------------------------------------------------
	void SetInt(const std::string &name, int value) const;
	// ------------------------------------------------------------------------
	void SetFloat(const std::string &name, float value) const;
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void CheckCompileErrors(GLuint shader, std::string type);
};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MODEL BLEND

namespace graphics
{
	class NewMesh
	{
	public:
		unsigned int index;
		GLuint vao; // Vertex Array Object
		void Draw(unsigned int TEXTURE, unsigned int SHADER);
		void LoadFile(char* FILENAME);
		// add void unload
	private:
		GLuint vbo; // Vertex Buffer Object
		GLuint ebo; // Element Buffer Object
	};
}

class ModelBlend
{
public:
	unsigned int index;
	GLuint vao; // Vertex Array Object
	//void Generate(Mesh& A, Mesh& B); // Generate a new buffer based on two input meshes
	void Draw(unsigned int TEXTURE, unsigned int SHADER);
	//void SaveFile(char* FILENAME);
	void LoadFile(char* FILENAME);
	// add void unload
private:
	GLuint vbo; // Vertex Buffer Object
	GLuint ebo; // Element Buffer Object
};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| GUI

class GUIBitmap
{
public:
	//l2d_texture* t;
	//texture id
	int tid;
	Shader* s;
	unsigned int VBO, VAO, EBO;
	GUIBitmap();
	void SetTexture(Texture TEXTURE);
	void SetShader(Shader* SHADER);
	//missing parameters obviously
	void Draw(int posx, int posy, int WIDTH, int HEIGHT);
};

//new graphics namespace, we want to move our stuff into here!
namespace graphics
{
	void SetFrontFace();
	void SetFrontFaceInverse();

	void ConvertFileTex(char* FILENAME_SRC, char* FILENAME_DST);

	unsigned int TxLoadBTT(const char* fn);
}