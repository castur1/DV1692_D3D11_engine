#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>

class Renderer;
class Entity;

class Scene {
    std::vector<std::unique_ptr<Entity>> entities;

public:
    Scene();
    ~Scene();

    void Update(float deltaTime);
    void Render(Renderer *renderer);

    Entity *AddEntity();
};

#endif