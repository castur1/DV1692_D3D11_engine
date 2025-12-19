#include "mesh_renderer.hpp"
#include "renderer.hpp"
#include "transform.hpp"
#include "entity.hpp"
#include "logging.hpp"

MeshRenderer::MeshRenderer(Entity *owner, bool isActive, MeshData *mesh, Material *material) 
    : Component(owner, isActive), mesh(mesh), material(material) {}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::Update(float deltaTime) {}

void MeshRenderer::Render(Renderer *renderer) {
    if (!this->isActive)
        return;

    Transform *transform = this->GetOwner()->GetComponent<Transform>();
    if (transform == nullptr) {
        LogError("Missing Transform component on owner");
        return;
    }

    Draw_command command{};
    command.mesh = this->mesh;
    command.material = this->material;
    XMStoreFloat4x4(&command.worldMatrix, XMMatrixTranspose(transform->GetWorldMatrix()));
    renderer->Submit(command);
}