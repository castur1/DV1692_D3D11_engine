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

    Pipeline_state defaultPipelineState;
    MaterialPtr defaultMaterial;

    std::string assetDir;

    void CreateDefaultPipelineState();
    void CreateDefaultMaterial();

public:
    AssetManager();
    ~AssetManager();

    void Initialize(ID3D11Device *device);

    bool LoadFileContents(const std::string &path, void **buffer, size_t *size);
    bool LoadShaders(const std::string &vertexShaderPath, const std::string &pixelShaderPath,
        ID3D11VertexShader **vertexShader, ID3D11PixelShader **pixelShader, ID3D11InputLayout **inputLayout);
    // MaterialPtr LoadMaterial();
    ModelPtr LoadModel(const std::string &path);

    Pipeline_state *GetDefaultPipelineState();
    MaterialPtr GetDefaultMaterial();
};

#endif