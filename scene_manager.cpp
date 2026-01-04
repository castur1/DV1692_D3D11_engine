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

    if (!this->currentScene) {
        LogError("No scene was set");
        return;
    }

    this->currentScene->Update(deltaTime);
}

void SceneManager::Render(Renderer *renderer) {
    if (!this->currentScene) {
        LogError("No scene was set");
        return;
    }

    this->currentScene->Render(renderer);
}