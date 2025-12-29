#ifndef LIGHTING_SYSTEM_HPP
#define LIGHTING_SYSTEM_HPP

#include <DirectXMath.h>

#include "component.hpp"

using namespace DirectX;

class Renderer;

class LightingSystem : public Component {
    Renderer *renderer;

public:
    XMFLOAT3 ambientColour;

    LightingSystem(Entity *owner, bool isActive, Renderer *renderer);
    ~LightingSystem();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif