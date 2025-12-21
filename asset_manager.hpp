#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <d3d11.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "model.hpp"

class AssetManager {
    ID3D11Device *device;

    std::unordered_map<std::string, ModelPtr> modelCache;
    std::unordered_map<std::string, MaterialPtr> materialCache;
    std::unordered_map<std::string, TexturePtr> textureCache;

    Pipeline_state defaultPipelineState;
    TexturePtr defaultTexture;
    MaterialPtr defaultMaterial;

    std::string assetDir;

    void CreateDefaultPipelineState();
    void CreateDefaultTexture();
    void CreateDefaultMaterial();

public:
    AssetManager();
    ~AssetManager();

    void Initialize(ID3D11Device *device);

    bool LoadFileContents(const std::string &path, void **buffer, size_t *size);
    bool LoadShaders(const std::string &vertexShaderPath, const std::string &pixelShaderPath,
        ID3D11VertexShader **vertexShader, ID3D11PixelShader **pixelShader, ID3D11InputLayout **inputLayout);
    TexturePtr LoadTexture(const std::string &path);
    // MaterialPtr LoadMaterial();
    ModelPtr LoadModel(const std::string &path);

    Pipeline_state *GetDefaultPipelineState();
    TexturePtr GetDefaultTexture();
    MaterialPtr GetDefaultMaterial();
};

#endif