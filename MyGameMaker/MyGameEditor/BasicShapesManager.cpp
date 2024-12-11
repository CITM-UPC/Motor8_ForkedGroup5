#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"
#include "BasicShapesManager.h"

// Gestor de figuras geométricas básicas

// Triángulo
std::shared_ptr<Mesh> BasicShapesManager::MakeTriangleMesh(double size) {
    const glm::vec3 vertices[] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(0, size, 0) };
    unsigned int indices[] = { 0, 1, 2 };
    const glm::vec2 texcoords[] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0.5, 0) };
    const glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
    const glm::u8vec3 colors[] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };

    auto mesh_ptr = std::make_shared<Mesh>();
    mesh_ptr->load(vertices, 3, indices, 3);
    mesh_ptr->loadTexCoords(texcoords, sizeof(texcoords) / sizeof(texcoords[0]));
    mesh_ptr->loadNormals(normals, sizeof(normals) / sizeof(normals[0]));
    mesh_ptr->loadColors(colors, sizeof(colors) / sizeof(colors[0]));
    return mesh_ptr;
}

// Cuadrado
std::shared_ptr<Mesh> BasicShapesManager::MakeQuadMesh(double size) {
    const glm::vec3 vertices[] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(size, size, 0), glm::vec3(-size, size, 0) };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    const glm::vec2 texcoords[] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0) };
    const glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
    const glm::u8vec3 colors[] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255), glm::u8vec3(255, 255, 0) };

    auto mesh_ptr = std::make_shared<Mesh>();
    mesh_ptr->load(vertices, 4, indices, 6);
    mesh_ptr->loadTexCoords(texcoords, sizeof(texcoords) / sizeof(texcoords[0]));
    mesh_ptr->loadNormals(normals, sizeof(normals) / sizeof(normals[0]));
    mesh_ptr->loadColors(colors, sizeof(colors) / sizeof(colors[0]));
    return mesh_ptr;
}

// Cubo
std::shared_ptr<Mesh> BasicShapesManager::MakeCubeMesh(double size) {
    const glm::vec3 vertices[] = {
        // Cara frontal
        {-size, -size, size}, {size, -size, size}, {size, size, size}, {-size, size, size},
        // Cara trasera
        {-size, -size, -size}, {-size, size, -size}, {size, size, -size}, {size, -size, -size}
    };
    unsigned int indices[] = {
        // Cara frontal
        0, 1, 2, 2, 3, 0,
        // Cara trasera
        4, 5, 6, 6, 7, 4,
        // Cara izquierda
        4, 0, 3, 3, 5, 4,
        // Cara derecha
        1, 7, 6, 6, 2, 1,
        // Cara superior
        3, 2, 6, 6, 5, 3,
        // Cara inferior
        4, 7, 1, 1, 0, 4
    };
    const glm::vec2 texcoords[] = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1},
        {0, 0}, {1, 0}, {1, 1}, {0, 1}
    };
    const glm::vec3 normals[] = {
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}
    };
    const glm::u8vec3 colors[] = {
        {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0},
        {255, 0, 255}, {0, 255, 255}, {255, 128, 0}, {128, 0, 255}
    };

    auto mesh_ptr = std::make_shared<Mesh>();
    mesh_ptr->load(vertices, 8, indices, 36);
    mesh_ptr->loadTexCoords(texcoords, sizeof(texcoords) / sizeof(texcoords[0]));
    mesh_ptr->loadNormals(normals, sizeof(normals) / sizeof(normals[0]));
    mesh_ptr->loadColors(colors, sizeof(colors) / sizeof(colors[0]));
    return mesh_ptr;
}

void BasicShapesManager::createFigure(int figureType, std::vector<GameObject>& gameObjects, double size, glm::vec3 mousePosition) {
    GameObject go;

    // Dependiendo del tipo de figura
    switch (figureType) {
    case 1:  // Triángulo
       // go.setMesh(MakeTriangleMesh(size));
        break;
    case 2:  // Cuadrado
        //go.setMesh(MakeQuadMesh(size));
        break;
    case 3:  // Cubo
       // go.setMesh(MakeCubeMesh(size));
        break;
    default:
        std::cout << "Figure type not recognized." << std::endl;
        return;
    }

    // Establece la posición del GameObject en la posición 3D calculada desde la posición del ratón
    go.transform().translate(mousePosition);
    go.setName("GameObject (" + std::to_string(gameObjects.size()) + ")");
    gameObjects.push_back(go);
}
