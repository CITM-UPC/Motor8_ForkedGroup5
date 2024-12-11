#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include "types.h"

struct Material {

	color4 color = Colors::White;
	Texture texture;

};
