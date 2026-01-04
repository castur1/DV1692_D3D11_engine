#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <DirectXMath.h>

#include "component.hpp"

using namespace DirectX;

class Renderer;

class CameraController : public Component {
    Renderer *renderer;

    float fieldOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;

public:
    float moveSpeed;
    float rotationSpeed;
    float mouseSensitivity;
    bool useMouseInput;

    CameraController(
        Entity *owner, 
        bool isActive,
        Renderer *renderer, 
        float moveSpeed        = 8.0f, 
        float rotationSpeed    = 1.5f,
        float mouseSensitivity = 0.0015f,
        float fov              = XM_PIDIV4, 
        float aspectRatio      = 16.0f / 9.0f, 
        float nearPlane        = 0.1f, 
        float farPlane         = 100.0f
    );
    ~CameraController();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;

    void SetProjection(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
};

#endif