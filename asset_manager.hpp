#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <d3d11.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "model.hpp"

using ModelPtr = std::shared_ptr<Model>;
using MaterialPtr = std::shared_ptr<Material>;

class AssetManager {
    ID3D11Device *device;

    std::unordered_map<std::string, ModelPtr> modelCache;
    std::unordered_map<std::string, MaterialPtr> materialCache;

    MaterialPtr defaultMaterial;

    std::string assetDir;

public:
    Pipeline_state pipelineStateDefault;

    AssetManager();
    ~AssetManager();

    void Initialize(ID3D11Device *device);

    MaterialPtr LoadMaterial();
    ModelPtr LoadModel(const std::string &path);
};

#endif