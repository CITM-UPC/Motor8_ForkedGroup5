#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

    // Ajustes de transformación
    go.transform().pos() = vec3(4, 0, 0);
    go.transform().rotate(-1.5708, vec3(1, 0, 0));

    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}

/*void SceneManager::spawnStreetEnviroment()
{
    GameObject go;
    auto mesh = std::make_shared<Mesh>();
    mesh->LoadFile("Assets/Street/Street environment_V01.fbx");
    go.setMesh(mesh);

    // Cargar texturas
    auto imageTexture1 = std::make_shared<Image>();
    imageTexture1->loadTexture("Assets/Street/Building_V01_C.png");
    go.setTextureImage(imageTexture1);

    auto imageTexture2 = std::make_shared<Image>();
    imageTexture2->loadTexture("Assets/Street/Additional_Texture_01.png");
    go.addTextureImage(imageTexture2);

    auto imageTexture3 = std::make_shared<Image>();
    imageTexture3->loadTexture("Assets/Street/Additional_Texture_02.png");
    go.addTextureImage(imageTexture3);

    // Ajustes de transformación
    go.transform().pos() = vec3(4, 0, 0);
    go.transform().rotate(-1.5708, vec3(1, 0, 0));

    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}*/

void SceneManager::spawnStreetEnviroment()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("Assets/Street/Street environment_V01.fbx", aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Console::Instance().Log("Error loading FBX file: " + std::string(importer.GetErrorString()));
        return;
    }

    processNode(scene->mRootNode, scene);
}

void SceneManager::processNode(aiNode* node, const aiScene* scene)
{
    // Procesar todos los meshes (mallas) en el nodo actual
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, node->mTransformation);
    }

    // Procesar los nodos hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void DecomposeMatrix(const aiMatrix4x4& transform, aiVector3D& scaling, aiQuaternion& rotation, aiVector3D& position) {
    // Extraer la posición
    position.x = transform.a4;
    position.y = transform.b4;
    position.z = transform.c4;

    // Extraer la escala
    scaling.x = sqrt(transform.a1 * transform.a1 + transform.b1 * transform.b1 + transform.c1 * transform.c1);
    scaling.y = sqrt(transform.a2 * transform.a2 + transform.b2 * transform.b2 + transform.c2 * transform.c2);
    scaling.z = sqrt(transform.a3 * transform.a3 + transform.b3 * transform.b3 + transform.c3 * transform.c3);

    // Extraer la rotación
    aiMatrix3x3 rotationMatrix(transform);
    rotation = aiQuaternion(rotationMatrix);
}



void SceneManager::processMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform)
{
    GameObject go;
    auto myMesh = std::make_shared<Mesh>();
    myMesh->LoadFromAssimpMesh(mesh);
    go.setMesh(myMesh);

    // Cargar texturas
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, i, &str);
            auto imageTexture = std::make_shared<Image>();
            imageTexture->loadTexture("Assets/Street/" + std::string(str.C_Str()));
            if (i == 0) {
                go.setTextureImage(imageTexture);
            }
            else {
                go.addTextureImage(imageTexture);
            }
        }
    }

    // Aplicar la transformación del nodo
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    DecomposeMatrix(transform, scaling, rotation, position);

    go.transform().pos() = vec3(position.x, position.y, position.z);
    go.transform().scale() = vec3(scaling.x, scaling.y, scaling.z);

    // Convertir aiQuaternion a glm::quat
    glm::quat glmRotation(rotation.w, rotation.x, rotation.y, rotation.z);

    // Convertir glm::quat a ángulo y eje de rotación
    float angle = glm::angle(glmRotation);
    glm::vec3 axis = glm::axis(glmRotation);

    // Llamar a la función rotate con ángulo y eje
    go.transform().rotate(angle, axis);

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
