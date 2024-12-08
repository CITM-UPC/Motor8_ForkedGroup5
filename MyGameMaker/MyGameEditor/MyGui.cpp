#include "MyGUI.h"
#include "MyGameEngine/GameObject.h"
#include "SceneManager.h"
#include "BasicShapesManager.h"
#include "SystemInfo.h"
#include "Console.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs/tinyfiledialogs.h> 

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
            if (ImGui::BeginMenu("Import")) {
                if (ImGui::MenuItem("FBX")) {
                    const char* filterPatterns[1] = { "*.fbx" };
                    const char* filePath = tinyfd_openFileDialog(
                        "Select an FBX file",
                        "",
                        1,
                        filterPatterns,
                        NULL,
                        0
                    );
                    if (filePath) {
                        SceneManager::LoadGameObject(filePath);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Quit")) {
                SDL_Quit();
                exit(0);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mesh")) {
            ImGui::Checkbox("Mesh Creator", &show_spawn_figures_window);
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

            // Crear la entrada en la jerarquía
            ImGui::Selectable(go.getName().c_str(), isSelected);

            // Comenzar el "drag" si se selecciona este objeto
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                // Cambiar para usar un puntero válido
                ImGui::SetDragDropPayload("GAMEOBJECT", &go, sizeof(GameObject*));
                ImGui::Text("Dragging %s", go.getName().c_str()); // Ajusta si cambias a puntero
                ImGui::EndDragDropSource();
            }

            // Hacer que este objeto sea un "drop target"
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    // Verificar tamaño del payload
                    IM_ASSERT(payload->DataSize == sizeof(GameObject*));

                    // Recuperar el objeto arrastrado
                    GameObject* draggedObject = *(GameObject**)payload->Data;

                    // Logs para confirmar que los datos son válidos
                    if (draggedObject != nullptr) {
                        std::cout << "Dropped " << draggedObject->getName()
                            << " onto " << go.getName() << std::endl;

                        // Establecer relación de jerarquía
                        if (draggedObject != &go) { // Prevenir auto-emparentamiento
                            draggedObject->setParent(&go);
                        }
                    }
                    else {
                        std::cerr << "Error: Dragged object is null!" << std::endl;
                    }
                }

                ImGui::EndDragDropTarget();
            }

            // Añadir un desplazamiento visual para mostrar jerarquías
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

        if (persistentSelectedObject->hasMesh() && ImGui::CollapsingHeader("Mesh")) {
            Mesh& mesh = persistentSelectedObject->mesh();

            static bool showNormalsPerTriangle = false;
            static bool showNormalsPerFace = false;

            ImGui::Checkbox("Show Normals (Per Triangle)", &showNormalsPerTriangle);
            ImGui::Checkbox("Show Normals (Per Face)", &showNormalsPerFace);
            if (showNormalsPerTriangle) {
                persistentSelectedObject->mesh().drawNormals(persistentSelectedObject->transform().mat());
            }
            if (showNormalsPerFace) {
                persistentSelectedObject->mesh().drawNormalsPerFace(persistentSelectedObject->transform().mat());
            }
        }
        if (persistentSelectedObject->hasTexture() && ImGui::CollapsingHeader("Texture")) {
            Texture& texture = persistentSelectedObject->texture();
            static bool showCheckerTexture = false;
            ImGui::Text("Width: %d", texture.image().width());
            ImGui::Text("Height: %d", texture.image().height());

            if (ImGui::Button("Toggle Checker Texture")) {
                showCheckerTexture = !showCheckerTexture;
                persistentSelectedObject->hasCheckerTexture = showCheckerTexture;
            }

            // Drag and Drop para añadir una textura
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_PATH")) {
                    IM_ASSERT(payload->DataSize == sizeof(const char*));
                    const char* path = *(const char**)payload->Data;
                    persistentSelectedObject->setTexture(path);
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