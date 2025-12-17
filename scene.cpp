#include "scene.hpp"
#include "entity.hpp"

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