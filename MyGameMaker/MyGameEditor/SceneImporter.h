#pragma once
#include <string>

#include "MyGameEngine/GameObject.h"

class SceneImporter {
public:
	static GameObject loadFromFile(const std::string& path);
};