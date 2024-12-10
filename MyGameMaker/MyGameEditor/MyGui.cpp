#include "MyGUI.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/SceneSerializer.h"
#include "SceneManager.h"
#include "BasicShapesManager.h"
#include "SystemInfo.h"
#include "Console.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs/tinyfiledialogs.h> 
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>

#include <windows.h>
#include <psapi.h>

bool show_metrics_window = false;
bool show_hardware_window = false;
bool show_software_window = false;
bool show_spawn_figures_window = false;
bool is_running = false;
bool is_paused = false;

MyGUI::MyGUI(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void MyGUI::ShowMainMenuBar() {
    if (show_metrics_window) {
        ShowMetricsWindow(&show_metrics_window);
    }
    if (show_hardware_window) {
        ShowRenderSystemInfo(&show_hardware_window);
    }
    if (show_software_window) {
        ShowLibraryVerions(&show_software_window);
    }
    if (show_spawn_figures_window) {
        ShowSpawnFigures(&show_spawn_figures_window);
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // Men� para guardar y cargar escenas
            if (ImGui::MenuItem("Save Scene")) {
                const char* filePath = tinyfd_saveFileDialog(
                    "Save Scene",
                    "Library/Scenes/MyScene.scene",
                    0, nullptr, nullptr
                );
                if (filePath) {
                    SceneSerializer serializer;
                    serializer.SaveSceneToFile(SceneManager::GetCurrentScene(), filePath);
                    Console::Instance().Log("Scene saved to " + std::string(filePath));
                }
            }
            if (ImGui::MenuItem("Load Scene")) {
                const char* filePath = tinyfd_openFileDialog(
                    "Load Scene",
                    "",
                    0, nullptr, nullptr,
                    0
                );
                if (filePath) {
                    SceneSerializer serializer;
                    auto scene = serializer.LoadSceneFromFile(filePath);
                    SceneManager::SetCurrentScene(scene);
                    Console::Instance().Log("Scene loaded from " + std::string(filePath));
                }
            }
            if (ImGui::MenuItem("Quit")) {
                SDL_Quit();
                exit(0);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                const char* url = "https://github.com/CITM-UPC/Motor8_ForkedGroup5";
                SDL_OpenURL(url);
            }
            ImGui::Checkbox("Metrics", &show_metrics_window);
            ImGui::Checkbox("Hardware Info", &show_hardware_window);
            ImGui::Checkbox("Software Info", &show_software_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}


void MyGUI::ShowConsole() {
    ImGui::SetNextWindowSize(ImVec2(680, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, ImGui::GetIO().DisplaySize.y - 200), ImGuiCond_Always);

    ImGui::Begin("Console");

    if (ImGui::Button("Clear")) {
        Console::Instance().Clear();
    }

    const auto& messages = Console::Instance().GetMessages();
    ImGui::Text("Message Count: %zu", messages.size());
    for (const auto& message : messages) {
        ImGui::Text("%s", message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::End();
}

void MyGUI::ShowAssetsFolder(bool* p_open) {
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Assets Folder", p_open);

    static std::vector<std::string> files;
    static bool filesLoaded = false;

    if (!filesLoaded) {
        std::string assetsPath = "Assets";
        for (const auto& entry : std::experimental::filesystem::directory_iterator(assetsPath)) {
            files.push_back(entry.path().filename().string());
        }
        filesLoaded = true;
    }

    for (int i = 0; i < files.size(); ++i) {
        if (ImGui::Selectable(files[i].c_str())) {
            Console::Instance().Log("Selected file: " + files[i]);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const char* filePath = files[i].c_str();
            ImGui::SetDragDropPayload("ASSET_FILE", filePath, strlen(filePath) + 1);
            ImGui::Text("Dragging %s", files[i].c_str());
            ImGui::EndDragDropSource();
        }
    }

    ImGui::End();
}


void MyGUI::ShowSpawnFigures(bool* p_open) {
    ImGui::Begin("Spawn Figures");

    if (ImGui::Button("Spawn Triangle")) {
        BasicShapesManager::createFigure(1, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f, 0.0f, 0.0f));
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }

    if (ImGui::Button("Spawn Square")) {
        BasicShapesManager::createFigure(2, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f, 0.0f, 0.0f));
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }

    if (ImGui::Button("Spawn Cube")) {
        BasicShapesManager::createFigure(3, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f, 0.0f, 0.0f));
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }
    ImGui::End();
}

static float GetMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter))) {
        return static_cast<float>(memCounter.WorkingSetSize) / (1024.0f * 1024.0f); // Convert bytes to MB
    }
    return 0.0f; // Return 0 if there's an issue getting the memory info
}

void MyGUI::ShowMetricsWindow(bool* p_open) {
    static std::vector<float> fpsHistory;
    static std::vector<float> memoryHistory;
    static const int maxSamples = 100;

    // Gather data for FPS
    float fps = ImGui::GetIO().Framerate;
    fpsHistory.push_back(fps);
    if (fpsHistory.size() > maxSamples) {
        fpsHistory.erase(fpsHistory.begin());
    }

    // Gather data for Memory Usage
    float memoryUsage = GetMemoryUsage();
    memoryHistory.push_back(memoryUsage);
    if (memoryHistory.size() > maxSamples) {
        memoryHistory.erase(memoryHistory.begin());
    }

    ImGui::Begin("Performance Graphs");

    ImGui::Text("FPS Graph");
    ImGui::PlotLines("FPS", fpsHistory.data(), static_cast<int>(fpsHistory.size()), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
    ImGui::Text("Current FPS: %.1f", fps);

    ImGui::Separator();

    ImGui::Text("Memory Usage Graph");
    ImGui::PlotLines("Memory (MB)", memoryHistory.data(), static_cast<int>(memoryHistory.size()), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
    ImGui::Text("Current Memory Usage: %.1f MB", memoryUsage);

    ImGui::End();
}

void MyGUI::ShowRenderSystemInfo(bool* p_open) {
    ImGui::Begin("Hardware Information");
    std::string systemInfo = SystemInfo::GetFullSystemInfo();
    ImGui::TextWrapped("%s", systemInfo.c_str());
    ImGui::End();
}

void MyGUI::ShowLibraryVerions(bool* p_open) {
    ImGui::Begin("Software Information");
    std::string libraryInfo = SystemInfo::GetFullLibraryVerions();
    ImGui::TextWrapped("%s", libraryInfo.c_str());
    ImGui::End();
}

void MyGUI::ShowHierarchy() {
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);

    if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        // Iterar sobre todos los objetos en la escena
        for (auto& go : SceneManager::gameObjectsOnScene) {
            if (SceneManager::gameObjectsOnScene.empty()) continue;

            // Identificar si este objeto es seleccionado
            bool isSelected = (SceneManager::selectedObject == &go);            
         
            // Crear la entrada en la jerarqu�a
            if (ImGui::Selectable(go.getName().c_str(), isSelected)) {
                // Actualizar el objeto seleccionado en SceneManager
                SceneManager::selectedObject = &go;
            }

            // Comenzar el "drag" si se selecciona este objeto
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                GameObject* goPtr = &go; // Aseg�rate de que est�s pasando un puntero v�lido
                ImGui::SetDragDropPayload("GAMEOBJECT", &goPtr, sizeof(GameObject*)); // El payload debe contener el puntero
                ImGui::Text("Dragging %s", go.getName().c_str());
                ImGui::EndDragDropSource();
            }


            // Hacer que este objeto sea un "drop target"
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    IM_ASSERT(payload->DataSize == sizeof(GameObject*)); // Verifica el tama�o del payload
                    GameObject* draggedObject = *(GameObject**)payload->Data;

                    if (draggedObject == nullptr) {
                        std::cout << "Error: Dragged object is null!" << std::endl;
                    }
                    else {
                        std::cout << "Dragged object: " << draggedObject->getName() << std::endl;
                        // Aqu� se maneja el emparentamiento
                        if (draggedObject != &go) {
                            draggedObject->setParent(&go);
                           // SceneManager::gameObjectsOnScene.erase(draggedObject);
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            // A�adir un desplazamiento visual para mostrar jerarqu�as
            if (go.hasChildren()) {
                ImGui::Indent();
                for (auto& child : go.getChildren()) {
                    ImGui::Text("  %s", child->getName().c_str());
                }
                ImGui::Unindent();
            }
            if (ImGui::TreeNode(go.getName().c_str())) {
                for (auto& child : go.getChildren()) {
                    ImGui::BulletText("%s", child->getName().c_str());
                }
                ImGui::TreePop();
            }
        }

        ImGui::End();
    }
}

void MyGUI::renderInspector() {
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(980, 20), ImGuiCond_Always);
    ImGui::Begin("Inspector");

    static GameObject* persistentSelectedObject = nullptr;

    if (SceneManager::selectedObject != nullptr) {
        persistentSelectedObject = SceneManager::selectedObject;
    }

    if (persistentSelectedObject) {
        if (ImGui::CollapsingHeader("Transform")) {
            glm::vec3 position = persistentSelectedObject->transform().pos();
            glm::vec3 rotation = glm::vec3(persistentSelectedObject->transform().extractEulerAngles(persistentSelectedObject->transform().mat()));
            glm::vec3 scale = persistentSelectedObject->transform().extractScale(persistentSelectedObject->transform().mat());

            if (ImGui::DragFloat3("Position", &position[0], 1.0f)) {
                persistentSelectedObject->transform().setPos(position);
            }
            if (ImGui::DragFloat3("Rotation", &rotation[0], 1.0f)) {
                persistentSelectedObject->transform().setRotation(rotation);
            }
            if (ImGui::DragFloat3("Scale", &scale[0], 1.0f)) {
                persistentSelectedObject->transform().setScale(scale);
            }
        }

        // Opciones de malla
        if (persistentSelectedObject->hasMesh() && ImGui::CollapsingHeader("Mesh")) {
            ImGui::Text("Mesh Path: %s", persistentSelectedObject->meshPath.c_str());
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE")) {
                    const char* path = (const char*)payload->Data;
                    persistentSelectedObject->setMesh(path);
                    Console::Instance().Log("Mesh assigned: " + std::string(path));
                }
                ImGui::EndDragDropTarget();
            }
        }

        // Opciones de textura
        if (persistentSelectedObject->hasTexture() && ImGui::CollapsingHeader("Texture")) {
            ImGui::Text("Texture Path: %s", persistentSelectedObject->texturePath.c_str());
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE")) {
                    const char* path = (const char*)payload->Data;
                    persistentSelectedObject->setTexture(path);
                    Console::Instance().Log("Texture assigned: " + std::string(path));
                }
                ImGui::EndDragDropTarget();
            }
        }
    }
    else {
        ImGui::Text("No GameObject selected.");
    }

    ImGui::End();
}

void MyGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ShowMainMenuBar();
    ShowHierarchy();
    renderInspector();
    ShowConsole();

    static bool showAssetsFolder = true;
    ShowAssetsFolder(&showAssetsFolder);

    // Manejar el "drop" de archivos en la pantalla
    if (ImGui::Begin("Main Viewport")) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE")) {
                const char* filePath = (const char*)payload->Data;
                Console::Instance().Log("Dropped file: " + std::string(filePath));
                // Aqu� puedes agregar la l�gica para manejar el archivo arrastrado y soltado
                SceneManager::LoadGameObject(filePath);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::handleEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_DROPFILE) {
        const char* droppedFilePath = event.drop.file;
        ImGui::SetDragDropPayload("TEXTURE_PATH", &droppedFilePath, sizeof(droppedFilePath));
        SDL_free((void*)droppedFilePath);
    }
}


void MyGUI::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}