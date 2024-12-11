#include "Mesh.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

Mesh::Mesh()
{
	//_meshLoader = new MeshLoader();
}

void Mesh::load(const glm::vec3* vertices, size_t num_verts, unsigned int* indices, size_t num_indexs)
{
	_vertices.assign(vertices, vertices + num_verts);
	_indices.assign(indices, indices + num_indexs);
	_vertexBuffer.loadData(vertices, num_verts * sizeof(glm::vec3));
	_indexBuffer.loadIndices(indices, num_indexs);
	_texCoordsBuffer.unload();
	_normalsBuffer.unload();
	_colorsBuffer.unload();

	_boundingBox.min = _vertices.front();
	_boundingBox.max = _vertices.front();

	for (const auto& v : _vertices) {
		_boundingBox.min = glm::min(_boundingBox.min, glm::dvec3(v));
		_boundingBox.max = glm::max(_boundingBox.max, glm::dvec3(v));
	}
}

void Mesh::loadTexCoords(const glm::vec2* tex_coords, size_t num_tex_coords)
{
	_texCoordsBuffer.loadData(tex_coords, num_tex_coords * sizeof(glm::vec2));
}

void Mesh::loadNormals(const glm::vec3* normals, size_t num_normals)
{
	_normalsBuffer.loadData(normals, num_normals * sizeof(glm::vec3));
}

void Mesh::loadColors(const glm::u8vec3* colors, size_t num_colors)
{
	_colorsBuffer.loadData(colors, num_colors * sizeof(glm::u8vec3));
}

void Mesh::draw() const
{

	if (_idTexture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _idTexture);
	}

	if (_texCoordsBuffer.id())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordsBuffer.bind();
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}

	if (_normalsBuffer.id())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		_normalsBuffer.bind();
		glNormalPointer(GL_FLOAT, 0, nullptr);
	}

	if (_colorsBuffer.id())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		_colorsBuffer.bind();
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	_vertexBuffer.bind();
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	_indexBuffer.bind();
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (_colorsBuffer.id()) glDisableClientState(GL_COLOR_ARRAY);
	if (_normalsBuffer.id()) glDisableClientState(GL_NORMAL_ARRAY);
	if (_texCoordsBuffer.id()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (_idTexture)
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::LoadFile(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) {
		std::vector<glm::vec3> all_vertices;
		std::vector<unsigned int> all_indices;
		std::vector<glm::vec2> all_texCoords;
		std::vector<glm::vec3> all_normals;
		std::vector<glm::u8vec3> all_colors;

		unsigned int vertex_offset = 0;

		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
				all_vertices.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
			}

			for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
				aiFace& face = mesh->mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; k++) {
					all_indices.push_back(face.mIndices[k] + vertex_offset);
				}
			}

			if (mesh->HasTextureCoords(0)) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_texCoords.push_back(glm::vec2(mesh->mTextureCoords[0][j].x, -mesh->mTextureCoords[0][j].y));
				}
			}

			// Copy normals
			if (mesh->HasNormals()) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
				}
			}

			// Copy colors
			if (mesh->HasVertexColors(0)) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_colors.push_back(glm::u8vec3(mesh->mColors[0][j].r * 255, mesh->mColors[0][j].g * 255, mesh->mColors[0][j].b * 255));
				}
			}

			vertex_offset += mesh->mNumVertices;
		}

		// Load the combined mesh data
		load(all_vertices.data(), all_vertices.size(), all_indices.data(), all_indices.size());

		if (!all_texCoords.empty()) {
			loadTexCoords(all_texCoords.data(), all_texCoords.size());
		}

		aiReleaseImport(scene);

	}
	else {
	}
}




void Mesh::drawNormals(const glm::mat4& modelMatrix) {
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	for (size_t i = 0; i < _indices.size(); i += 3) {
		glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i]], 1.0f));
		glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 1]], 1.0f));
		glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 2]], 1.0f));

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
		glm::vec3 center = (v0 + v1 + v2) / 3.0f;

		glVertex3fv(glm::value_ptr(center));
		glVertex3fv(glm::value_ptr(center + normal * 0.2f));
	}
	glEnd();
}


void Mesh::drawNormalsPerFace(const glm::mat4& modelMatrix) {
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);

	for (size_t i = 0; i < _indices.size();) {
		glm::vec3 normal, center;

		if (i + 3 < _indices.size() && (_indices.size() - i) % 4 == 0) { 
			glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i]], 1.0f));
			glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 1]], 1.0f));
			glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 2]], 1.0f));
			glm::vec3 v3 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 3]], 1.0f));

			glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
			glm::vec3 normal2 = glm::normalize(glm::cross(v3 - v0, v2 - v0));
			normal = glm::normalize(normal1 + normal2);

			center = (v0 + v1 + v2 + v3) / 4.0f;
			i += 4; 
		}
		else if (i + 2 < _indices.size()) {
			glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i]], 1.0f));
			glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 1]], 1.0f));
			glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(_vertices[_indices[i + 2]], 1.0f));

			normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			center = (v0 + v1 + v2) / 3.0f;
			i += 3; 
		}

		glVertex3fv(glm::value_ptr(center));
		glVertex3fv(glm::value_ptr(center + normal * 0.2f));
	}

	glEnd();
}
