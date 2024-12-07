#include "Texture.h"
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
	glGenTextures(1, &id1);
	glBindTexture(GL_TEXTURE_2D, id1);

	int nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		return true;
	}
	else {
		stbi_image_free(data);
		return false;
	}
}
