#include "TextureImporter.h"
#include <fstream>
#include <stdexcept>

void TextureImporter::SaveTextureToBinaryFile(const TextureDTO& texture, const std::string& filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Unable to open file for writing: " + filePath);
    }

    // Escribir dimensiones y datos
    outFile.write(reinterpret_cast<const char*>(&texture.width), sizeof(texture.width));
    outFile.write(reinterpret_cast<const char*>(&texture.height), sizeof(texture.height));
    outFile.write(reinterpret_cast<const char*>(&texture.channels), sizeof(texture.channels));
    outFile.write(reinterpret_cast<const char*>(texture.data.data()), texture.data.size());
}


TextureImporter::TextureDTO TextureImporter::LoadTextureFromBinaryFile(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Unable to open file for reading: " + filePath);
    }

    TextureDTO texture;
    inFile.read(reinterpret_cast<char*>(&texture.width), sizeof(texture.width));
    inFile.read(reinterpret_cast<char*>(&texture.height), sizeof(texture.height));
    inFile.read(reinterpret_cast<char*>(&texture.channels), sizeof(texture.channels));

    size_t dataSize = texture.width * texture.height * texture.channels;
    texture.data.resize(dataSize);
    inFile.read(reinterpret_cast<char*>(texture.data.data()), dataSize);

    return texture;
}