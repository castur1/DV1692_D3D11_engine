#ifndef MESH_HPP
#define MESH_HPP

#include "component.hpp"

class Mesh : public Component {


public:
    Mesh();
    ~Mesh();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif