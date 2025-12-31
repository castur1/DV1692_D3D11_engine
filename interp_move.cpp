#include "interp_move.hpp"
#include "transform.hpp"
#include "entity.hpp"
#include "logging.hpp"

InterpMove::InterpMove(
    Entity *owner,
    bool isActive,
    XMFLOAT3 startPosition,
    XMFLOAT3 startRotation,
    XMFLOAT3 startScale,
    XMFLOAT3 endPosition,
    XMFLOAT3 endRotation,
    XMFLOAT3 endScale,
    float moveDuration,
    float pauseDuration,
    Easing_function_type easingFunctionType,
    bool shouldPingPong,
    bool shouldignorePosition,
    bool shouldIgnoreRotation,
    bool shouldIgnoreScale
)   : Component(owner, isActive),
      startPosition(startPosition),
      startRotation(startRotation),
      startScale(startScale),
      endPosition(endPosition),
      endRotation(endRotation),
      endScale(endScale),
      moveDuration(moveDuration),
      pauseDuration(pauseDuration),
      easingFunctionType(easingFunctionType),
      shouldPingPong(shouldPingPong),
      shouldIgnorePosition(shouldignorePosition),
      shouldIgnoreRotation(shouldIgnoreRotation),
      shouldIgnoreScale(shouldIgnoreScale),
      moveTimer(0.0f),
      pauseTimer(0.0f),
      isReverse(false) {}

InterpMove::~InterpMove() {}

static XMFLOAT3 operator+(const XMFLOAT3 &a, const XMFLOAT3 &b) {
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

static XMFLOAT3 operator-(const XMFLOAT3 &a, const XMFLOAT3 &b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

static XMFLOAT3 operator*(const XMFLOAT3 &v, float s) {
    return {
        v.x * s,
        v.y * s,
        v.z * s
    };
}

void InterpMove::EaseHelper(float t) {
    Transform *transform = this->GetOwner()->GetComponent<Transform>();
    if (!transform) {
        LogInfo("Missing Transform in InterpMove\n");
        return;
    }

    if (!this->shouldIgnorePosition)
        transform->SetPosition(this->startPosition + (this->endPosition - this->startPosition) * t);

    if (!this->shouldIgnoreRotation)
        transform->SetRotation(this->startRotation + (this->endRotation - this->startRotation) * t);

    if (!this->shouldIgnoreScale)
        transform->SetScale(this->startScale + (this->endScale - this->startScale) * t);
}

void InterpMove::EaseLinear() {
    float t = this->moveTimer / this->moveDuration;

    t = t;

    this->EaseHelper(t);
}

void InterpMove::EaseInSine() {
    float t = this->moveTimer / this->moveDuration;

    t = 1.0f - cosf((t * XM_PI) / 2.0f);

    this->EaseHelper(t);
}

void InterpMove::EaseOutSine() {
    float t = this->moveTimer / this->moveDuration;

    t = sinf((t * XM_PI) / 2.0f);

    this->EaseHelper(t);
}

void InterpMove::EaseInOutSine() {
    float t = this->moveTimer / this->moveDuration;

    t = -(cosf(XM_PI * t) - 1.0f) / 2.0f;

    this->EaseHelper(t);
}

void InterpMove::UpdateTimersNormal(float deltaTime) {
    this->moveTimer += deltaTime;
    if (this->moveTimer >= this->moveDuration) {
        this->moveTimer = this->moveDuration;

        this->pauseTimer += deltaTime;
        if (this->pauseTimer >= this->pauseDuration) {
            this->pauseTimer = 0.0f;
            this->moveTimer = 0.0f;
        }
    }
}

void InterpMove::UpdateTimersPingPong(float deltaTime) {
    if (this->isReverse) {
        this->moveTimer -= deltaTime;
        if (this->moveTimer <= 0.0f) {
            this->moveTimer = 0.0f;

            this->pauseTimer += deltaTime;
            if (this->pauseTimer >= this->pauseDuration) {
                this->pauseTimer = 0.0f;
                this->isReverse = false;
            }
        }
    }
    else {
        this->moveTimer += deltaTime;
        if (this->moveTimer >= this->moveDuration) {
            this->moveTimer = this->moveDuration;

            this->pauseTimer += deltaTime;
            if (this->pauseTimer >= this->pauseDuration) {
                this->pauseTimer = 0.0f;
                this->isReverse = true;
            }
        }
    }
}

void InterpMove::Update(float deltaTime) {
    if (!this->isActive)
        return;

    if (this->shouldPingPong)
        this->UpdateTimersPingPong(deltaTime);
    else
        this->UpdateTimersNormal(deltaTime);

    switch (this->easingFunctionType) {
        case Easing_function_type::LINEAR: {
            this->EaseLinear();
        } break;
        case Easing_function_type::IN_SINE: {
            this->EaseInSine();
        } break;
        case Easing_function_type::OUT_SINE: {
            this->EaseOutSine();
        } break;
        case Easing_function_type::IN_OUT_SINE: {
            this->EaseInOutSine();
        } break;
    }
}

void InterpMove::Render(Renderer *renderer) {}

void InterpMove::SetShouldPingPong(bool shouldPingPong) {
    this->shouldPingPong = shouldPingPong;

    if (!shouldPingPong)
        this->isReverse = false;
}

bool InterpMove::GetShouldPingPong() const {
    return this->shouldPingPong;
}