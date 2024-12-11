#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"
#include "../MyGameEngine/SceneSerializer.h"

std::vector<GameObject> SceneManager::gameObjectsOnScene;
GameObject* SceneManager::selectedObject = nullptr;

void SceneManager::LoadGameObject(const std::string& filePath) {
    auto mesh = std::make_shared<Mesh>();

    GameObject go;
    go.setMesh(filePath);
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

void SceneManager::SaveScene(const std::string& filePath) {
    SceneSerializer serializer;
    SceneSerializer::Scene scene;

    for (auto& go : gameObjectsOnScene) {
        SceneSerializer::SceneObject obj;
        obj.name = go.getName();
        obj.transform.position = go.transform().pos();
        obj.transform.rotation = go.transform().extractEulerAngles(go.transform().mat());
        obj.transform.scale = go.transform().extractScale(go.transform().mat());
        obj.meshFilePath = go.meshFilePath;
        obj.textureFilePath = go.getTexturePath();
        scene.objects.push_back(obj);
    }

    serializer.SaveSceneToFile(scene, filePath);
    Console::Instance().Log("Scene saved to: " + filePath);
}

void SceneManager::LoadScene(const std::string& filePath) {
    SceneSerializer serializer;
    auto scene = serializer.LoadSceneFromFile(filePath);

    gameObjectsOnScene.clear();

    for (auto& obj : scene.objects) {
        GameObject go;
        go.setName(obj.name);
        go.transform().setPos(obj.transform.position);
        go.transform().setRotation(obj.transform.rotation);
        go.transform().setScale(obj.transform.scale);
        go.setMesh(filePath);
        go.setTexture(obj.textureFilePath);
        gameObjectsOnScene.push_back(go);
    }

    Console::Instance().Log("Scene loaded from: " + filePath);
}

// Retorna la escena actual serializada
SceneSerializer::Scene SceneManager::GetCurrentScene() {
    SceneSerializer::Scene scene;

    for (auto& go : gameObjectsOnScene) {
        SceneSerializer::SceneObject obj;
        obj.name = go.getName();
        obj.transform.position = go.transform().pos();
        obj.transform.rotation = go.transform().extractEulerAngles(go.transform().mat());
        obj.transform.scale = go.transform().extractScale(go.transform().mat());
        obj.meshFilePath = go.getMeshPath();
        obj.textureFilePath = go.getTexturePath();
        scene.objects.push_back(obj);
    }

    return scene;
}

// Establece la escena actual desde una escena serializada
void SceneManager::SetCurrentScene(const SceneSerializer::Scene& scene) {
    gameObjectsOnScene.clear();

    for (const auto& obj : scene.objects) {
        GameObject go;
        go.setName(obj.name);
        go.transform().setPos(obj.transform.position);
        go.transform().setRotation(obj.transform.rotation);
        go.transform().setScale(obj.transform.scale);
        go.setMesh(obj.meshFilePath);
        go.setTexture(obj.textureFilePath);
        gameObjectsOnScene.push_back(go);
    }
}