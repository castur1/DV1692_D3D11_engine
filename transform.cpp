#include "transform.hpp"

Transform::Transform(Entity *owner, const XMFLOAT3 &position, const XMFLOAT3 &rotation, const XMFLOAT3 &scale)
    : Component(owner), position(position), rotation(rotation), scale(scale) {}

Transform::Transform(Entity *owner) 
    : Transform(owner, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}) {}

Transform::~Transform() {}

void Transform::Update(float deltaTime) {}
void Transform::Render(Renderer *renderer) {}

XMMATRIX Transform::GetWorldMatrix() const {
    XMMATRIX scaleMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
    XMMATRIX translationMatrix = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

    return scaleMatrix * rotationMatrix * translationMatrix; // p' = pABC because row-major
}

void Transform::SetPosition(const XMFLOAT3 &position) {
    this->position = position;
}

void Transform::SetRotation(const XMFLOAT3 &rotation) {
    this->rotation = rotation;
}

void Transform::SetScale(const XMFLOAT3 &scale) {
    this->scale = scale;
}

XMFLOAT3 Transform::GetPosition() const {
    return this->position;
}

XMFLOAT3 Transform::GetRotation() const {
    return this->rotation;
}

XMFLOAT3 Transform::GetScale() const {
    return this->scale;
}
