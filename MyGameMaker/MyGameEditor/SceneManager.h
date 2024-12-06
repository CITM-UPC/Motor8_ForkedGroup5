#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MyGameEngine/GameObject.h"
class SceneManager
{
public:
	static void LoadGameObject(const std::string& filePath);
	static void spawnBakerHouse();
	static void spawnStreet();
	static GameObject* getGameObject(int index);
    //File drop handler

public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;

};

