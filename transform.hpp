#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <DirectXMath.h>

#include "component.hpp"

using namespace DirectX;

class Transform : public Component {
    XMFLOAT3 position;
    XMFLOAT3 rotation;
    XMFLOAT3 scale;

public:
    Transform(Entity *owner, bool isActive, const XMFLOAT3 &position, const XMFLOAT3 &rotation, const XMFLOAT3 &scale);
    Transform(Entity *owner, bool isActive);
    ~Transform();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;

    XMMATRIX GetWorldMatrix() const;

    void SetPosition(const XMFLOAT3 &position);
    void SetRotation(const XMFLOAT3 &rotation);
    void SetScale(const XMFLOAT3 &scale);

    XMFLOAT3 GetPosition() const;
    XMFLOAT3 GetRotation() const;
    XMFLOAT3 GetScale() const;
};

#endif