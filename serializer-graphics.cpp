#include "serializer-graphics.h"

#include "tga_reader.h"

#include <fstream>
#include <vector>
#include <iostream>

//assimp stuff
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags

extern TGA_ORDER *TGA_READER_ARGB;
extern TGA_ORDER *TGA_READER_ABGR;

namespace serializer_graphics
{
	#define FILE_VERSION_MB 0x0u
	#define FILE_VERSION_M 0x0u
	#define FILE_VERSION_TEX 0x0u

	typedef btui16 version_t;

	struct color
	{
		btui8 r = '\0';
		btui8 g = '\0';
		btui8 b = '\0';
		btui8 a = '\0';
	};

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- DECLARATIONS ---------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MESH

	class Mesh {
	public:
		std::vector<vert> vertices;
		std::vector<unsigned int> indices;
		Mesh(std::vector<vert> vertices, std::vector<unsigned int> indices);
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MODEL

	class Model
	{
	public:
		//model data
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;

		Model();
		Model(std::string const &path);

	private:
		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(std::string const &path);

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode *node, const aiScene *scene);
		//i dont know
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	};

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- FUNCTION DEFINITIONS -------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MESH

	// constructor
	Mesh::Mesh(const std::vector<vert> vertices, const std::vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MODEL

	Model::Model()
	{
	}

	Model::Model(std::string const &path)
	{
		loadModel(path);
	}

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void Model::loadModel(std::string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Model::processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		std::vector<vert> vertices;
		std::vector<unsigned int> indices;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vert vertex;
			vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = -mesh->mVertices[i].x; // X handedness flip
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.pos = vector;
			// normals
			vector.x = -mesh->mNormals[i].x; // X handedness flip
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.nor = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.uvc = vec;
			}
			if (mesh->mColors[0]) // does the mesh contain texture coordinates?
			{
				vec4 vec4;
				vec4.x = mesh->mColors[0][i].r;
				vec4.y = mesh->mColors[0][i].g;
				vec4.z = mesh->mColors[0][i].b;
				vec4.w = mesh->mColors[0][i].a;
				vertex.col = vec4;
			}
			else
			{
				vertex.col.x = 1.f;
				vertex.col.y = 1.f;
				vertex.col.z = 1.f;
				vertex.col.w = 1.f;
			}

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices);
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| CONVERSION FUNCTIONS

	//add dds loader from this address
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/

	void ConvertTex(char* sfn, char* dfn, btui8 filter, btui8 edge)
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
			btui16 width = (btui16)tgaGetWidth(buffer);
			btui16 height = (btui16)tgaGetHeight(buffer);

			// copy into new format?

			int i = sizeof(color) * width * height;
			color* carr = (color*)malloc(i);
			//memcpy(&carr[0], (void*)pixels, i);
			memcpy(carr, (void*)pixels, i);

			//std::cout << "Loaded texture: " << sfn << std::endl;

			FILE* out = fopen(dfn, "wb");
			if (out != NULL)
			{
				// Seek the beginning of the file
				fseek(out, 0, SEEK_SET);
				// Write version
				version_t v = FILE_VERSION_TEX;
				fwrite(&v, sizeof(version_t), 1, out);
				fwrite(&filter, 1, 1, out);
				fwrite(&edge, 1, 1, out);
				// Write dimensions
				fwrite(&width, sizeof(btui16), 1, out); // Max value: 65535
				fwrite(&height, sizeof(btui16), 1, out);
				// Write pixel buffer
				//fwrite(&carr[0], sizeof(color), width * height, out);
				fwrite(carr, sizeof(color), width * height, out);
				fclose(out);
			}
			else
			{
				std::cout << errno << std::endl;
				std::cout << "Failed to convert Texture: " << dfn << std::endl;
			}

			free(carr);
			tgaFree(pixels);
		}
	}

	void ConvertMesh(char* sfn, char* dfn)
	{
		Model ma = Model(sfn);
		Mesh a = ma.meshes[0];

		std::vector<vert> vces; // Vertices
		std::vector<btui32> ices; // Indices

		// For each vertex
		for (int i = 0; i < a.vertices.size(); i++)
		{
			vces.push_back(vert());
			vces[i].pos = a.vertices[i].pos;
			vces[i].nor = a.vertices[i].nor;
			vces[i].uvc = a.vertices[i].uvc;
			vces[i].col = a.vertices[i].col;
		}
		// Copy indices straight from mesh A, as they should be identical in each mesh
		ices = a.indices;

		FILE* out = fopen(dfn, "wb");
		if (out != NULL)
		{
			// Seek the beginning of the file
			fseek(out, 0, SEEK_SET);
			// Write version
			version_t v = FILE_VERSION_MB;
			fwrite(&v, sizeof(version_t), 1, out);
			// Write vertices
			size_t i = vces.size(); // Get number of vertices
			fwrite(&i, sizeof(size_t), 1, out);
			fwrite(&vces[0], sizeof(vert), vces.size(), out);
			// Write indices
			i = ices.size(); // Get number of indices
			fwrite(&i, sizeof(size_t), 1, out);
			fwrite(&ices[0], sizeof(btui32), ices.size(), out);
			fclose(out);
		}
	}
	void ConvertMB(char* sfn_a, char* sfn_b, char* dfn)
	{
		Model ma = Model(sfn_a);
		Model mb = Model(sfn_b);
		Mesh a = ma.meshes[0];
		Mesh b = mb.meshes[0];

		std::vector<vert_blend> vces; // Vertices
		std::vector<unsigned int> ices; // Indices

		// Progress if mesh A and B have same number of verts
		if (a.vertices.size() == b.vertices.size())
		{
			// For each vertex
			for (int i = 0; i < a.vertices.size(); i++)
			{
				vces.push_back(vert_blend());
				vces[i].pos_a = a.vertices[i].pos;
				vces[i].pos_b = b.vertices[i].pos;
				vces[i].nor_a = a.vertices[i].nor;
				vces[i].nor_b = b.vertices[i].nor;
				vces[i].uvc = a.vertices[i].uvc;
			}
			// Copy indices straight from mesh A, as they should be identical in each mesh
			ices = a.indices;

			FILE* out = fopen(dfn, "wb");
			if (out != NULL)
			{
				// Seek the beginning of the file
				fseek(out, 0, SEEK_SET);
				// Write version
				version_t v = FILE_VERSION_MB;
				fwrite(&v, sizeof(version_t), 1, out);
				// Write vertices
				size_t i = vces.size(); // Get number of vertices
				fwrite(&i, sizeof(size_t), 1, out);
				fwrite(&vces[0], sizeof(vert_blend), vces.size(), out);
				// Write indices
				i = ices.size(); // Get number of indices
				fwrite(&i, sizeof(size_t), 1, out);
				fwrite(&ices[0], sizeof(unsigned int), ices.size(), out);
				fclose(out);
			}
		}
		else
		{
			std::cout << "Could not generate Model Blend, number of vertices not consistent!" << std::endl;
		}
	}
}