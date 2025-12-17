#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <vector>
#include <memory>

class Renderer;
class Component;

class Entity {
    std::vector<std::unique_ptr<Component>> components;

public:
    Entity();
    ~Entity();

    void Update(float deltaTime);
    void Render(Renderer *renderer);
};

#endif