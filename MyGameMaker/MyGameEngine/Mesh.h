#pragma once
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>
#include <glm/glm.hpp>
#include "BufferObject.h"
#include "BoundingBox.h"
#include "../MyGameEditor/MeshImporter.h"



class Mesh
{
public:
private:
	std::vector<glm::vec3> _vertices;       // Vértices de la malla
	std::vector<glm::vec2> _texCoords;      // Coordenadas de textura
	std::vector<glm::vec3> _normals;        // Normales de la malla
	std::vector<unsigned int> _indices;     // Índices para dibujo con elementos

	BufferObject _vertexBuffer;
	BufferObject _indexBuffer;
	BufferObject _texCoordsBuffer;
	BufferObject _normalsBuffer;
	BufferObject _colorsBuffer;

public:
	Mesh() = default;  // Constructor por defecto

	// Constructor que usa un MeshDTO
	Mesh(const MeshImporter::MeshDTO& dto);

private:
	unsigned int _idTexture = 0;

	BoundingBox _boundingBox;

	//MeshLoader* _meshLoader;

public:

	const auto& vertices() const { return _vertices; }
	const auto& textCoords() const { return _texCoords; }
	const auto& normals() const { return _normals; }
	const auto& indices() const { return _indices; }
	const auto& boundingBox() const { return _boundingBox; }
	//const auto& meshLoader() const { return *_meshLoader; }

	void load(const glm::vec3* vertices, size_t num_verts, unsigned int* indices, size_t num_indexs);
	void loadTexCoords(const glm::vec2* tex_coords, size_t num_tex_coords);
	void loadNormals(const glm::vec3* normals, size_t num_normals);
	void loadColors(const glm::u8vec3* colors, size_t num_colors);
	void draw() const;

	void LoadFile(const char* filePath);

	// Load Texture
	void drawNormals(const glm::mat4& modelMatrix);
	void drawNormalsPerFace(const glm::mat4& modelMatrix);	
};