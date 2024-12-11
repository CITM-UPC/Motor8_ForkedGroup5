#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MyGameEngine/GameObject.h"
class SceneManager {
public:
    static void spawnBakerHouse();
    static void spawnStreet();
    static void spawnStreetEnviroment();
    static void LoadGameObject(const std::string& filePath);
    static void DeleteGameObject(GameObject* gameObject);
    static GameObject* CreateEmptyGameObject();
    static void LoadGameObject(const char* filePath);
    static GameObject* getGameObject(int index);

    static std::vector<GameObject> gameObjectsOnScene;
    static GameObject* selectedObject;

private:
    static void processNode(aiNode* node, const aiScene* scene);
    static void processMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform);

    
};

