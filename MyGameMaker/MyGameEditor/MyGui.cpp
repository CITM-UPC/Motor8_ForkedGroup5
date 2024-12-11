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

enum class ViewMode {
    Console,
    AssetsFolder
};

ViewMode currentViewMode = ViewMode::Console;

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
        if (ImGui::BeginMenu("Control")) {
            if (ImGui::MenuItem("Start", NULL, is_running)) {
                is_running = true;
                is_paused = false;
                Console::Instance().Log("System started.");
                // Lógica para iniciar la funcionalidad
            }
            if (ImGui::MenuItem("Stop", NULL, !is_running)) {
                is_running = false;
                Console::Instance().Log("System stopped.");
                // Lógica para detener la funcionalidad
            }
            if (ImGui::MenuItem("Pause", NULL, is_paused)) {
                is_paused = !is_paused;
                if (is_paused) {
                    Console::Instance().Log("System paused.");
                }
                else {
                    Console::Instance().Log("System resumed.");
                }
                // Lógica para pausar/reanudar la funcionalidad
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void MyGUI::ShowMainWindow() {
    ImGui::SetNextWindowSize(ImVec2(180, 55), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, ImGui::GetIO().DisplaySize.y - 225), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f); // Establecer transparencia para la ventana principal
    ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus); // Añadir la bandera ImGuiWindowFlags_NoBringToFrontOnFocus

    if (ImGui::BeginTabBar("MainTabBar")) {
        if (ImGui::BeginTabItem("Console")) {
            currentViewMode = ViewMode::Console;
            ShowConsole();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Assets Folder")) {
            currentViewMode = ViewMode::AssetsFolder;
            ShowAssetsFolder(nullptr);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}


void MyGUI::ShowConsole() {
    if (currentViewMode != ViewMode::Console) return;

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
    if (currentViewMode != ViewMode::AssetsFolder) return;

    ImGui::SetNextWindowSize(ImVec2(680, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, ImGui::GetIO().DisplaySize.y - 200), ImGuiCond_Always);


    ImGui::Begin("Assets Folder", p_open);

    static std::vector<std::string> files;
    static bool filesLoaded = false;
    static int selectedFileIndex = -1;

    if (!filesLoaded) {
        std::string assetsPath = "Assets";
        for (const auto& entry : std::experimental::filesystem::directory_iterator(assetsPath)) {
            files.push_back(entry.path().filename().string());
        }
        filesLoaded = true;
    }

    if (ImGui::Button("Import File")) {
        const char* filterPatterns[1] = { "*" };
        const char* filePath = tinyfd_openFileDialog(
            "Select a file to import",
            "",
            1,
            filterPatterns,
            NULL,
            0
        );
        if (filePath) {
            std::experimental::filesystem::copy(filePath, "Assets/");
            files.push_back(std::experimental::filesystem::path(filePath).filename().string());
            Console::Instance().Log("Imported file: " + std::string(filePath));
        }
    }

    for (int i = 0; i < files.size(); ++i) {
        if (ImGui::Selectable(files[i].c_str(), selectedFileIndex == i)) {
            selectedFileIndex = i;
            Console::Instance().Log("Selected file: " + files[i]);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const char* filePath = files[i].c_str();
            ImGui::SetDragDropPayload("ASSET_FILE", filePath, strlen(filePath) + 1);
            ImGui::Text("Dragging %s", files[i].c_str());
            ImGui::EndDragDropSource();
        }
    }

    if (selectedFileIndex != -1 && ImGui::Button("Delete File")) {
        std::string filePath = "Assets/" + files[selectedFileIndex];
        if (std::experimental::filesystem::remove(filePath)) {
            Console::Instance().Log("Deleted file: " + files[selectedFileIndex]);
            files.erase(files.begin() + selectedFileIndex);
            selectedFileIndex = -1;
        }
        else {
            Console::Instance().Log("Failed to delete file: " + files[selectedFileIndex]);
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
         
            // Crear la entrada en la jerarquía
            if (ImGui::Selectable(go.getName().c_str(), isSelected)) {
                // Actualizar el objeto seleccionado en SceneManager
                SceneManager::selectedObject = &go;
            }

            // Comenzar el "drag" si se selecciona este objeto
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                GameObject* goPtr = &go; // Asegúrate de que estás pasando un puntero válido
                ImGui::SetDragDropPayload("GAMEOBJECT", &goPtr, sizeof(GameObject*)); // El payload debe contener el puntero
                ImGui::Text("Dragging %s", go.getName().c_str());
                ImGui::EndDragDropSource();
            }


            // Hacer que este objeto sea un "drop target"
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    IM_ASSERT(payload->DataSize == sizeof(GameObject*)); // Verifica el tamaño del payload
                    GameObject* draggedObject = *(GameObject**)payload->Data;

                    if (draggedObject == nullptr) {
                        std::cout << "Error: Dragged object is null!" << std::endl;
                    }
                    else {
                        std::cout << "Dragged object: " << draggedObject->getName() << std::endl;
                        // Aquí se maneja el emparentamiento
                        if (draggedObject != &go) {
                            draggedObject->setParent(&go);
                           // SceneManager::gameObjectsOnScene.erase(draggedObject);
                        }
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

        // Opciones adicionales
        if (ImGui::CollapsingHeader("Options")) {
            if (ImGui::Button("Delete")) {
                SceneManager::DeleteGameObject(persistentSelectedObject);
                persistentSelectedObject = nullptr;
                SceneManager::selectedObject = nullptr;
            }

            if (ImGui::Button("Reparent")) {
                ImGui::OpenPopup("Select Parent");
            }

            if (ImGui::BeginPopup("Select Parent")) {
                ImGui::Text("Select new parent for the selected object:");
                ImGui::Separator();

                for (auto& go : SceneManager::gameObjectsOnScene) {
                    if (ImGui::Selectable(go.getName().c_str())) {
                        if (SceneManager::selectedObject != &go) {
                            SceneManager::selectedObject->setParent(&go);
                            Console::Instance().Log("Reparented " + SceneManager::selectedObject->getName() + " to " + go.getName());
                        }
                        else {
                            Console::Instance().Log("Cannot reparent an object to itself.");
                        }
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }

            if (ImGui::Button("Create Empty")) {
                GameObject* newObject = SceneManager::CreateEmptyGameObject();
                newObject->setParent(persistentSelectedObject);
            }

            if (ImGui::Button("Create Children")) {
                GameObject* newChild = SceneManager::CreateEmptyGameObject();
                newChild->setParent(persistentSelectedObject);
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
    ShowMainWindow();

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