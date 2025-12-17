#include "mesh_renderer.hpp"
#include "renderer.hpp"

MeshRenderer::MeshRenderer(MeshData *mesh, Material *material) 
    : Component(), mesh(mesh), material(material) {}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::Update(float deltaTime) {}

void MeshRenderer::Render(Renderer *renderer) {
    renderer->Submit({this->mesh, this->material});
}