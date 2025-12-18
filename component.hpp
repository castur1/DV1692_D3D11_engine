#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Renderer;
class Entity;

class Component {
    Entity *owner;

public:
    Component(Entity *owner);
    ~Component();

    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer *renderer) = 0;

    Entity *GetOwner() const;
};

#endif