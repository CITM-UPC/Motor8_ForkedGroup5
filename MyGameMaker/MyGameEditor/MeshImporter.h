#pragma once
#include "MyGameEngine/Mesh.h"
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <string>

class MeshImporter
{
public:
    struct MeshDTO
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        std::vector<glm::u8vec3> colors;
        std::vector<unsigned int> indices;
    };

    MeshDTO ImportMesh(const aiMesh* mesh);

    void SaveMeshToBinaryFile(const MeshDTO& mesh, const std::string& filePath);
    MeshDTO LoadMeshFromBinaryFile(const std::string& filePath);

};

std::ostream& operator<<(std::ostream& os, const MeshImporter::MeshDTO& mesh)
{
    os << mesh.vertices.size() << "\n";
    for (const auto& vertex : mesh.vertices)
    {
        os << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    os << mesh.texCoords.size() << "\n";
    for (const auto& texCoord : mesh.texCoords)
    {
        os << texCoord.x << " " << texCoord.y << "\n";
    }

    os << mesh.normals.size() << "\n";
    for (const auto& normal : mesh.normals)
    {
        os << normal.x << " " << normal.y << " " << normal.z << "\n";
    }

    os << mesh.colors.size() << "\n";
    for (const auto& color : mesh.colors)
    {
        os << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " " << static_cast<int>(color.b) << "\n";
    }

    os << mesh.indices.size() << "\n";
    for (const auto& index : mesh.indices)
    {
        os << index << "\n";
    }

    return os;
}

std::istream& operator>>(std::istream& is, MeshImporter::MeshDTO& mesh)
{
    size_t size;

    is >> size;
    mesh.vertices.resize(size);
    for (auto& vertex : mesh.vertices)
    {
        is >> vertex.x >> vertex.y >> vertex.z;
    }

    is >> size;
    mesh.texCoords.resize(size);
    for (auto& texCoord : mesh.texCoords)
    {
        is >> texCoord.x >> texCoord.y;
    }

    is >> size;
    mesh.normals.resize(size);
    for (auto& normal : mesh.normals)
    {
        is >> normal.x >> normal.y >> normal.z;
    }

    is >> size;
    mesh.colors.resize(size);
    for (auto& color : mesh.colors)
    {
        int r, g, b;
        is >> r >> g >> b;
        color = glm::u8vec3(r, g, b);
    }

    is >> size;
    mesh.indices.resize(size);
    for (auto& index : mesh.indices)
    {
        is >> index;
    }

    return is;
}


