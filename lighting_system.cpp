#include "lighting_system.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include "scene.hpp"
#include "light_source.hpp"
#include "transform.hpp"

LightingSystem::LightingSystem(Entity *owner, bool isActive, Renderer *renderer)
    : Component(owner, isActive), 
      renderer(renderer), 
      ambientColour({0.15f, 0.15f, 0.2f}) {}

LightingSystem::~LightingSystem() {}

void LightingSystem::Update(float deltaTime) {
    std::vector<Light_source_data> lightSourceData;

    std::vector<Entity *> entities = this->GetOwner()->GetScene()->GetEntities();
    for (auto &entity : entities) {
        if (!entity->isActive)
            continue;

        LightSource *lightSource = entity->GetComponent<LightSource>();
        if (!lightSource || !lightSource->isActive)
            continue;

        Transform *transform = entity->GetComponent<Transform>();
        if (!transform || !transform->isActive)
            continue;

        Light_source_data data{};

        data.type = (int)lightSource->type;
        data.colour = lightSource->colour;
        data.intensity = lightSource->intensity;

        switch (lightSource->type) {
            case Light_source_type::DIRECTIONAL: {
                XMFLOAT3 rotation = transform->GetRotation();
                XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
                XMVECTOR forward = XMVector3Normalize(rotationMatrix.r[2]);
                XMStoreFloat3(&data.direction, forward);
            } break;
            case Light_source_type::POINT: {
                data.position = transform->GetPosition();
            } break;
            case Light_source_type::SPOT: {
                data.position = transform->GetPosition();

                XMFLOAT3 rotation = transform->GetRotation();
                XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
                XMVECTOR forward = XMVector3Normalize(rotationMatrix.r[2]);
                XMStoreFloat3(&data.direction, forward);

                data.spotLightCosHalfAngle = cosf(lightSource->spotLightAngle / 2.0f);
            } break;
        }

        lightSourceData.push_back(data);
        if (lightSourceData.size() > MAX_LIGHTS)
            break;
    }

    this->renderer->SetLightSourceData(lightSourceData);
    this->renderer->SetAmbientLightColour(this->ambientColour);
}

void LightingSystem::Render(Renderer *renderer) {}