#pragma once

#include <memory>
#include "Image.h"

class Texture
{
public:
	enum WrapModes { Repeat, MirroredRepeat, Clamp };
	WrapModes wrapMode = Repeat;

	enum Filters { Nearest, Linear };
	Filters filter = Nearest;

	
	

private:
	std::shared_ptr<Image> _img_ptr;

	unsigned int id1;
	int width, height;

public:
	unsigned int id() const { return _img_ptr ? _img_ptr->id() : 0; }
	void bind() const;
	void setImage(const std::shared_ptr<Image>& img_ptr) { _img_ptr = img_ptr; }
	const auto& image() const { return *_img_ptr; }
	bool loadFromFile(const std::string& path);
	std::shared_ptr<Image> imagePtr() const { return _img_ptr; }
};

