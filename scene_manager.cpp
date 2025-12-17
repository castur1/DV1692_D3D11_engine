#include "scene_manager.hpp"
#include "scene.hpp"
#include "entity.hpp"
#include "all_components.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh_data.hpp"
#include "renderer.hpp"

SceneManager::SceneManager() : currentScene(nullptr) {

}

SceneManager::~SceneManager() {}

Scene *SceneManager::AddBlankScene(const std::string &name) {
    auto result = this->scenes.emplace(name, std::make_unique<Scene>());
    return result.first->second.get();
}

void SceneManager::CreateScenes(Renderer *renderer) {
    Scene *scene = this->AddBlankScene("demo_0");

    // CONTINUE HERE! Change shader vector4 to vector3!

    {
        std::vector<Vertex> vertices = {
            { { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
        };

        std::vector<UINT> indices = {
            0, 1, 2,
            0, 2, 3
        };

        auto meshData = std::make_unique<MeshData>(renderer->GetDevice(), vertices, indices);
        MeshData *meshDataPtr = meshData.get();
        this->meshes.push_back(std::move(meshData));

        auto material = std::make_unique<Material>(renderer->GetDevice(), L"vertex_shader.cso", L"pixel_shader.cso");
        Material *materialPtr = material.get();
        this->materials.push_back(std::move(material));

        Entity *entity = scene->AddEntity();
        entity->AddComponent<MeshRenderer>(meshDataPtr, materialPtr);
    }

    {
        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { {  -0.8f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { {  -0.8f, -1.0, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
            { { -1.0f, -1.0, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
        };

        std::vector<UINT> indices = {
            0, 1, 2,
            0, 2, 3
        };

        auto meshData = std::make_unique<MeshData>(renderer->GetDevice(), vertices, indices);
        MeshData *meshDataPtr = meshData.get();
        this->meshes.push_back(std::move(meshData));

        auto material = std::make_unique<Material>(renderer->GetDevice(), L"vertex_shader.cso", L"pixel_shader.cso");
        Material *materialPtr = material.get();
        this->materials.push_back(std::move(material));

        Entity *entity = scene->AddEntity();
        entity->AddComponent<MeshRenderer>(meshDataPtr, materialPtr);
    }

}

void SceneManager::ChangeScene(const std::string &name) {
    this->currentSceneName = name;

    auto iter = this->scenes.find(name);
    if (iter == this->scenes.end()) {
        LogInfo("Scene '%s' does not exist.\n", name.c_str());
        return;
    }

    this->currentScene = iter->second.get();
}

void SceneManager::RequestSceneChange(const std::string &name) {
    this->targetSceneName = name;
}

void SceneManager::Initialize(Renderer *renderer) {
    this->CreateScenes(renderer);
    this->RequestSceneChange("demo_0");
}

void SceneManager::Update(float deltaTime) {
    if (this->targetSceneName != this->currentSceneName)
        this->ChangeScene(this->targetSceneName);

    if (this->currentScene == nullptr) {
        LogError("No scene was set");
        return;
    }

    this->currentScene->Update(deltaTime);
}

void SceneManager::Render(Renderer *renderer) {
    if (this->currentScene == nullptr) {
        LogError("No scene was set");
        return;
    }

    this->currentScene->Render(renderer);
}