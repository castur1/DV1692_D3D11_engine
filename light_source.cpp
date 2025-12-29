#include "light_source.hpp"

LightSource::LightSource(Entity *owner, bool isActive, Light_source_type type, const XMFLOAT3 &colour, float intensity) 
    : Component(owner, isActive),
      type(type),
      colour(colour),
      intensity(intensity),
      spotLightAngle(XM_PIDIV4) {}

LightSource::~LightSource() {}

void LightSource::Update(float deltaTime) {}

void LightSource::Render(Renderer *renderer) {}