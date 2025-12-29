#include "scene.hpp"
#include "entity.hpp"
#include "light_source.hpp"
#include "renderer.hpp"
#include "transform.hpp"

Scene::Scene() {}

Scene::~Scene() {}

void Scene::Update(float deltaTime) {
    for (auto &entity : this->entities)
        entity->Update(deltaTime);
}

void Scene::Render(Renderer *renderer) {
    for (auto &entity : this->entities)
        entity->Render(renderer);
}

Entity *Scene::AddEntity(bool isActive) {
    this->entities.emplace_back(std::make_unique<Entity>(this, isActive));
    return this->entities.back().get();
}

std::vector<Entity *> Scene::GetEntities() {
    std::vector<Entity *> result;
    result.reserve(this->entities.size());

    for (auto &entity : this->entities)
        result.push_back(entity.get());

    return result;
}