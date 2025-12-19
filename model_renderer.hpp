#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include <vector>

#include "component.hpp"
#include "model.hpp"

class ModelRenderer : public Component {
    Model *model; // TODO: ModelPtr?
    std::vector<Material *> materials;

public:
    ModelRenderer(Entity *owner, bool isActive, Model *model, const std::vector<Material *> &materials = {});
    ~ModelRenderer();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif