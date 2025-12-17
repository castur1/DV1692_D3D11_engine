#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Renderer;

class Component {


public:
    Component();
    ~Component();

    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer *renderer) = 0;
};

#endif