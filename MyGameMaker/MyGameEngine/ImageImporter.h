#pragma once

#include <string>
#include <memory>
#include "MyGameEngine/Image.h"

class ImageImporter {
public:

	static std::shared_ptr<Image> loadFromFile(const std::string& path);

};

