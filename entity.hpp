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

    template<typename T, typename... Args>
    T *AddComponent(Args&&... args) {
        this->components.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T *>(this->components.back().get());
    }

    void Update(float deltaTime);
    void Render(Renderer *renderer);
};

#endif