#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include "component.hpp"

class MeshData;
class Material;

class MeshRenderer : public Component {
    MeshData *mesh;
    Material *material;

public:
    MeshRenderer(MeshData *mesh, Material *material);
    ~MeshRenderer();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif