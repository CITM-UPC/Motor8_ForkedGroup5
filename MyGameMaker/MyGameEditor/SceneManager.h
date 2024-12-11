#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MyGameEngine/GameObject.h"
#include "../MyGameEngine/SceneSerializer.h"
class SceneManager
{
public:
	static void LoadGameObject(const std::string& filePath);
	static GameObject* getGameObject(int index);
    //File drop handler
	static void DeleteGameObject(GameObject* gameObject);
	static GameObject* CreateEmptyGameObject();
	static void LoadGameObject(const char* filePath);

public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;

	static void SaveScene(const std::string& filePath);
	static void LoadScene(const std::string& filePath);
	static GameObject* FindGameObjectByName(const std::string& name);
	static void ClearScene();		

	// Declaración de los métodos de escena
	static SceneSerializer::Scene GetCurrentScene();
	static void SetCurrentScene(const SceneSerializer::Scene& scene);

	static void spawnBakerHouse();
	static void spawnStreet();
};

