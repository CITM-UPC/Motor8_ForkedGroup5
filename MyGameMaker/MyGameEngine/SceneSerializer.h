#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

class SceneSerializer {
public:
    struct Transform {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    struct SceneObject {
        std::string name;                 // Nombre del objeto
        Transform transform;              // Transformaciones
        std::string meshFilePath;         // Ruta al archivo de malla (en formato interno)
        std::string textureFilePath;      // Ruta al archivo de textura (en formato interno)
        std::vector<SceneObject> children; // Objetos hijos (para jerarquía)
    };

    struct Scene {
        std::string name;                 // Nombre de la escena
        std::vector<SceneObject> objects; // Lista de objetos raíz
    };

    void SaveSceneToFile(const Scene& scene, const std::string& filePath);
    Scene LoadSceneFromFile(const std::string& filePath);

private:
    void SaveSceneObject(std::ofstream& file, const SceneObject& object);
    SceneObject LoadSceneObject(std::ifstream& file);
};
