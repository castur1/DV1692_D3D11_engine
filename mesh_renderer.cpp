#include "mesh_renderer.hpp"
#include "renderer.hpp"

MeshRenderer::MeshRenderer(Entity *owner, MeshData *mesh, Material *material) 
    : Component(owner), mesh(mesh), material(material) {}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::Update(float deltaTime) {}

void MeshRenderer::Render(Renderer *renderer) {
    renderer->Submit({this->mesh, this->material});
}