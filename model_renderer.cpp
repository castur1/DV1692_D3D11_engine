#include "model_renderer.hpp"
#include "renderer.hpp"
#include "transform.hpp"
#include "entity.hpp"
#include "logging.hpp"

ModelRenderer::ModelRenderer(Entity *owner, bool isActive, Model *model, const std::vector<Material *> &materials) 
    : Component(owner, isActive), model(model), materials(materials) {
    this->materials.resize(model->subModels.size());
}

ModelRenderer::~ModelRenderer() {}

void ModelRenderer::Update(float deltaTime) {}

void ModelRenderer::Render(Renderer *renderer) {
    if (!this->isActive)
        return;

    Transform *transform = this->GetOwner()->GetComponent<Transform>();
    if (transform == nullptr) {
        LogError("Missing Transform component on owner");
        return;
    }

    for (int i = 0; i < this->model->subModels.size(); ++i) {
        const auto &subModel = this->model->subModels[i];

        Material *material = this->materials[i];
        if (material == nullptr)
            material = subModel.material;

        Draw_command command{};

        command.vertexBuffer = this->model->vertexBuffer;
        command.indexBuffer = this->model->indexBuffer;

        command.indexCount = subModel.mesh.indexCount;
        command.startIndex = subModel.mesh.startIndex;
        command.baseVertex = subModel.mesh.baseVertex;

        command.material = material;

        XMStoreFloat4x4(&command.worldMatrix, XMMatrixTranspose(transform->GetWorldMatrix()));

        renderer->Submit(command);
    }
}