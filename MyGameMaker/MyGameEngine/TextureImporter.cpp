#include "TextureImporter.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

void TextureImporter::SaveTextureToBinaryFile(const TextureDTO& texture, const std::string& filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    const char* magic = "MYTEXT";
    file.write(magic, 6); // Identificador del archivo

    file.write(reinterpret_cast<const char*>(&texture.width), sizeof(texture.width));
    file.write(reinterpret_cast<const char*>(&texture.height), sizeof(texture.height));
    file.write(reinterpret_cast<const char*>(&texture.channels), sizeof(texture.channels));

    unsigned int dataSize = static_cast<unsigned int>(texture.data.size());
    file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    file.write(reinterpret_cast<const char*>(texture.data.data()), dataSize);

    file.close();
}

TextureImporter::TextureDTO TextureImporter::LoadTextureFromBinaryFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    TextureDTO texture;

    char magic[6];
    file.read(magic, 6);
    if (std::string(magic, 6) != "MYTEXT") {
        throw std::runtime_error("Invalid file format: " + filePath);
    }

    file.read(reinterpret_cast<char*>(&texture.width), sizeof(texture.width));
    file.read(reinterpret_cast<char*>(&texture.height), sizeof(texture.height));
    file.read(reinterpret_cast<char*>(&texture.channels), sizeof(texture.channels));

    unsigned int dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
    texture.data.resize(dataSize);
    file.read(reinterpret_cast<char*>(texture.data.data()), dataSize);

    file.close();
    return texture;
}
