#pragma once
#include <vector>
#include <string>

class TextureImporter {
public:
    struct TextureDTO {
        unsigned int width;                    // Ancho de la textura
        unsigned int height;                   // Alto de la textura
        unsigned int channels;                 // Número de canales (e.g., RGB = 3, RGBA = 4)
        std::vector<unsigned char> data;       // Datos de píxeles en bruto (raw data)
    };

    void SaveTextureToBinaryFile(const TextureDTO& texture, const std::string& filePath);
    TextureDTO LoadTextureFromBinaryFile(const std::string& filePath);
};

