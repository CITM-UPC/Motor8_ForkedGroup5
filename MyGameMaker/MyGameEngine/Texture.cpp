#include "Texture.h"
#include "TextureImporter.h"

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static auto GLWrapMode(Texture::WrapModes mode) {
	switch (mode) {
	case Texture::Repeat: return GL_REPEAT;
	case Texture::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case Texture::Clamp: return GL_CLAMP_TO_EDGE;
	default: return GL_REPEAT;
	}
}

static auto GLMagFilter(Texture::Filters filter) {
	switch (filter) {
	case Texture::Nearest: return GL_NEAREST;
	case Texture::Linear: return GL_LINEAR;
	default: return GL_NEAREST;
	}
}

static auto GLMinFilter(Texture::Filters filter) {
	switch (filter) {
	case Texture::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
	case Texture::Linear: return GL_LINEAR_MIPMAP_LINEAR;
	default: return GL_NEAREST_MIPMAP_LINEAR;
	}
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, _img_ptr->id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLMinFilter(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLMagFilter(filter));
}


bool Texture::loadFromFile(const std::string& path) {
	// Utilizar Image para cargar la textura
	_img_ptr = std::make_shared<Image>();
	_img_ptr->loadTexture(path);
	if (_img_ptr !=nullptr) {
		width = _img_ptr->width();
		height = _img_ptr->height();
		
		return true;
	}
	return false;
}

bool Texture::saveToBinaryFile(const std::string& filePath) const {
	if (!_img_ptr) return false;

	TextureImporter importer;
	TextureImporter::TextureDTO textureDTO;
	textureDTO.width = _img_ptr->width();
	textureDTO.height = _img_ptr->height();
	textureDTO.channels = _img_ptr->channels();
	//textureDTO.data = _img_ptr->load(); // Asume que `Image` tiene un método `getRawData`

	importer.SaveTextureToBinaryFile(textureDTO, filePath);
	return true;
}

bool Texture::loadFromBinaryFile(const std::string& filePath) {
	TextureImporter importer;
	TextureImporter::TextureDTO textureDTO = importer.LoadTextureFromBinaryFile(filePath);

	if (textureDTO.data.empty()) return false;

	_img_ptr = std::make_shared<Image>();
	_img_ptr->load(textureDTO.width, textureDTO.height, textureDTO.channels, &textureDTO.data);
	width = textureDTO.width;
	height = textureDTO.height;

	return true;
}