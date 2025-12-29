#include "scene_manager.hpp"
#include "scene.hpp"
#include "entity.hpp"
#include "all_components.hpp"
#include "logging.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "asset_manager.hpp"

SceneManager::SceneManager() : currentScene(nullptr) {}

SceneManager::~SceneManager() {}

Scene *SceneManager::AddBlankScene(const std::string &name) {
    auto result = this->scenes.emplace(name, std::make_unique<Scene>());
    return result.first->second.get();
}

void SceneManager::CreateScenes(Renderer *renderer, AssetManager *assetManager) {
    Scene *scene = this->AddBlankScene("demo_0");

    ModelPtr model = assetManager->LoadModel("models/apple/apple.obj");

    float r2d = XM_PI / 180.0f;

    Entity *entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, -90 * r2d, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f));
    entity->AddComponent<ModelRenderer>(model);

    model = assetManager->LoadModel("models/suzanne/Suzanne.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(3.0f, 1.0f, -2.0f), XMFLOAT3(0.0f, -45 * r2d, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<ModelRenderer>(model);

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(-3.0f, 1.0f, -2.0f), XMFLOAT3(0.0f, -135 * r2d, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<ModelRenderer>(model);

    model = assetManager->LoadModel("models/quad/quad.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>()->SetScale({20.0f, 1.0f, 20.0f});
    entity->AddComponent<ModelRenderer>(model);

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(0.0f, 2.0f, -8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<CameraController>(renderer);
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

void SceneManager::Initialize(Renderer *renderer, AssetManager *assetManager) {
    this->CreateScenes(renderer, assetManager);
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