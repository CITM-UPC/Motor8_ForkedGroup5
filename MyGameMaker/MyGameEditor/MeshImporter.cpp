#include "MeshImporter.h"

MeshImporter::MeshDTO MeshImporter::ImportMesh(const aiMesh* mesh)
{
    MeshDTO meshDTO;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        meshDTO.vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        meshDTO.normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        if (mesh->mTextureCoords[0])
        {
            meshDTO.texCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }
        else
        {
            meshDTO.texCoords.push_back(glm::vec2(0.0f, 0.0f));
        }
        if (mesh->mColors[0])
        {
            meshDTO.colors.push_back(glm::u8vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b));
        }
        else
        {
            meshDTO.colors.push_back(glm::u8vec3(255, 255, 255));
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            meshDTO.indices.push_back(face.mIndices[j]);
        }
    }

    return meshDTO;
}

void MeshImporter::SaveMeshToFile(const MeshDTO& mesh, const std::string& filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    file << mesh;
    file.close();
}

MeshImporter::MeshDTO MeshImporter::LoadMeshFromFile(const std::string& filePath)
{
    MeshDTO mesh;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    file >> mesh;
    file.close();
    return mesh;
}