#include "GameObject.h"
#include "Texture.h"
#include "Mesh.h"
#include "../MyGameEditor/MeshImporter.h"
#include "TextureImporter.h"
#include <GL/glew.h>
#include <glm/gtc/epsilon.hpp>
#include <iostream>

const int CHECKERS_WIDTH = 64;
const int CHECKERS_HEIGHT = 64;
GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
GLuint checker_texture_id;


void deleteCheckerTexture() {
    if (checker_texture_id) {
        glDeleteTextures(1, &checker_texture_id);
		checker_texture_id = 0;
    }
}

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component))
{
    AddComponent<TransformComponent>();
    if (name == "Main Camera")
    {
        AddComponent<CameraComponent>();
    }
}

// Crea la textura de tablero y devuelve el ID de la textura
void CheckerTexture(bool hasCreatedCheckerImage) {
    
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255);
            checkerImage[i][j][0] = (GLubyte)c; // Rojo
            checkerImage[i][j][1] = (GLubyte)c; // Verde
            checkerImage[i][j][2] = (GLubyte)c; // Azul
            checkerImage[i][j][3] = (GLubyte)255; // Opacidad completa
        }
    }
    
	deleteCheckerTexture();
    glGenTextures(1, &checker_texture_id);
    glBindTexture(GL_TEXTURE_2D, checker_texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::setTexture(const std::string& path) {
    textura.loadFromBinaryFile(path);
    setTexturePath(path);                                 // Guardar la ruta
}

void GameObject::setMesh(const std::string& path) {
    MeshImporter importer;
    auto dto = importer.LoadMeshFromBinaryFile(path); // Cargar malla desde archivo
    _mesh_ptr = std::make_shared<Mesh>(dto);              // Crear malla usando el DTO
    setMeshPath(path);                               // Guardar la ruta
}


inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
    glBegin(GL_LINE_LOOP);
    glVertex3(v0);
    glVertex3(v1);
    glVertex3(v2);
    glVertex3(v3);
    glEnd();
}

static void drawBoundingBox(const BoundingBox& bbox) {
    glLineWidth(2.0);
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
    drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
    drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
    drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());
}

void GameObject::draw() const {
    glPushMatrix();
    glMultMatrixd(_transform.data());
    glColor3ubv(&_color.r);

    if (hasTexture()) {
        glEnable(GL_TEXTURE_2D);

        if (hasCheckerTexture) {
            if (!hasCreatedCheckerTexture) 
            {
                hasCreatedCheckerTexture = true;
            }
            CheckerTexture(hasCreatedCheckerTexture);
        }
        else {
            if (hasCreatedCheckerTexture)
            {
                hasCreatedCheckerTexture = false;
            }
            _texture.bind();
            
        }
    }

    if (hasMesh()) _mesh_ptr->draw();

    if (hasTexture()) glDisable(GL_TEXTURE_2D);

    if (parent == nullptr) {
    for (const auto& child : children)
        child->draw();
        
    }
    

    glPopMatrix();
}

BoundingBox GameObject::localBoundingBox() const {
    if (!children.empty()) {
        BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children.front()->boundingBox();
        for (const auto& child : children) bbox = bbox + child->boundingBox();
        return bbox;
    }

    else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GameObject::worldBoundingBox() const {
    BoundingBox bbox = worldTransform().mat() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
    for (const auto& child : children) bbox = bbox + child->worldBoundingBox();
    return bbox;
}

void GameObject::setParent(GameObject* newParent) {
    if (this == newParent) return; // Prevenir auto-emparentamiento

    // Remover del padre actual (si existe)
    if (parent != nullptr) {
        parent->removeChild(this);
    }

    // Asignar el nuevo padre
    parent = newParent;

    // Agregar este objeto como hijo del nuevo padre
    if (newParent != nullptr) {
        newParent->addChild(this);
    }
}

void GameObject::addChild(GameObject* child) {
    if (std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void GameObject::removeChild(GameObject* child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}
bool GameObject::hasChildren() const {
    return !children.empty();
}

const std::vector<GameObject*>& GameObject::getChildren() const {
    return children;
}
