#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <vector>
#include <memory>

class Renderer;
class Component;
class Scene;

class Entity {
    Scene *scene;

    std::vector<std::unique_ptr<Component>> components;

public:
    Entity(Scene *scene);
    ~Entity();

    template<typename T, typename... Args>
    T *AddComponent( Args&&... args) {
        this->components.emplace_back(std::make_unique<T>(this, std::forward<Args>(args)...));
        return static_cast<T *>(this->components.back().get());
    }

    template<typename T>
    T *GetComponent() {
        for (auto &component : this->components) {
            T *ptr = dynamic_cast<T *>(component.get());
            if (ptr != nullptr)
                return ptr;
        }

        return nullptr;
    }

    void Update(float deltaTime);
    void Render(Renderer *renderer);
};

#endif