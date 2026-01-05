#include "scene_manager.hpp"
#include "scene.hpp"
#include "entity.hpp"
#include "all_components.hpp"
#include "logging.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "asset_manager.hpp"

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

    ModelPtr model = assetManager->LoadModel("models/apple/apple.obj");

    MaterialPtr appleMaterial1 = assetManager->CreateDefaultMaterialCopy("apple1");
    appleMaterial1->diffuseColour = {1.0f, 0.0f, 0.0f};

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(8.0f, 3.0f, 8.0f), XMFLOAT3(XM_PIDIV4, 5 * XM_PIDIV4, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f));
    entity->AddComponent<LightSource>(Light_source_type::SPOT, XMFLOAT3(1.0f, 0.0f, 0.0f), 12.0f);
    entity->AddComponent<ModelRenderer>(model, std::vector<MaterialPtr>{appleMaterial1});

    MaterialPtr appleMaterial2 = assetManager->CreateDefaultMaterialCopy("apple2");
    appleMaterial2->diffuseColour = {0.0f, 0.0f, 1.0f};

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(0.0f, 1.0f, 3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f));
    entity->AddComponent<LightSource>(Light_source_type::POINT, XMFLOAT3(0.0f, 0.0f, 1.0f), 5.0f);
    entity->AddComponent<ModelRenderer>(model, std::vector<MaterialPtr>{appleMaterial2});

    MaterialPtr appleMaterial3 = assetManager->CreateDefaultMaterialCopy("apple3");
    appleMaterial3->diffuseColour = {1.0f, 0.0f, 1.0f};

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(-2.0f, 2.0f, 7.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f));
    entity->AddComponent<LightSource>(Light_source_type::POINT, XMFLOAT3(1.0f, 0.0f, 1.0f), 5.0f);
    entity->AddComponent<ModelRenderer>(model, std::vector<MaterialPtr>{appleMaterial3});
    entity->AddComponent<InterpMove>(
        XMFLOAT3(-2.0f, 2.0f, 7.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(-6.0f, 3.0f, 2.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        4.0f,
        1.0f,
        Easing_function_type::IN_OUT_SINE,
        true,
        false,
        true,
        true
    );

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
    entity->AddComponent<InterpMove>(
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.0f, 1.0f, 1.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(2.0f, 1.0f, 3.0f),
        3.0f,
        1.0f,
        Easing_function_type::IN_OUT_SINE,
        true,
        true,
        true,
        false
    );

    model = assetManager->LoadModel("models/stanford_dragon/stanford_dragon.obj");

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(-6.0f, 0.0f, 6.0f), XMFLOAT3(0.0f, -XM_PIDIV4, 0.0f), XMFLOAT3(0.05f, 0.05f, 0.05f));
    entity->AddComponent<ModelRenderer>(model);

    model = assetManager->LoadModel("models/quad/quad.obj");
    MaterialPtr grassMaterial = assetManager->CreateDefaultMaterialCopy("grass");
    grassMaterial->diffuseTexture = assetManager->LoadTexture("textures/Grass004_1K-JPG_Color.jpg");
    grassMaterial->specularColour = {0.2f, 0.3f, 0.1f};
    grassMaterial->specularExponent = 200.0f;

    entity = scene->AddEntity();
    entity->AddComponent<Transform>()->SetScale({20.0f, 1.0f, 20.0f});
    entity->AddComponent<ModelRenderer>(model, std::vector<MaterialPtr>{grassMaterial});

    entity = scene->AddEntity();
    entity->AddComponent<Transform>(XMFLOAT3(0.0f, 2.0f, -8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    entity->AddComponent<CameraController>(renderer);
}