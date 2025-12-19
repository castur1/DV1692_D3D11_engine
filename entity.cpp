#include "entity.hpp"
#include "component.hpp"

Entity::Entity(Scene *scene, bool isActive) : scene(scene), isActive(isActive) {}

Entity::~Entity() {}

void Entity::Update(float deltaTime) {
    if (!this->isActive)
        return;

    for (auto &component : this->components)
        component->Update(deltaTime);
}

void Entity::Render(Renderer *renderer) {
    if (!this->isActive)
        return;

    for (auto &component : this->components)
        component->Render(renderer);
}