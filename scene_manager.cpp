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

    renderer->clearColour[0] = 0.5f;
    renderer->clearColour[1] = 0.8f;
    renderer->clearColour[2] = 0.9f;

    Entity *entity = scene->AddEntity();
    entity->AddComponent<LightingSystem>(renderer)->ambientColour = {0.05f, 0.08f, 0.09f};

    entity = scene->AddEntity();
    entity->AddComponent<Transform>()->SetRotation({XM_PIDIV4, XM_PIDIV4, 0.0f});
    entity->AddComponent<LightSource>(Light_source_type::DIRECTIONAL, XMFLOAT3(1.0f, 0.9f, 0.5f), 1.5f);

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(9.0f, 3.0f, 9.0f), XMFLOAT3(XM_PIDIV4, 5 * XM_PIDIV4, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<LightSource>(Light_source_type::SPOT, XMFLOAT3(1.0f, 0.0f, 0.0f), 15.0f);

    entity = scene->AddEntity();
    entity->AddComponent<Transform>()->SetPosition({7.0f, 0.5f, 5.0f});
    entity->AddComponent<LightSource>(Light_source_type::POINT, XMFLOAT3(0.0f, 0.0f, 1.0f), 8.0f);

    ModelPtr model = assetManager->LoadModel("models/apple/apple.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(0.0f, -0.15f, 0.0f), XMFLOAT3(0.0f, XM_PIDIV4, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f));
    entity->AddComponent<ModelRenderer>(model);

    model = assetManager->LoadModel("models/suzanne/Suzanne.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>();
    entity->AddComponent<ModelRenderer>(model);
    entity->AddComponent<InterpMove>(
        XMFLOAT3(3.0f, -0.15f, -2.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(3.0f, 1.0f, -2.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        2.0f,
        0.0f,
        Easing_function_type::IN_OUT_SINE,
        true,
        false,
        true,
        true
    );
    entity->AddComponent<InterpMove>(
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, XM_2PI, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        4.5f,
        0.0f,
        Easing_function_type::LINEAR,
        false,
        true,
        false,
        true
    );

    entity = scene->AddEntity();
    entity->AddComponent<Transform>();
    entity->AddComponent<ModelRenderer>(model);
    entity->AddComponent<InterpMove>(
        XMFLOAT3(-3.0f, -0.15f, -2.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(-3.0f, 1.0f, -2.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        2.0f,
        0.0f,
        Easing_function_type::IN_OUT_SINE,
        true,
        false,
        true,
        true
    );
    entity->AddComponent<InterpMove>(
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, XM_2PI, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        4.5f,
        0.0f,
        Easing_function_type::LINEAR,
        false,
        true,
        false,
        true
    );

    model = assetManager->LoadModel("models/spot/Spot.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(6.0f, 0.0f, 6.0f), XMFLOAT3(0.0f, -XM_PIDIV4, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<ModelRenderer>(model);

    model = assetManager->LoadModel("models/quad/quad.obj");
    MaterialPtr grassMaterial = assetManager->CreateDefaultMaterialCopy("grass");
    grassMaterial->diffuseTexture = assetManager->LoadTexture("textures/Grass004_1K-JPG_Color.jpg");
    grassMaterial->specularColour = {0.2f, 0.3f, 0.1f};
    grassMaterial->shininess = 200.0f;

    entity = scene->AddEntity();
    entity->AddComponent<Transform>()->SetScale({20.0f, 1.0f, 20.0f});
    entity->AddComponent<ModelRenderer>(model, std::vector<MaterialPtr>{grassMaterial});

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