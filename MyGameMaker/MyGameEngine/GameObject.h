#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Transform.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Mesh.h"
#include <string>

class GameObject : public TreeExt<GameObject> {
private:
    Transform _transform;                       // Transformaci�n del objeto
    glm::u8vec3 _color = glm::u8vec3(255, 255, 255); // Color del objeto
    Texture _texture;                           // Textura del objeto
    std::shared_ptr<Mesh> _mesh_ptr;           // Puntero a la malla
    //std::vector<std::shared_ptr<Component>> _components; // Lista de componentes
    bool _active = true;                        // Estado de activaci�n
    std::string name;
	mutable bool hasCreatedCheckerTexture = false;		// Indica si la textura de cuadros ha sido creada
    // Restaura la textura original del objeto
    Texture textura;
	
public:
    bool hasCheckerTexture = false;
    // Constructor y destructor
    /*GameObject(const std::string& name = "New GameObject");
    ~GameObject();*/

    // M�todos para acceder y modificar propiedades
    const auto& transform() const { return _transform; }
    auto& transform() { return _transform; }

    const auto& color() const { return _color; }
    auto& color() { return _color; }

    const auto& texture() const { return _texture; }
    auto& texture() { return _texture; }

    const auto& mesh() const { return *_mesh_ptr; }
    auto& mesh() { return *_mesh_ptr; }

    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // Gesti�n de componentes
    //void addComponent(std::shared_ptr<Component> component);
    //void removeComponent(std::shared_ptr<Component> component);
    //std::vector<std::shared_ptr<Component>> getComponents() const;

    // Transformaci�n global del objeto
    Transform worldTransform() const { return isRoot() ? _transform : parent->worldTransform() * _transform; }

    // C�lculo de las cajas de colisi�n
    BoundingBox localBoundingBox() const; // Definir en el .cpp
    BoundingBox boundingBox() const { return _transform.mat() * localBoundingBox(); }
    BoundingBox worldBoundingBox() const; // Definir en el .cpp

    // M�todos para manejar textura y malla
    void setTextureImage(const std::shared_ptr<Image>& img_ptr) { _texture.setImage(img_ptr); }
    void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }
    void setTexture(const std::string& path);

    // Comprobaciones de existencia de textura y malla
    bool hasTexture() const { return _texture.id(); }
    bool hasMesh() const { return _mesh_ptr != nullptr; }

    // M�todo para dibujar el objeto
    void draw() const; // Definir en el .cpp

    // M�todos de ciclo de vida
    //virtual void awake();      // Inicializaci�n
    //virtual void start();      // Llamado al inicio
    //virtual void update(float deltaTime); // L�gica de actualizaci�n
    //virtual void onDestroy();  // Limpieza antes de eliminar el objeto

    // Activaci�n del objeto
    void setActive(bool active) { _active = active; }
    bool isActive() const { return _active; }

    void initializeCheckerTexture();

    // M�todos de jerarqu�a
    void setParent(GameObject* newParent);                  // Establece el padre de este objeto
    void addChild(GameObject* child);                      // A�ade un hijo a este objeto
    void removeChild(GameObject* child);                   // Elimina un hijo de este objeto
    bool hasChildren() const;                              // Devuelve true si tiene hijos
    const std::vector<GameObject*>& getChildren() const;   // Devuelve los hijos de este objeto

    // M�todos para obtener informaci�n del objeto
    GameObject* getParent() const;  // Devuelve el padre del objeto
    

private:    
    GameObject* parent = nullptr;         // Padre del objeto
    std::vector<GameObject*> children;    // Hijos del objeto
};


