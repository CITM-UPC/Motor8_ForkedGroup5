#include "ModelImporter.h"
#include <filesystem>
#include <fstream>

std::shared_ptr<Mesh> ModelImporter::ImportModel(const char* filePath)
{
    auto mesh = std::make_shared<Mesh>();
    const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

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

            if (mesh->HasNormals()) {
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    all_normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
                }
            }

            if (mesh->HasVertexColors(0)) {
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    all_colors.push_back(glm::u8vec3(mesh->mColors[0][j].r * 255, mesh->mColors[0][j].g * 255, mesh->mColors[0][j].b * 255));
                }
            }

            vertex_offset += mesh->mNumVertices;
        }

        mesh->load(all_vertices.data(), all_vertices.size(), all_indices.data(), all_indices.size());

        if (!all_texCoords.empty()) {
            mesh->loadTexCoords(all_texCoords.data(), all_texCoords.size());
        }

        if (!all_normals.empty()) {
            mesh->loadNormals(all_normals.data(), all_normals.size());
        }

        if (!all_colors.empty()) {
            mesh->loadColors(all_colors.data(), all_colors.size());
        }

        aiReleaseImport(scene);
    }
    return mesh;
}

void ModelImporter::SaveMeshToFile(const std::shared_ptr<Mesh>& mesh, const std::string& filePath)
{
    std::filesystem::path path(filePath);

    if (!std::filesystem::exists(path.parent_path())) {
        throw std::runtime_error("Directory does not exist: " + path.parent_path().string());
    }

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    if (!mesh) {
        return;
    }

    const auto& vertices = mesh->vertices();
    size_t verticesSize = vertices.size();
    outFile.write(reinterpret_cast<const char*>(&verticesSize), sizeof(verticesSize));
    outFile.write(reinterpret_cast<const char*>(vertices.data()), verticesSize * sizeof(glm::vec3));

    const auto& indices = mesh->indices();
    size_t indicesSize = indices.size();
    outFile.write(reinterpret_cast<const char*>(&indicesSize), sizeof(indicesSize));
    outFile.write(reinterpret_cast<const char*>(indices.data()), indicesSize * sizeof(unsigned int));

    const auto& texCoords = mesh->texCoords();
    size_t texCoordsSize = texCoords.size();
    outFile.write(reinterpret_cast<const char*>(&texCoordsSize), sizeof(texCoordsSize));
    outFile.write(reinterpret_cast<const char*>(texCoords.data()), texCoordsSize * sizeof(glm::vec2));

    const auto& boundingBox = mesh->boundingBox();
    outFile.write(reinterpret_cast<const char*>(&boundingBox), sizeof(BoundingBox));

    outFile.close();
}

std::shared_ptr<Mesh> ModelImporter::LoadModelFromFile(const std::string& filePath)
{
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    size_t verticesSize;
    inFile.read(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));
    std::vector<glm::vec3> vertices(verticesSize);
    inFile.read(reinterpret_cast<char*>(vertices.data()), verticesSize * sizeof(glm::vec3));

    size_t indicesSize;
    inFile.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
    std::vector<unsigned int> indices(indicesSize);
    inFile.read(reinterpret_cast<char*>(indices.data()), indicesSize * sizeof(unsigned int));

    size_t texCoordsSize;
    inFile.read(reinterpret_cast<char*>(&texCoordsSize), sizeof(texCoordsSize));
    std::vector<glm::vec2> texCoords(texCoordsSize);
    inFile.read(reinterpret_cast<char*>(texCoords.data()), texCoordsSize * sizeof(glm::vec2));

    BoundingBox boundingBox;
    inFile.read(reinterpret_cast<char*>(&boundingBox), sizeof(BoundingBox));

    mesh->load(vertices.data(), vertices.size(), indices.data(), indices.size());
    if (!texCoords.empty()) {
        mesh->loadTexCoords(texCoords.data(), texCoords.size());
    }

    return mesh;
}