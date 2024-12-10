#include "MeshImporter.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

void MeshImporter::SaveMeshToBinaryFile(const MeshDTO& mesh, const std::string& filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    const char* magic = "MYMESH";
    file.write(magic, 6);

    unsigned int vertexCount = static_cast<unsigned int>(mesh.vertices.size());
    file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
    file.write(reinterpret_cast<const char*>(mesh.vertices.data()), vertexCount * sizeof(glm::vec3));

    unsigned int texCoordCount = static_cast<unsigned int>(mesh.texCoords.size());
    file.write(reinterpret_cast<const char*>(&texCoordCount), sizeof(texCoordCount));
    file.write(reinterpret_cast<const char*>(mesh.texCoords.data()), texCoordCount * sizeof(glm::vec2));

    unsigned int normalCount = static_cast<unsigned int>(mesh.normals.size());
    file.write(reinterpret_cast<const char*>(&normalCount), sizeof(normalCount));
    file.write(reinterpret_cast<const char*>(mesh.normals.data()), normalCount * sizeof(glm::vec3));

    unsigned int colorCount = static_cast<unsigned int>(mesh.colors.size());
    file.write(reinterpret_cast<const char*>(&colorCount), sizeof(colorCount));
    file.write(reinterpret_cast<const char*>(mesh.colors.data()), colorCount * sizeof(glm::u8vec3));

    unsigned int indexCount = static_cast<unsigned int>(mesh.indices.size());
    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
    file.write(reinterpret_cast<const char*>(mesh.indices.data()), indexCount * sizeof(unsigned int));

    file.close();
}

MeshImporter::MeshDTO MeshImporter::LoadMeshFromBinaryFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    MeshDTO mesh;
    char magic[6];
    file.read(magic, 6);
    if (std::string(magic, 6) != "MYMESH") {
        throw std::runtime_error("Invalid file format: " + filePath);
    }

    unsigned int vertexCount;
    file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    mesh.vertices.resize(vertexCount);
    file.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexCount * sizeof(glm::vec3));

    unsigned int texCoordCount;
    file.read(reinterpret_cast<char*>(&texCoordCount), sizeof(texCoordCount));
    mesh.texCoords.resize(texCoordCount);
    file.read(reinterpret_cast<char*>(mesh.texCoords.data()), texCoordCount * sizeof(glm::vec2));

    unsigned int normalCount;
    file.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount));
    mesh.normals.resize(normalCount);
    file.read(reinterpret_cast<char*>(mesh.normals.data()), normalCount * sizeof(glm::vec3));

    unsigned int colorCount;
    file.read(reinterpret_cast<char*>(&colorCount), sizeof(colorCount));
    mesh.colors.resize(colorCount);
    file.read(reinterpret_cast<char*>(mesh.colors.data()), colorCount * sizeof(glm::u8vec3));

    unsigned int indexCount;
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
    mesh.indices.resize(indexCount);
    file.read(reinterpret_cast<char*>(mesh.indices.data()), indexCount * sizeof(unsigned int));

    file.close();
    return mesh;
}
