#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtx/orthonormalize.hpp>
#include <iostream>
#include <string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <list>
#include "MyGameEngine/Camera.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"
#include "MyWindow.h"
#include "BasicShapesManager.h"
#include "MyGui.h"
#include "SceneManager.h"
#include "Console.h"
#include <MyGameEngine/GameObject.cpp>
#include "MyGameEngine/BoundingBox.h"
#include "MyGameEngine/types.h"
#include "MeshImporter.h"
#include "SceneImporter.h"


using namespace std;
using hrclock = chrono::high_resolution_clock;
using vec3 = glm::dvec3;

static const glm::ivec2 WINDOW_SIZE(1280, 720);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

//static Camera camera;
GameObject mainCamera("Main Camera");
GameObject secondaryCamera("Secondary Camera");
Camera* activeCamera = nullptr;  // Pointer to track the current active camera

MeshImporter importer;
MeshImporter::MeshDTO meshDTO;
auto testMesh = make_shared<Mesh>();

SDL_Event event;
bool rightMouseButtonDown = false;
int lastMouseX, lastMouseY;

// Inicialización de OpenGL
void initOpenGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}
//------CODIGO PARA GUARDAR Y CARGAR TEXTURAS------
//#include "TextureImporter.h"
//#include "stb_image.h"
//
//void ImportAndSaveTexture(const std::string& imagePath, const std::string& outputPath) {
//    int width, height, channels;
//    unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
//
//    if (!imageData) {
//        throw std::runtime_error("Failed to load texture: " + imagePath);
//    }
//
//    TextureImporter::TextureDTO texture;
//    texture.width = width;
//    texture.height = height;
//    texture.channels = channels;
//    texture.data.assign(imageData, imageData + (width * height * channels));
//
//    stbi_image_free(imageData);
//
//    TextureImporter importer;
//    importer.SaveTextureToBinaryFile(texture, outputPath);
//}
// -----------------------------------------
//TextureImporter importer;
//TextureImporter::TextureDTO texture = importer.LoadTextureFromBinaryFile("Library/texture.mytexture");


//Mouse relative positions
glm::vec2 getMousePosition() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

//Funcion para convertir de coordenadas de pantalla a coordenadas del mundo
glm::vec3 screenToWorld(const glm::vec2& mousePos, float depth, const glm::mat4& projection, const glm::mat4& view) {

    float x = (2.0f * mousePos.x) / WINDOW_SIZE.x - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y) / WINDOW_SIZE.y;  
    glm::vec4 clipCoords(x, y, -1.0f, 1.0f); 


    glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);


    glm::vec3 worldRay = glm::vec3(glm::inverse(view) * eyeCoords);
    worldRay = glm::normalize(worldRay);


    glm::vec3 cameraPosition = glm::vec3(glm::inverse(view)[3]); 
    return cameraPosition + worldRay * depth;
}



//RayCastFunctions
glm::vec3 getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    float x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportSize.y;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    return rayWorld;
}


bool intersectRayWithBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const BoundingBox& bbox) {
    float tmin = (bbox.min.x - rayOrigin.x) / rayDirection.x;
    float tmax = (bbox.max.x - rayOrigin.x) / rayDirection.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (bbox.min.y - rayOrigin.y) / rayDirection.y;
    float tymax = (bbox.max.y - rayOrigin.y) / rayDirection.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (bbox.min.z - rayOrigin.z) / rayDirection.z;
    float tzmax = (bbox.max.z - rayOrigin.z) / rayDirection.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmin >= 0.0f;
}
// Raycast desde el mouse para detectar si está sobre un GameObject
GameObject* raycastFromMouseToGameObject(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    glm::vec3 rayOrigin = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));
    glm::vec3 rayDirection = getRayFromMouse(mouseX, mouseY, projection, view, viewportSize);

    GameObject* hitObject = nullptr;

    for (auto& go : SceneManager::gameObjectsOnScene) {
        if (intersectRayWithBoundingBox(rayOrigin, rayDirection, go.boundingBox())) {
            hitObject = &go;
            break;
        }
    }
    return hitObject;
}



//File drop handler
std::string getFileExtension(const std::string& filePath) {
    // Find the last dot in the file path
    size_t dotPosition = filePath.rfind('.');

    // If no dot is found, return an empty string
    if (dotPosition == std::string::npos) {
        return "";
    }

    // Extract and return the file extension
    return filePath.substr(dotPosition + 1);
}

void handleFileDrop(const std::string& filePath, mat4 projection, mat4 view) {
    auto extension = getFileExtension(filePath);
    auto imageTexture = std::make_shared<Image>();
    int mouseX, mouseY;
    // Obtener posición actual del mouse
    SDL_GetMouseState(&mouseX, &mouseY);

    if (extension == "obj" || extension == "fbx" || extension == "dae") {
       
		SceneManager::LoadGameObject(filePath);
		SceneManager::getGameObject(SceneManager::gameObjectsOnScene.size() - 1)->transform().pos() = screenToWorld(glm::vec2(mouseX, mouseY), 10.0f, projection, view);
		SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();

    }
    else if (extension == "png" || extension == "jpg" || extension == "bmp") {
        imageTexture->loadTexture(filePath);
        // Detectar si el mouse está sobre algún GameObject
        GameObject* hitObject = raycastFromMouseToGameObject(mouseX, mouseY, projection, view, WINDOW_SIZE);
        if (hitObject) {
            // Si hay un GameObject debajo del mouse, aplicar la textura
            hitObject->setTextureImage(imageTexture);
            cout << "Texture applied to GameObject under mouse." << endl;
            Console::Instance().Log("Texture applied to GameObject under mouse.");
        }
        else {
            cout << "No GameObject under mouse to apply texture." << endl;
            Console::Instance().Log("No GameObject under mouse to apply texture.");
        }
    }
    else {
        cout << "Unsupported file extension: " << extension << endl;
        Console::Instance().Log("Unsupported file extension: ");
    }
}

//Renderizado del suelo
static void drawFloorGrid(int size, double step) {
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    for (double i = -size; i <= size; i += step) {
        glVertex3d(i, 0, -size);
        glVertex3d(i, 0, size);
        glVertex3d(-size, 0, i);
        glVertex3d(size, 0, i);
    }
    glEnd();
}

std::array<glm::vec3, 8> calculateFrustumCorners(const Camera& camera) {
    glm::mat4 viewProj = camera.projection() * camera.view();
    glm::mat4 invViewProj = glm::inverse(viewProj);

    // Clip space corners
    std::array<glm::vec4, 8> clipSpaceCorners = {
        glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
        glm::vec4(1, 1, -1, 1),   glm::vec4(-1, 1, -1, 1),
        glm::vec4(-1, -1, 1, 1),  glm::vec4(1, -1, 1, 1),
        glm::vec4(1, 1, 1, 1),    glm::vec4(-1, 1, 1, 1),
    };

    // Transform to world space
    std::array<glm::vec3, 8> worldSpaceCorners;
    for (size_t i = 0; i < 8; ++i) {
        glm::vec4 corner = invViewProj * clipSpaceCorners[i];
        corner /= corner.w;  // Perspective divide
        worldSpaceCorners[i] = glm::vec3(corner);
    }
    return worldSpaceCorners;
}

void drawFrustum(const std::array<glm::vec3, 8>& corners) {
    glLineWidth(2.0);
    glColor3f(1.0f, 0.0f, 0.0f); // Red frustum
    glBegin(GL_LINES);

    // Connect near plane corners
    glVertex3fv(glm::value_ptr(corners[0])); glVertex3fv(glm::value_ptr(corners[1]));
    glVertex3fv(glm::value_ptr(corners[1])); glVertex3fv(glm::value_ptr(corners[2]));
    glVertex3fv(glm::value_ptr(corners[2])); glVertex3fv(glm::value_ptr(corners[3]));
    glVertex3fv(glm::value_ptr(corners[3])); glVertex3fv(glm::value_ptr(corners[0]));

    // Connect far plane corners
    glVertex3fv(glm::value_ptr(corners[4])); glVertex3fv(glm::value_ptr(corners[5]));
    glVertex3fv(glm::value_ptr(corners[5])); glVertex3fv(glm::value_ptr(corners[6]));
    glVertex3fv(glm::value_ptr(corners[6])); glVertex3fv(glm::value_ptr(corners[7]));
    glVertex3fv(glm::value_ptr(corners[7])); glVertex3fv(glm::value_ptr(corners[4]));

    // Connect near and far planes
    glVertex3fv(glm::value_ptr(corners[0])); glVertex3fv(glm::value_ptr(corners[4]));
    glVertex3fv(glm::value_ptr(corners[1])); glVertex3fv(glm::value_ptr(corners[5]));
    glVertex3fv(glm::value_ptr(corners[2])); glVertex3fv(glm::value_ptr(corners[6]));
    glVertex3fv(glm::value_ptr(corners[3])); glVertex3fv(glm::value_ptr(corners[7]));

    glEnd();
}

void configureCamera() {
    glm::dmat4 projectionMatrix = glm::perspective(glm::radians(45.0), static_cast<double>(WINDOW_SIZE.x) / WINDOW_SIZE.y, 0.1, 100.0);
    //glm::dmat4 viewMatrix = camera.view();
   /* glm::dmat4 viewMatrix = mainCamera.GetComponent<CameraComponent>()->camera().view();*/
    glm::dmat4 viewMatrix = activeCamera->view();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(glm::value_ptr(projectionMatrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(glm::value_ptr(viewMatrix));
}

bool isBoundingBoxInsideFrustum(const BoundingBox& bbox, const std::list<Plane>& frustumPlanes) {
    for (const auto& plane : frustumPlanes) {
        if (plane.distance(bbox.v000()) < 0 &&
            plane.distance(bbox.v001()) < 0 &&
            plane.distance(bbox.v010()) < 0 &&
            plane.distance(bbox.v011()) < 0 &&
            plane.distance(bbox.v100()) < 0 &&
            plane.distance(bbox.v101()) < 0 &&
            plane.distance(bbox.v110()) < 0 &&
            plane.distance(bbox.v111()) < 0) {
            return false;
        }
    }
    return true;
}

// Función de renderizado
void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configureCamera();

    auto frustumPlanes = activeCamera->frustumPlanes();

    for (auto& go : SceneManager::gameObjectsOnScene) {
        if (isBoundingBoxInsideFrustum(go.boundingBox(), frustumPlanes)) {
            glColor3f(1.0f, 0.0f, 0.0f); // Red for culled objects
        }
        else {
            glColor3f(0.0f, 1.0f, 0.0f); // Green for visible objects
            go.draw();
        }
        drawBoundingBox(go.boundingBox());
    }
    auto frustumCorners = calculateFrustumCorners(secondaryCamera.GetComponent<CameraComponent>()->camera());
    drawFrustum(frustumCorners);

    drawFloorGrid(16, 0.25);
}

// Funciones de manejo de mouse
void mouseButton_func(int button, int state, int x, int y) {
    if (button == SDL_BUTTON_RIGHT) {
        rightMouseButtonDown = (state == SDL_PRESSED);
        lastMouseX = x;
        lastMouseY = y;
    }
}

//Camera rotation
float yaw = 0.0f;
float pitch = 0.0f;
const float MAX_PITCH = 89.0f;
bool altKeyDown = false;
bool altPressedOnce = false;
vec3 target;

void handleAltKey() {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    altKeyDown = state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT];
}

void orbitCamera(const vec3& target, int deltaX, int deltaY) {
    const float sensitivity = 0.1f;

    yaw += deltaX * sensitivity;
    pitch -= deltaY * sensitivity;
    float distance = glm::length(mainCamera.transform().pos() - target);

    vec3 newPosition;
    newPosition.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newPosition.y = target.y + distance * sin(glm::radians(pitch));
    newPosition.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

   
    mainCamera.GetComponent<CameraComponent>()->camera().transform().pos() = newPosition;
    mainCamera.GetComponent<CameraComponent>()->camera().transform().lookAt(target);
}
void mouseMotion_func(int x, int y) {
    if (rightMouseButtonDown && altKeyDown == false) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        const double sensitivity = 0.1;

        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;

        if (pitch > MAX_PITCH) pitch = MAX_PITCH;
        if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;

        mainCamera.GetComponent<CameraComponent>()->camera().transform().rotate(glm::radians(-deltaX * sensitivity), glm::vec3(0, 1, 0));
        mainCamera.GetComponent<CameraComponent>()->camera().transform().rotate(glm::radians(deltaY * sensitivity), glm::vec3(1, 0, 0));

        lastMouseX = x;
        lastMouseY = y;
        mainCamera.GetComponent<CameraComponent>()->camera().transform().alignCamera();
    }

    if (rightMouseButtonDown && altKeyDown) {
       
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        if (!altPressedOnce) {
            altPressedOnce = true;

            glm::vec2 mouseScreenPos = getMousePosition();

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1f, 100.0f);
            glm::mat4 view = mainCamera.GetComponent<CameraComponent>()->camera().view();
			if (SceneManager::selectedObject != nullptr) {
				target = SceneManager::selectedObject->transform().pos();
			}
			else {
				target = glm::vec3(0,0,0);
			}
            orbitCamera(target, deltaX, deltaY);
		}
        else {
            orbitCamera(target, deltaX, deltaY);
        }
        lastMouseX = x;
        lastMouseY = y;
        mainCamera.GetComponent<CameraComponent>()->camera().transform().alignCamera();
	}
	else {
		altPressedOnce = false; // Reinicia la bandera si Alt no está presionado
        /*
        
        TO DO: ARREGLAR BOOL QUE NO SE EJECUTE CADA FRAME
        
        
        */
	}
    
}

bool fKeyDown = false;
static void idle_func() {
    float move_speed = 0.1f;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    //Debug to rotate the initial baker house
    /*if (state[SDL_SCANCODE_T]) {

        SceneManager::gameObjectsOnScene[0].transform().rotate(glm::radians(10.0f), glm::vec3(0, -1, 0));
    }*/
    if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) {
        move_speed = 0.2f;
    }
    if (rightMouseButtonDown) {

        if (state[SDL_SCANCODE_W]) {
            std::cout << "Moving camera forward." << std::endl;
            activeCamera->transform().translate(glm::vec3(0, 0, move_speed));
            Console::Instance().Log("Moving camera forward.");
        }
        if (state[SDL_SCANCODE_S]) {
            std::cout << "Moving camera backward." << std::endl;
            activeCamera->transform().translate(glm::vec3(0, 0, -move_speed));
            Console::Instance().Log("Moving camera backward.");
        }
        if (state[SDL_SCANCODE_A]) {
            std::cout << "Moving camera left." << std::endl;
            activeCamera->transform().translate(glm::vec3(move_speed, 0, 0));
            Console::Instance().Log("Moving camera left.");
        }
        if (state[SDL_SCANCODE_D]) {
            std::cout << "Moving camera right." << std::endl;
            activeCamera->transform().translate(glm::vec3(-move_speed, 0, 0));
            Console::Instance().Log("Moving camera right.");
        }
        if (state[SDL_SCANCODE_E]) {
            std::cout << "Moving camera up." << std::endl;
            activeCamera->transform().translate(glm::vec3(0, move_speed, 0));
            Console::Instance().Log("Moving camera up.");
        }
        if (state[SDL_SCANCODE_Q]) {
            std::cout << "Moving camera down." << std::endl;
            activeCamera->transform().translate(glm::vec3(0, -move_speed, 0));
            Console::Instance().Log("Moving camera down.");
        }
    }

    if (state[SDL_SCANCODE_F] && !fKeyDown && SceneManager::selectedObject != NULL) {
        mainCamera.GetComponent<CameraComponent>()->camera().transform().pos() = SceneManager::selectedObject->transform().pos() + vec3(0, 1, 4);
        fKeyDown = true;
        mainCamera.GetComponent<CameraComponent>()->camera().transform().lookAt(SceneManager::selectedObject->transform().pos());
        std::cout << "Camera looking at target." << std::endl;
    }
    else if (!state[SDL_SCANCODE_F]) {
        fKeyDown = false;
    }
    mainCamera.GetComponent<CameraComponent>()->camera().transform().alignCamera();
}
void mouseWheel_func(int direction) {
    mainCamera.GetComponent<CameraComponent>()->camera().transform().translate(vec3(0, 0, direction * 0.1));
}
//debug, showing the bounding boxes, not finished


int main(int argc, char* argv[]) {
    ilInit();
    iluInit();

    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    MyGUI gui(window.windowPtr(), window.contextPtr());
    initOpenGL();

    // Posición inicial de la cámara
    mainCamera.name = "Main Camera";
    mainCamera.AddComponent<CameraComponent>();
    SceneManager::gameObjectsOnScene.push_back(mainCamera);
    mainCamera.GetComponent<CameraComponent>()->camera().transform().pos() = vec3(0, 1, 4);
    mainCamera.GetComponent<CameraComponent>()->camera().transform().rotate(glm::radians(180.0), vec3(0, 1, 0));


    GameObject scene1 = SceneImporter::loadFromFile("Assets/BakerHouse.fbx");
    GameObject scene2 = SceneImporter::loadFromFile("Assets/strawberry.fbx");
    //GameObject scene3 = SceneImporter::loadFromFile("Assets/Street environment_V01.fbx");
    GameObject scene3 = SceneImporter::loadFromFile("Assets/Street/street2.fbx");

    secondaryCamera.AddComponent<CameraComponent>();
    SceneManager::gameObjectsOnScene.push_back(secondaryCamera);
    secondaryCamera.GetComponent<CameraComponent>()->camera().transform().pos() = glm::dvec3(10, 5, 10);
    secondaryCamera.GetComponent<CameraComponent>()->camera().transform().lookAt(glm::dvec3(0, 0, 0)); // Look at the origin
   
    activeCamera = &mainCamera.GetComponent<CameraComponent>()->camera();

    

    while (window.isOpen()) {
        const auto t0 = hrclock::now();
		handleAltKey();
        // Obtener la posición actual del mouse
        glm::vec2 mouseScreenPos = getMousePosition();       

        

        display_func(); // Renderizar la escena
        gui.render();
        window.swapBuffers();

       

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);

        SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
        
        while (SDL_PollEvent(&event))
        {
            // Obtener matrices de proyección y vista de la cámara
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1f, 100.0f);
            glm::mat4 view = mainCamera.GetComponent<CameraComponent>()->camera().view();

          
            gui.processEvent(event);
            
            switch (event.type)
            {
			case SDL_QUIT:
				window.close();
				break;
            case SDL_DROPFILE:               
				cout << "File dropped: " << event.drop.file << endl;
                handleFileDrop(event.drop.file, projection, view);
                SDL_free(event.drop.file);
                break;
            case SDL_MOUSEBUTTONDOWN:   //MOUSEPICKING OBJETO SELECCION
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Raycast para detectar el objeto debajo del mouse
                    SceneManager::selectedObject = raycastFromMouseToGameObject(mouseScreenPos.x, mouseScreenPos.y, projection, view, WINDOW_SIZE);
                }
            case SDL_MOUSEBUTTONUP:
                mouseButton_func(event.button.button, event.button.state, event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                mouseMotion_func(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEWHEEL:
                mouseWheel_func(event.wheel.y);
                break;
            case SDL_KEYDOWN:
                glm::vec3 mouseWorldPos = screenToWorld(mouseScreenPos, 10.0f, projection, view);
                if (event.key.keysym.sym == SDLK_c) {  // Press 'C' to toggle cameras
                    if (activeCamera == &mainCamera.GetComponent<CameraComponent>()->camera()) {
                        activeCamera = &secondaryCamera.GetComponent<CameraComponent>()->camera();
                        std::cout << "Switched to Secondary Camera\n";
                    }
                    else {
                        activeCamera = &mainCamera.GetComponent<CameraComponent>()->camera();
                        std::cout << "Switched to Main Camera\n";
                    }
                }
                break;
            
                // Crear figuras en la posición 3D calculada
                switch (event.key.keysym.sym) {
                case SDLK_1:  // Crear Triángulo
                    BasicShapesManager::createFigure(1, SceneManager::gameObjectsOnScene, 1.0, mouseWorldPos);
                    SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
                    break;
                case SDLK_2:  // Crear Cuadrado
                    BasicShapesManager::createFigure(2, SceneManager::gameObjectsOnScene, 1.0, mouseWorldPos);
                    SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
                    break;
                case SDLK_3:  // Crear Cubo
                    BasicShapesManager::createFigure(3, SceneManager::gameObjectsOnScene, 1.0, mouseWorldPos);
                    SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
                    break;
                default:
                    break;
                }
                break;
			default:
				cout << event.type << endl;
				break;
            }



        }
        idle_func();    // Actualizar lógica de juego
    }
    return EXIT_SUCCESS;
}