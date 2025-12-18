#include "scene.hpp"
#include "entity.hpp"

Scene::Scene() {}

Scene::~Scene() {}

Entity *Scene::AddEntity() {
    this->entities.emplace_back(std::make_unique<Entity>(this));
    return this->entities.back().get();
}

void Scene::Update(float deltaTime) {
    for (auto &entity : this->entities)
        entity->Update(deltaTime);
}

void Scene::Render(Renderer *renderer) {
    for (auto &entity : this->entities)
        entity->Render(renderer);
}