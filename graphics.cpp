//TOP INCLUDE
#include "graphics.hpp"

//would like to drop this include
#include <glm\gtc\matrix_transform.hpp>
//i dont know what the point of using this one is
#include <glm\gtc\type_ptr.hpp>

//#include "global.h"
#include "cfg.h"

extern TGA_ORDER *TGA_READER_ARGB;
extern TGA_ORDER *TGA_READER_ABGR;

namespace graphics
{
	glm::mat4 mat_proj;
	glm::mat4 mat_view;

	glm::mat4 GetMatProj()
	{
		return mat_proj;
	}
	void SetMatProj(float ofx, float ofy, float ofh, float yaw, float pitch)
	{
		//float aspect = cfg::iWinY / (cfg::iWinX * 0.5f);
		//float scale = ((cfg::iWinX * 0.5f) / TILE_RENDER_SIZE_X) * 0.5f;
		
		mat_proj = glm::mat4(1.0f);
		//mat_proj = glm::scale(mat_proj, glm::vec3(-(2.f / (cfg::iWinX / 2)), 0.25f, (2.f / cfg::iWinY)));
		mat_proj = glm::scale(mat_proj, glm::vec3(-(2.f / (cfg::iWinX / 2)), 0.25f, (2.f / cfg::iWinY)));

		//y = vertical scale (1 is near clip)
		mat_proj = glm::scale(mat_proj, glm::vec3(-64, 1.f, -64)); // middle value seems to represent clip range

		mat_proj = glm::translate(mat_proj, glm::vec3(0.f, 0.f, 4.f));

		//real 3d version
		mat_proj = glm::rotate(mat_proj, pitch, glm::vec3(1, 0, 0));
		mat_proj = glm::rotate(mat_proj, yaw, glm::vec3(0, 1, 0));

		mat_proj = glm::translate(mat_proj, glm::vec3(ofx, -ofh, -ofy));
	}

	glm::mat4 GetMatView()
	{
		return mat_view;
	}
	void SetMatView(float ofx, float ofy, float pitch, float yaw)
	{
		mat_view = glm::mat4(1.0f);

		/*mat_view = glm::rotate(mat_view, -pitch, glm::vec3(1, 0, 0));
		mat_view = glm::rotate(mat_view, yaw, glm::vec3(0, 1, 0));*/

		//mat_view = glm::translate(mat_view, glm::vec3(ofx, -1.f, -ofy));


	}

	void SetFrontFace()
	{
		glFrontFace(GL_CW);
	}
	void SetFrontFaceInverse()
	{
		glFrontFace(GL_CCW);
	}
}

void Texture::InitRenderTexture(int x, int y, bool linear)
{
	width = x;
	height = y;
	
	// The texture we're going to render to
	//unsigned int rendertexture;
	glGenTextures(1, &id);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	#ifdef DEF_MULTISAMPLE
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id); // "Bind" the newly created texture : all future texture functions will modify this texture
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, x, y, GL_TRUE); //create a blank image
	#else
	glBindTexture(GL_TEXTURE_2D, id); // "Bind" the newly created texture : all future texture functions will modify this texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //create a blank image
	#endif
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
	#ifdef DEF_MULTISAMPLE
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, id, 0);
	#else
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
	#endif
}
void Texture::InitDepthTexture(int x, int y, bool linear)
{
	width = x;
	height = y;

	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
}
void Texture::InitShadowTexture(int x, int y, bool linear)
{
	width = x;
	height = y;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
	
	//We only need the depth information when rendering the scene from the light's perspective
	//so there is no need for a color buffer. A framebuffer object however is not complete without
	//a color buffer so we need to explicitly tell OpenGL we're not going to render any color data.
	//We do this by setting both the read and draw buffer to GL_NONE with glDrawBuffer and glReadbuffer.
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}
void Texture::InitRenderBuffer(int x, int y, bool linear)
{
	width = x;
	height = y;

	//render buffer object (depth)
	//unsigned int depthbuffer;
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	#ifdef DEF_MULTISAMPLE
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, x, y);
	#else
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);
	#endif

	//Once we've allocated enough memory for the renderbuffer object we can unbind the renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//bind rbo to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, id);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER

Shader::Shader()
{
}

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
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
	int success;
	char infoLog[512];
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

}
// activate the shader
// ------------------------------------------------------------------------
void Shader::Use()
{
	glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| TGA LOADER

unsigned int TxLoadTGA(const char *path, const std::string &directory, bool gamma)
{
	//uh
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	//generate opengl texture id
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//open file
	//tga reader version
	FILE *file = fopen(filename.c_str(), "rb");
	if (file) {
		int size;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char *buffer = (unsigned char *)tgaMalloc(size);
		fread(buffer, 1, size, file);
		fclose(file);

		int *pixels = tgaRead(buffer, TGA_READER_ABGR);
		//int *pixels = tgaRead(buffer, TGA_READER_ARGB);
		int width = tgaGetWidth(buffer);
		int height = tgaGetHeight(buffer);

		//assign ogl texture
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
		//glGenerateMipmap(GL_TEXTURE_2D);

		//glGenTextures(1, &id);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifdef DEF_OLDSKOOL
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//generate mips for this texture
		//glGenerateMipmap(GL_TEXTURE_2D);
#endif // DEF_OLDSKOOL
#ifndef DEF_OLDSKOOL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//generate mips for this texture
		glGenerateMipmap(GL_TEXTURE_2D);
#endif // !DEF_OLDSKOOL

		std::cout << "Loaded texture: " << path << std::endl;

		tgaFree(pixels);
	}
	return textureID;
}

unsigned int TxLoadTGA(const char * path, bool gamma)
{
	//uh
	//std::string filename = std::string(path);
	//filename = directory + '/' + filename;
	//generate opengl texture id
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//open file
	//tga reader version
	FILE *file = fopen(path, "rb");
	if (file) {
		int size;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char *buffer = (unsigned char *)tgaMalloc(size);
		fread(buffer, 1, size, file);
		fclose(file);

		int *pixels = tgaRead(buffer, TGA_READER_ABGR);
		//int *pixels = tgaRead(buffer, TGA_READER_ARGB);
		int width = tgaGetWidth(buffer);
		int height = tgaGetHeight(buffer);

		//assign ogl texture
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
		//glGenerateMipmap(GL_TEXTURE_2D);

		//glGenTextures(1, &id);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		#ifdef DEF_OLDSKOOL
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//generate mips for this texture
		//glGenerateMipmap(GL_TEXTURE_2D);
		#endif
		#ifndef DEF_OLDSKOOL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//generate mips for this texture
		glGenerateMipmap(GL_TEXTURE_2D);
		#endif

		std::cout << "Loaded texture: " << path << std::endl;

		tgaFree(pixels);
	}
	return textureID;
}

//add dds loader from this address
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/

/*
GLuint TxLoadDDS(const char * imagepath) {
unsigned char header[124];

FILE *fp;

// try to open the file
fp = fopen(imagepath, "rb");
if (fp == NULL)
return 0;

// verify the type of file
char filecode[4];
fread(filecode, 1, 4, fp);
if (strncmp(filecode, "DDS ", 4) != 0) {
fclose(fp);
return 0;
}

// get the surface desc
fread(&header, 124, 1, fp);

unsigned int height = *(unsigned int*)&(header[8]);
unsigned int width = *(unsigned int*)&(header[12]);
unsigned int linearSize = *(unsigned int*)&(header[16]);
unsigned int mipMapCount = *(unsigned int*)&(header[24]);
unsigned int fourCC = *(unsigned int*)&(header[80]);

unsigned char * buffer;
unsigned int bufsize;
// how big is it going to be including all mipmaps?
bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
fread(buffer, 1, bufsize, fp);
// close the file pointer
fclose(fp);

unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
unsigned int format;
switch (fourCC)
{
case FOURCC_DXT1:
format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
break;
case FOURCC_DXT3:
format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
break;
case FOURCC_DXT5:
format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
break;
default:
free(buffer);
return 0;
}

// Create one OpenGL texture
GLuint textureID;
glGenTextures(1, &textureID);

// "Bind" the newly created texture : all future texture functions will modify this texture
glBindTexture(GL_TEXTURE_2D, textureID);

unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
unsigned int offset = 0;

// load the mipmaps
for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
{
unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
0, size, buffer + offset);

offset += size;
width /= 2;
height /= 2;
}
free(buffer);

return textureID;
}
*/

GUIBitmap::GUIBitmap()
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
void GUIBitmap::SetTexture(Texture tex)
{
	tid = tex.id;
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

	transform = glm::scale(transform, glm::vec3(2.f / cfg::iWinX, 2.f / cfg::iWinY, 1.0f));
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

#define FILE_VERSION_MB 0x0u
#define FILE_VERSION_M 0x0u
#define FILE_VERSION_TEX 0x0u

typedef unsigned short version_t;

namespace graphics
{
	void NewMesh::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}

	void NewMesh::LoadFile(char* fn)
	{
		std::cout << "Loading " << fn << std::endl;

		//....................................... OPEN FILE
		
		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			std::vector<vert_blend> vces; // Vertices
			std::vector<unsigned int> ices; // Indices

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			 //....................................... READ VERTICES

			size_t i;
			fread(&i, sizeof(size_t), 1, in); // Read number of vertices
			vces.resize(i); // Resize vector to accommodate new size
			fread(&vces[0], sizeof(vert), i, in); // Read vertices

			//....................................... READ INDICES

			fread(&i, sizeof(size_t), 1, in); // Read number of indices
			ices.resize(i); // Resize vector to accommodate new size
			fread(&ices[0], sizeof(unsigned int), i, in); // Read indices

			fclose(in);

			index = ices.size(); // Set number of indices, needed to know how many faces to draw

			//....................................... INITIALIZE OPENGL BUFFER
			 
			// create buffers/arrays
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo);

			glBindVertexArray(vao); // Bind vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // load data into vertex buffers
			// pass vertex struct to opengl
			glBufferData(GL_ARRAY_BUFFER, vces.size() * sizeof(vert), &vces[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices.size() * sizeof(unsigned int), &ices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(vns::vi_pos); // Set Vertex positions
			glVertexAttribPointer(vns::vi_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_pos);
			glEnableVertexAttribArray(vns::vi_nor); // Set Vertex normals
			glVertexAttribPointer(vns::vi_nor, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_nor);
			glEnableVertexAttribArray(vns::vi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(vns::vi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_uvc);
			glEnableVertexAttribArray(vns::vi_col); // Set Vertex colour
			glVertexAttribPointer(vns::vi_col, 4, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_col);
			glBindVertexArray(0); // Bind default vertex array
			std::cout << "Generated model!" << std::endl;
		}
	}
}

void ModelBlend::Draw(unsigned int tex, unsigned int shd)
{
	glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
	glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
	// and finally bind the texture
	glBindTexture(GL_TEXTURE_2D, tex);

	// draw mesh
	glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, ices.size(), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void ModelBlend::LoadFile(char* fn)
{
	std::cout << "Loading " << fn << std::endl;
	FILE* in = fopen(fn, "rb");
	if (in != NULL)
	{
		std::vector<vert_blend> vces; // Vertices
		std::vector<unsigned int> ices; // Indices

		// Seek the beginning of the file
		fseek(in, 0, SEEK_SET);
		// Read version
		version_t v;
		fread(&v, sizeof(version_t), 1, in);
		// Write vertices
		size_t i;
		fread(&i, sizeof(size_t), 1, in); // Read number of vertices
		vces.resize(i); // Resize vector to accommodate new size
		fread(&vces[0], sizeof(vert_blend), i, in); // Read vertices

		// Write indices
		fread(&i, sizeof(size_t), 1, in); // Read number of indices
		ices.resize(i); // Resize vector to accommodate new size
		fread(&ices[0], sizeof(unsigned int), i, in); // Read indices

		//fwrite(&ices[0], sizeof(unsigned int), ices.size(), out);
		fclose(in);

		//set this for later
		index = ices.size();

		//initialize 
		// create buffers/arrays
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vces.size() * sizeof(vert_blend), &vces[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices.size() * sizeof(unsigned int), &ices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)offsetof(vert_blend, pos_b));
		// vertex normals
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)offsetof(vert_blend, nor_a));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)offsetof(vert_blend, nor_b));
		// vertex texture coords
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)offsetof(vert_blend, uvc));

		glBindVertexArray(0);

		std::cout << "Generated model blend!" << std::endl;
	}
}


namespace graphics
{
	struct color
	{
		btui8 r = '\0';
		btui8 g = '\0';
		btui8 b = '\0';
		btui8 a = '\0';
	};
}

unsigned int graphics::TxLoadBTT(const char* fn)
{
	//uh
	//std::string filename = std::string(path);
	//filename = directory + '/' + filename;
	//generate opengl texture id
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//int *pixels;
	int width;
	int height;

	//open file
	FILE *file = fopen(fn, "rb");
	if (file)
	{
		// Seek the beginning of the file
		fseek(file, 0, SEEK_SET);
		// Write version
		version_t v;
		fread(&v, sizeof(version_t), 1, file);
		// Write dimensions
		fread(&width, sizeof(int), 1, file);
		fread(&height, sizeof(int), 1, file);
		// Write pixel buffer
		//color ctest[4];
		//fread(&ctest[0], sizeof(color), 4, file);

		color* carr = (color*)malloc(sizeof(color) * width * height);
		//memcpy(&carr[0], (void*)pixels, i);
		//fread(&carr[0], sizeof(color), width * height, file);
		fread(carr, sizeof(color), width * height, file);

		fclose(file);


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ctest);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, carr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		#ifdef DEF_OLDSKOOL
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//generate mips for this texture
		//glGenerateMipmap(GL_TEXTURE_2D);
		#endif // DEF_OLDSKOOL
		#ifndef DEF_OLDSKOOL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//generate mips for this texture
		glGenerateMipmap(GL_TEXTURE_2D);
		#endif // !DEF_OLDSKOOL

		free(carr);

		return textureID;
	}
	else
	{
		return 0u;
	}

}

void graphics::ConvertFileTex(char* sfn, char* dfn)
{
	//open file
	//tga reader version
	FILE *in = fopen(sfn, "rb");
	if (in) {
		int size;
		fseek(in, 0, SEEK_END);
		size = ftell(in);
		fseek(in, 0, SEEK_SET);

		unsigned char *buffer = (unsigned char *)tgaMalloc(size);
		fread(buffer, 1, size, in);
		fclose(in);

		int *pixels = tgaRead(buffer, TGA_READER_ABGR);
		unsigned int width = tgaGetWidth(buffer);
		unsigned int height = tgaGetHeight(buffer);

		int i = sizeof(color) * width * height;
		color* carr = (color*)malloc(i);
		//memcpy(&carr[0], (void*)pixels, i);
		memcpy(carr, (void*)pixels, i);

		std::cout << "Loaded texture: " << sfn << std::endl;

		FILE* out = fopen(dfn, "wb");
		if (out != NULL)
		{
			// Seek the beginning of the file
			fseek(out, 0, SEEK_SET);
			// Write version
			version_t v = FILE_VERSION_TEX;
			fwrite(&v, sizeof(version_t), 1, out);
			// Write dimensions
			fwrite(&width, sizeof(int), 1, out); // Max value: 65535
			fwrite(&height, sizeof(int), 1, out);
			// Write pixel buffer
			//fwrite(&carr[0], sizeof(color), width * height, out);
			fwrite(carr, sizeof(color), width * height, out);
			fclose(out);

			std::cout << "Converted Texture: " << dfn << std::endl;
		}
		else
		{
			std::cout << errno << std::endl;
		}

		free(carr);
		tgaFree(pixels);
	}
}