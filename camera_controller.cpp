#include "camera_controller.hpp"
#include "transform.hpp"
#include "entity.hpp"
#include "logging.hpp"
#include "input.hpp"
#include "renderer.hpp"

CameraController::CameraController(Entity *owner, bool isActive, Renderer *renderer, float moveSpeed, 
    float rotationSpeed, float fov, float aspectRatio, float nearPlane, float farPlane)
    : Component(owner, isActive), 
      renderer(renderer), 
      moveSpeed(moveSpeed), 
      rotationSpeed(rotationSpeed),
      fieldOfView(fov), 
      aspectRatio(aspectRatio), 
      nearPlane(nearPlane), 
      farPlane(farPlane) {
    this->SetProjection(fov, aspectRatio, nearPlane, farPlane);
}

CameraController::~CameraController() {}

void CameraController::Update(float deltaTime) {
    if (!this->isActive)
        return;

    Transform *transform = this->GetOwner()->GetComponent<Transform>();
    if (!transform) {
        LogError("Missing Transform component on owner");
        return;
    }

    XMFLOAT3 position = transform->GetPosition();
    XMFLOAT3 rotation = transform->GetRotation();

    float &pitch = rotation.x;
    float &yaw = rotation.y;
    float &roll = rotation.z;

    float rotationSpeed = this->rotationSpeed * deltaTime;

    if (Input::IsKeyDown(VK_RIGHT))
        yaw += rotationSpeed;
    if (Input::IsKeyDown(VK_LEFT))
        yaw -= rotationSpeed;
    if (Input::IsKeyDown(VK_UP))
        pitch -= rotationSpeed;
    if (Input::IsKeyDown(VK_DOWN))
        pitch += rotationSpeed;

    pitch = XMMax(-XM_PIDIV2 + 0.001f, XMMin(XM_PIDIV2 - 0.001f, pitch));

    while (yaw >= XM_2PI)
        yaw -= XM_2PI;
    while (yaw < 0.0f)
        yaw += XM_2PI;

    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll); // Basis vectors form the rows!

    XMVECTOR right = rotationMatrix.r[0];
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR forward = XMVector3Normalize(XMVector3Cross(right, up));

    XMVECTOR positionVector = XMLoadFloat3(&position);

    float moveSpeed = this->moveSpeed * deltaTime;
    if (Input::IsKeyDown('X'))
        moveSpeed *= 3.0f;

    if (Input::IsKeyDown('W'))
        positionVector = XMVectorAdd(positionVector, XMVectorScale(forward, moveSpeed));
    if (Input::IsKeyDown('S'))
        positionVector = XMVectorSubtract(positionVector, XMVectorScale(forward, moveSpeed));
    if (Input::IsKeyDown('D'))
        positionVector = XMVectorAdd(positionVector, XMVectorScale(right, moveSpeed));
    if (Input::IsKeyDown('A'))
        positionVector = XMVectorSubtract(positionVector, XMVectorScale(right, moveSpeed));
    if (Input::IsKeyDown(VK_SPACE))
        positionVector = XMVectorAdd(positionVector, XMVectorScale(up, moveSpeed));
    if (Input::IsKeyDown(VK_LSHIFT))
        positionVector = XMVectorSubtract(positionVector, XMVectorScale(up, moveSpeed));

    XMStoreFloat3(&position, positionVector);

    transform->SetPosition(position);
    transform->SetRotation(rotation);

    forward = rotationMatrix.r[2];
    XMMATRIX viewMatrix = XMMatrixLookToLH(positionVector, forward, up);

    this->renderer->SetViewMatrix(viewMatrix);
    this->renderer->SetCameraPosition(position);
}

void CameraController::Render(Renderer *renderer) {}

void CameraController::SetProjection(float fieldOfView, float aspectRatio, float nearPlane, float farPlane) {
    this->fieldOfView = fieldOfView;
    this->aspectRatio = aspectRatio;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;

    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(this->fieldOfView, this->aspectRatio, this->nearPlane, this->farPlane);

    this->renderer->SetProjectionMatrix(projectionMatrix);
}