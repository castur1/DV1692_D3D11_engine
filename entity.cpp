#include "entity.hpp"
#include "component.hpp"

Entity::Entity(Scene *scene) : scene(scene) {}

Entity::~Entity() {}

void Entity::Update(float deltaTime) {
    for (auto &component : this->components)
        component->Update(deltaTime);
}

void Entity::Render(Renderer *renderer) {
    for (auto &component : this->components)
        component->Render(renderer);
}