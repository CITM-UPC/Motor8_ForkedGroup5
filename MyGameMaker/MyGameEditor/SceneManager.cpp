#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"

std::vector<GameObject> SceneManager::gameObjectsOnScene;
GameObject* SceneManager::selectedObject = nullptr;



void SceneManager::LoadGameObject(const std::string& filePath) {
    auto mesh = std::make_shared<Mesh>();

    GameObject go;
    mesh->LoadFile(filePath.c_str());
    go.setMesh(mesh);
    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    gameObjectsOnScene.push_back(go);
    Console::Instance().Log("Fbx imported succesfully.");
}

void SceneManager::DeleteGameObject(GameObject* gameObject) {
    auto it = std::find_if(gameObjectsOnScene.begin(), gameObjectsOnScene.end(),
        [gameObject](const GameObject& go) { return &go == gameObject; });
    if (it != gameObjectsOnScene.end()) {
        gameObjectsOnScene.erase(it);
        Console::Instance().Log("GameObject deleted.");
    }
}

GameObject* SceneManager::CreateEmptyGameObject() {
    GameObject newObject;
    newObject.setName("Empty GameObject");
    gameObjectsOnScene.push_back(newObject);
    Console::Instance().Log("Empty GameObject created.");
    return &gameObjectsOnScene.back();
}

void SceneManager::LoadGameObject(const char* filePath) {
    // Implementación de carga de GameObject desde un archivo
    // Aquí puedes agregar la lógica para cargar un GameObject desde un archivo
    Console::Instance().Log("GameObject loaded from file: " + std::string(filePath));
}

GameObject* SceneManager::getGameObject(int index) {
	return &gameObjectsOnScene[index];
}
