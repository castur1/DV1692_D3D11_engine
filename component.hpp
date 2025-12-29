#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Renderer;
class Entity;

class Component {
    Entity *owner;

public:
    bool isActive;

    Component(Entity *owner, bool isActive);
    virtual ~Component();

    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer *renderer) = 0;

    Entity *GetOwner() const;
};

#endif