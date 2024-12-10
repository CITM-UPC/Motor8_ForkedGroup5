#include "SceneSerializer.h"
#include <fstream>
#include <stdexcept>

void WriteString(std::ofstream& file, const std::string& str) {
    size_t size = str.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(str.data(), size);
}

std::string ReadString(std::ifstream& file) {
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string str(size, '\0');
    file.read(&str[0], size);
    return str;
}

void SceneSerializer::SaveSceneToFile(const Scene& scene, const std::string& filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    WriteString(file, scene.name);

    size_t objectCount = scene.objects.size();
    file.write(reinterpret_cast<const char*>(&objectCount), sizeof(objectCount));

    for (const auto& object : scene.objects) {
        SaveSceneObject(file, object);
    }

    file.close();
}

void SceneSerializer::SaveSceneObject(std::ofstream& file, const SceneObject& object) {
    WriteString(file, object.name);

    file.write(reinterpret_cast<const char*>(&object.transform.position), sizeof(object.transform.position));
    file.write(reinterpret_cast<const char*>(&object.transform.rotation), sizeof(object.transform.rotation));
    file.write(reinterpret_cast<const char*>(&object.transform.scale), sizeof(object.transform.scale));

    WriteString(file, object.meshFilePath);
    WriteString(file, object.textureFilePath);

    size_t childCount = object.children.size();
    file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (const auto& child : object.children) {
        SaveSceneObject(file, child);
    }
}

SceneSerializer::Scene SceneSerializer::LoadSceneFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    Scene scene;
    scene.name = ReadString(file);

    size_t objectCount;
    file.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    scene.objects.resize(objectCount);
    for (auto& object : scene.objects) {
        object = LoadSceneObject(file);
    }

    file.close();
    return scene;
}

SceneSerializer::SceneObject SceneSerializer::LoadSceneObject(std::ifstream& file) {
    SceneObject object;
    object.name = ReadString(file);

    file.read(reinterpret_cast<char*>(&object.transform.position), sizeof(object.transform.position));
    file.read(reinterpret_cast<char*>(&object.transform.rotation), sizeof(object.transform.rotation));
    file.read(reinterpret_cast<char*>(&object.transform.scale), sizeof(object.transform.scale));

    object.meshFilePath = ReadString(file);
    object.textureFilePath = ReadString(file);

    size_t childCount;
    file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    object.children.resize(childCount);
    for (auto& child : object.children) {
        child = LoadSceneObject(file);
    }

    return object;
}
