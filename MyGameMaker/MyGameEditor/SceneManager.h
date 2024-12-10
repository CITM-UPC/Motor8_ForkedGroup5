#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/SceneSerializer.h"

class SceneManager
{
public:
	static void LoadGameObject(const std::string& filePath);
	static void spawnBakerHouse();
	static void spawnStreet();
	static GameObject* getGameObject(int index);
    //File drop handler
	static void DeleteGameObject(GameObject* gameObject);
	static GameObject* CreateEmptyGameObject();
	static void LoadGameObject(const char* filePath);

public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;

public:
	static SceneSerializer::Scene& GetCurrentScene();  // Devuelve la escena actual
	static void SetCurrentScene(const SceneSerializer::Scene& scene); // Actualiza la escena actual

private:
	static SceneSerializer::Scene currentScene;  // La escena actual
};

