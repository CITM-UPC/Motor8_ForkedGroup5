#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"

std::vector<GameObject> SceneManager::gameObjectsOnScene;
GameObject* SceneManager::selectedObject = nullptr;

void SceneManager::spawnBakerHouse() 
{
    GameObject go;
    auto mesh = std::make_shared<Mesh>();
    mesh->LoadFile("Assets/BakerHouse.fbx");
    go.setMesh(mesh);
    auto imageTexture = std::make_shared<Image>();
    imageTexture->loadTexture("Assets/Baker_House.png");
	go.setTextureImage(imageTexture);
	go.transform().pos() = vec3(4, 0, 0);
    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}

void SceneManager::spawnStreet()
{
    GameObject go;
    auto mesh = std::make_shared<Mesh>();
    mesh->LoadFile("Assets/Street/Street environment_V01.fbx");
    go.setMesh(mesh);
    auto imageTexture = std::make_shared<Image>();
    imageTexture->loadTexture("Assets/Street/Building_V01_C.png");
    go.setTextureImage(imageTexture);
    go.transform().pos() = vec3(4, 0, 0);
    go.transform().rotate(-1.5708, vec3(4, 0, 0));
    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}


void SceneManager::LoadGameObject(const std::string& filePath) {
    auto mesh = std::make_shared<Mesh>();

    GameObject go;
    mesh->LoadFile(filePath.c_str());
    go.setMesh(mesh);
    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    gameObjectsOnScene.push_back(go);
    Console::Instance().Log("Fbx imported succesfully.");
}

GameObject* SceneManager::getGameObject(int index) {
	return &gameObjectsOnScene[index];
}
