#ifndef INTERP_MOVE_HPP
#define INTERP_MOVE_HPP

#include <DirectXMath.h>
#include <cmath>

#include "component.hpp"

using namespace DirectX;

enum class Easing_function_type {
    LINEAR,
    IN_SINE,
    OUT_SINE,
    IN_OUT_SINE
};

class InterpMove : public Component {
    float moveTimer;
    float pauseTimer;

    bool shouldPingPong;
    bool isReverse;

    void EaseHelper(float t);
    void EaseLinear();
    void EaseInSine();
    void EaseOutSine();
    void EaseInOutSine();

    void UpdateTimersNormal(float deltaTime);
    void UpdateTimersPingPong(float deltaTime);

public:
    XMFLOAT3 startPosition;
    XMFLOAT3 startRotation;
    XMFLOAT3 startScale;

    XMFLOAT3 endPosition;
    XMFLOAT3 endRotation;
    XMFLOAT3 endScale;

    bool shouldIgnorePosition;
    bool shouldIgnoreRotation;
    bool shouldIgnoreScale;

    float moveDuration;
    float pauseDuration;

    Easing_function_type easingFunctionType;

    InterpMove(
        Entity *owner, 
        bool isActive,
        XMFLOAT3 startPosition,
        XMFLOAT3 startRotation,
        XMFLOAT3 startScale,
        XMFLOAT3 endPosition,
        XMFLOAT3 endRotation,
        XMFLOAT3 endScale,
        float moveDuration,
        float pauseDuration = 0.0f,
        Easing_function_type easingFunctionType = Easing_function_type::LINEAR,
        bool shouldPingPong = false,
        bool shouldignorePosition = false,
        bool shouldIgnoreRotation = false,
        bool shouldIgnoreScale = false
    );

    ~InterpMove();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;

    void SetShouldPingPong(bool shouldPingPong);
    bool GetShouldPingPong() const;
};

#endif