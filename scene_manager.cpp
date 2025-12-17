#include "scene_manager.hpp"
#include "scene.hpp"
#include "logging.hpp"

SceneManager::SceneManager() : currentScene(nullptr) {
    this->CreateScenes();
    this->RequestSceneChange("demo_0");
}

SceneManager::~SceneManager() {}

Scene *SceneManager::AddBlankScene(const std::string &name) {
    auto result = this->scenes.emplace(name, std::make_unique<Scene>());
    return result.first->second.get();
}

void SceneManager::CreateScenes() {
    Scene *scene = this->AddBlankScene("demo_0");
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