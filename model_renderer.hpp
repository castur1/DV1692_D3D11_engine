#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include <vector>

#include "component.hpp"
#include "model.hpp"

class ModelRenderer : public Component {
    ModelPtr model;
    std::vector<MaterialPtr> materials;

public:
    ModelRenderer(Entity *owner, bool isActive, Model *model, const std::vector<MaterialPtr> &materials = {});
    ~ModelRenderer();

    void Update(float deltaTime) override;
    void Render(Renderer *renderer) override;
};

#endif