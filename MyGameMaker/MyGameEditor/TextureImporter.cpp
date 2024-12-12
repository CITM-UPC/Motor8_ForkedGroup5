#include "TextureImporter.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "MyGameEngine/Texture.h"
#include <memory>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "MyGameEngine/Image.h"


static GLenum formatFromChannels(int channels) {
    switch (channels) {
    case 1: return GL_LUMINANCE;
    case 2: return GL_LUMINANCE_ALPHA;
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    default: return GL_RGB;
    }
}
bool TextureImporter::initialize() {
    ilInit();
    iluInit();
    ILenum err = ilGetError();
    if (err != IL_NO_ERROR) {
       // std::cerr << "DevIL initialization failed: " << iluErrorString(err) << std::endl;
        return false;
    }
    return true;
}

std::unique_ptr<Image> TextureImporter::loadTexture(const std::string& filePath) {
    // Convert std::string (filePath) to std::wstring
    size_t size = filePath.size() + 1; // Include null terminator
    std::vector<wchar_t> wFilePath(size);
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wFilePath.data(), size, filePath.c_str(), size - 1);

    // DevIL image ID
    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);

    // Load the image using wchar_t* (wide character string)
    if (!ilLoadImage((const wchar_t*)filePath.c_str())) {
        ILenum err = ilGetError();
        // std::cerr << "Failed to load image: " << iluErrorString(err) << std::endl;
        ilDeleteImages(1, &imgId);
        return nullptr;
    }

    // Get image properties
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    int channels = ilGetInteger(IL_IMAGE_CHANNELS);
    ILubyte* data = ilGetData();

    if (!data) {
        // std::cerr << "Failed to retrieve image data." << std::endl;
        ilDeleteImages(1, &imgId);
        return nullptr;
    }

    // Create an Image instance and load the texture data
    auto image = std::make_unique<Image>();
    image->load(width, height, channels, data);

    // Free DevIL resources
    ilDeleteImages(1, &imgId);

    return image;
}

std::shared_ptr<Image> TextureImporter::LoadTextureFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::in);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    std::shared_ptr<Image> image = std::make_shared<Image>();
    inFile >> *image;
    inFile.close();
    return image;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Image>& image) {
    if (!image) {
        return os;
    }

    // Escribe las propiedades de la textura como texto
    os << image->width() << " " << image->height() << " " << image->channels() << "\n";

    // Escribe los datos de la textura como texto
    const int dataSize = image->width() * image->height() * image->channels();
    glBindTexture(GL_TEXTURE_2D, 0); // Asegúrate de leer los datos desde la textura actual
    std::vector<unsigned char> data(dataSize);
    glGetTexImage(GL_TEXTURE_2D, 0, formatFromChannels(image->channels()), GL_UNSIGNED_BYTE, data.data());
    for (int i = 0; i < dataSize; ++i) {
        os << static_cast<int>(data[i]) << " ";
        if ((i + 1) % image->width() == 0) os << "\n";
    }

    return os;
}

std::istream& operator>>(std::istream& is, std::shared_ptr<Image>& image) {
    if (!image) {
        image = std::make_shared<Image>();
    }

    // Lee las propiedades de la textura
    int width, height, channels;
    is >> width >> height >> channels;

    // Calcula el tamaño de los datos de la textura y los lee
    const int dataSize = width * height * channels;
    std::vector<unsigned char> data(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        int pixelValue;
        is >> pixelValue;
        data[i] = static_cast<unsigned char>(pixelValue);
    }

    // Carga los datos en el objeto Image
    image->load(width, height, channels, data.data());

    return is;
}
void TextureImporter::SaveTextureToFile(const std::shared_ptr<Image>& image, const std::string& filePath) {
    if (!image) {
        throw std::runtime_error("Cannot save a null image.");
    }

    std::filesystem::path path(filePath);

    if (!std::filesystem::exists(path.parent_path())) {
        throw std::runtime_error("Directory does not exist: " + path.parent_path().string());
    }

    std::ofstream outFile(filePath, std::ios::out);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    // Escribe las propiedades de la textura
    outFile << image->width() << " " << image->height() << " " << image->channels() << "\n";

    // Escribe los datos de la textura
    const int dataSize = image->width() * image->height() * image->channels();
    glBindTexture(GL_TEXTURE_2D, 0); // Asegúrate de capturar la textura actual
    std::vector<unsigned char> data(dataSize);
    glGetTexImage(GL_TEXTURE_2D, 0, formatFromChannels(image->channels()), GL_UNSIGNED_BYTE, data.data());
    for (int i = 0; i < dataSize; ++i) {
        outFile << static_cast<int>(data[i]) << " ";
        if ((i + 1) % image->width() == 0) outFile << "\n";
    }

    outFile.close();
}