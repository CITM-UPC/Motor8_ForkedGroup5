#pragma once
#include <stb_image.h>
#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H
#include "../MyGameEngine/Image.h"
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <memory>
#include <string>
//#include "../Engine/Log.h"

class TextureImporter {
public:
    // Initialize the texture importer (e.g., DevIL library)
    static bool initialize();

    // Load a texture from a file
    static std::unique_ptr<Image> loadTexture(const std::string& filePath);
    void SaveTextureToFile(const std::shared_ptr<Image>& texture, const std::string& filePath);
    std::shared_ptr<Image> LoadTextureFromFile(const std::string& filePath);
};

#endif // TEXTUREIMPORTER_H