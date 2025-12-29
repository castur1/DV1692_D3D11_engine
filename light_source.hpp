#ifndef LIGHT_SOURCE_HPP
#define LIGHT_SOURCE_HPP

#include <DirectXMath.h>

#include "component.hpp"

using namespace DirectX;

enum class Light_source_type {
    DIRECTIONAL,
    POINT,
    SPOT
};

class LightSource : public Component {
public:
    Light_source_type type;

    XMFLOAT3 colour;
    float intensity;

    float spotLightAngle;

    LightSource(Entity *owner, bool isActive, Light_source_type type, const XMFLOAT3 &colour, float intensity);
    ~LightSource();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif