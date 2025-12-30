#include <fstream>
#include <map>

#include "external/tiny_obj_loader.h"
#include "external/stb_image.h"

#include "asset_manager.hpp"
#include "logging.hpp"

AssetManager::AssetManager() : device(nullptr), assetDir("assets/"), defaultPipelineState{} {}

AssetManager::~AssetManager() {}

void AssetManager::CreateDefaultPipelineState() {
    this->LoadShaders(
        "vs_default.cso",
        "ps_default.cso",
        &this->defaultPipelineState.vertexShader,
        &this->defaultPipelineState.pixelShader,
        &this->defaultPipelineState.inputLayout
    );

    this->defaultPipelineState.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

// 1x1 white square
void AssetManager::CreateDefaultTexture() {
    const UINT width  = 1;
    const UINT height = 1;
    UINT32 pixels[width * height] = {
        0xFFFFFFFF
    };

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA inititialData{};
    inititialData.pSysMem = pixels;
    inititialData.SysMemPitch = width * sizeof(UINT);
    inititialData.SysMemSlicePitch = 0;

    ID3D11Texture2D *texture{};
    HRESULT result = this->device->CreateTexture2D(&textureDesc, &inititialData, &texture);
    if (FAILED(result)) {
        LogInfo("Failed to create the default texture\n");
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    this->defaultTexture = std::make_shared<Texture>();
    result = this->device->CreateShaderResourceView(texture, &shaderResourceViewDesc, 
        &this->defaultTexture->shaderResourceView);
    
    texture->Release();

    if (FAILED(result)) {
        LogInfo("Failed to create the default texture SRV\n");
        return;
    }
}

void AssetManager::CreateDefaultMaterial() {
    this->defaultMaterial = std::make_shared<Material>();

    this->defaultMaterial->name = "";
    this->defaultMaterial->pipelineState = &this->defaultPipelineState;
    this->defaultMaterial->diffuseTexture = this->defaultTexture;
    this->defaultMaterial->specularColour = {1.0f, 1.0f, 1.0f};
    this->defaultMaterial->shininess = 100.0f;

    this->materialCache[""] = this->defaultMaterial;
}

void AssetManager::Initialize(ID3D11Device *device) {
    this->device = device;

    this->CreateDefaultPipelineState();
    this->CreateDefaultTexture();
    this->CreateDefaultMaterial();
}

bool AssetManager::LoadFileContents(const std::string &path, void **buffer, size_t *size) {
    if (!buffer || !size)
        return false;

    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        LogInfo("Failed to open file '%s'\n", path.c_str());
        return false;
    }

    *size = file.tellg();
    *buffer = malloc(*size);

    if (!*buffer) {
        LogInfo("Failed to allocate memory for file '%s'\n", path.c_str());
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read((char *)*buffer, *size);
    file.close();

    return true;
}

bool AssetManager::LoadShaders(
    const std::string &vertexShaderPath, 
    const std::string &pixelShaderPath,
    ID3D11VertexShader **vertexShader, 
    ID3D11PixelShader **pixelShader, 
    ID3D11InputLayout **inputLayout
) {
    void *vertexShaderBuffer{};
    size_t vertexShaderBufferSize{};

    if (!this->LoadFileContents(vertexShaderPath, &vertexShaderBuffer, &vertexShaderBufferSize))
        return false;

    HRESULT result = device->CreateVertexShader(vertexShaderBuffer, vertexShaderBufferSize, nullptr, vertexShader);
    if (FAILED(result)) {
        free(vertexShaderBuffer);
        LogInfo("Failed to create vertex shader '%s'\n", vertexShaderPath.c_str());
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    result = device->CreateInputLayout(inputElementDescs, 3, vertexShaderBuffer, vertexShaderBufferSize, inputLayout);

    free(vertexShaderBuffer);

    if (FAILED(result)) {
        LogInfo("Failed to create input layout for '%s'\n", vertexShaderPath.c_str());
        (*vertexShader)->Release();
        return false;
    }

    void *pixelShaderBuffer{};
    size_t pixelShaderBufferSize{};

    if (!this->LoadFileContents(pixelShaderPath, &pixelShaderBuffer, &pixelShaderBufferSize)) {
        (*vertexShader)->Release();
        (*inputLayout)->Release();
        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer, pixelShaderBufferSize, nullptr, pixelShader);

    free(pixelShaderBuffer);

    if (FAILED(result)) {
        LogInfo("Failed to create pixel shader '%s'\n", pixelShaderPath.c_str());
        (*vertexShader)->Release();
        (*inputLayout)->Release();
        return false;
    }

    return true;
}

TexturePtr AssetManager::LoadTexture(const std::string &path) {
    auto iter = this->textureCache.find(path);
    if (iter != this->textureCache.end()) {
        LogInfo("Texture '%s' loaded from cache\n", path.c_str());
        return iter->second;
    }

    std::string fullPath = this->assetDir.empty() ? path : this->assetDir + path;

    int width, height, components;
    unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &components, 4);

    if (!data) {
        LogInfo("Failed to load image file '%s': '%s'\n", path.c_str(), stbi_failure_reason());
        return this->defaultTexture;
    }

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA inititialData{};
    inititialData.pSysMem = data;
    inititialData.SysMemPitch = width * sizeof(unsigned char) * 4;
    inititialData.SysMemSlicePitch = 0;

    ID3D11Texture2D *texture{};
    HRESULT result = this->device->CreateTexture2D(&textureDesc, &inititialData, &texture);

    stbi_image_free(data);

    if (FAILED(result)) {
        LogInfo("Failed to create texture '%s'\n", path.c_str());
        return this->defaultTexture;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    TexturePtr texturePtr = std::make_shared<Texture>();
    result = this->device->CreateShaderResourceView(texture, &shaderResourceViewDesc, 
        &texturePtr->shaderResourceView);

    texture->Release();

    if (FAILED(result)) {
        LogInfo("Failed to create SRV for texture '%s'\n", path.c_str());
        return this->defaultTexture;
    }

    this->textureCache[path] = texturePtr;

    LogInfo("Texture '%s' loaded successfully (%dx%d)\n", path.c_str(), width, height);

    return texturePtr;
}

ModelPtr AssetManager::LoadModel(const std::string &path) {
    auto iter = this->modelCache.find(path);
    if (iter != this->modelCache.end()) {
        LogInfo("Model '%s' loaded from cache\n", path.c_str());
        return iter->second;
    }

    std::string fullPath = this->assetDir.empty() ? path : this->assetDir + path;

    size_t lastSlash = fullPath.find_last_of("/\\");
    std::string baseDir = lastSlash != std::string::npos ? fullPath.substr(0, lastSlash + 1) : "";

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &error, fullPath.c_str(), baseDir.c_str(), true);
    
    if (!error.empty())
        LogInfo("OBJ error or warning: %s\n", error.c_str());

    if (!success) {
        LogInfo("Failed to load OBJ file '%s'\n", path.c_str());
        return nullptr;
    }

    std::vector<MaterialPtr> modelMaterials(materials.size());

    for (int i = 0; i < materials.size(); ++i) {
        std::string materialName = materials[i].name;
        auto iter = this->materialCache.find(materialName);
        if (iter != this->materialCache.end()) {
            modelMaterials[i] = iter->second;
            continue;
        }

        MaterialPtr newMaterial = std::make_shared<Material>();

        newMaterial->name = materialName;

        newMaterial->pipelineState = this->defaultMaterial->pipelineState;

        if (materials[i].diffuse_texname.empty()) {
            newMaterial->diffuseTexture = this->defaultTexture;
        }
        else {
            size_t lastSlash = path.find_last_of("/\\");
            std::string baseDir = lastSlash != std::string::npos ? path.substr(0, lastSlash + 1) : "";
            newMaterial->diffuseTexture = this->LoadTexture(baseDir + materials[i].diffuse_texname);
        }

        newMaterial->specularColour = {
            materials[i].specular[0], 
            materials[i].specular[1], 
            materials[i].specular[2]
        };

        newMaterial->shininess = materials[i].shininess <= 1.0f 
            ? this->defaultMaterial->shininess 
            : materials[i].shininess;

        this->materialCache[materialName] = newMaterial;
        modelMaterials[i] = newMaterial;
    }

    struct Mesh_bucket {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;

        std::map<Vertex, UINT> uniqueVertices;
    };

    std::vector<Mesh_bucket> buckets(materials.size() + 1); // +1 for default material

    for (const tinyobj::shape_t& shape : shapes) {
        int indexOffset = 0;

        for (int faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); ++faceIndex) {
            int vertexCount = shape.mesh.num_face_vertices[faceIndex];

            int materialID = shape.mesh.material_ids[faceIndex];
            materialID = (materialID < 0) ? materials.size() : materialID; // Default material

            for (int vertexIndex = vertexCount - 1; vertexIndex >= 0; --vertexIndex) {
                tinyobj::index_t index = shape.mesh.indices[indexOffset + vertexIndex];

                Vertex vertex{};

                vertex.position[0] =  attributes.vertices[3 * index.vertex_index];
                vertex.position[1] =  attributes.vertices[3 * index.vertex_index + 1];
                vertex.position[2] = -attributes.vertices[3 * index.vertex_index + 2]; //

                if (index.normal_index >= 0) {
                    vertex.normal[0] =  attributes.normals[3 * index.normal_index];
                    vertex.normal[1] =  attributes.normals[3 * index.normal_index + 1];
                    vertex.normal[2] = -attributes.normals[3 * index.normal_index + 2]; //
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv[0] = attributes.texcoords[2 * index.texcoord_index];
                    vertex.uv[1] = 1.0f - attributes.texcoords[2 * index.texcoord_index + 1]; //
                }

                Mesh_bucket &bucket = buckets[materialID];

                auto iter = bucket.uniqueVertices.find(vertex);
                if (iter != bucket.uniqueVertices.end()) {
                    bucket.indices.push_back(iter->second);
                }
                else {
                    UINT newIndex = bucket.vertices.size();

                    bucket.vertices.push_back(vertex);
                    bucket.indices.push_back(newIndex);

                    bucket.uniqueVertices[vertex] = newIndex;
                }
            }

            indexOffset += vertexCount;
        }
    }

    ModelPtr newModel = std::make_shared<Model>();
    std::vector<Vertex> finalVertices;
    std::vector<UINT> finalIndices;

    for (int i = 0; i < buckets.size(); ++i) {
        if (buckets[i].vertices.empty())
            continue;

        Model::Sub_model subModel;

        subModel.mesh.startIndex = finalIndices.size();
        subModel.mesh.indexCount = buckets[i].indices.size();
        subModel.mesh.baseVertex = 0;

        if (i < modelMaterials.size())
            subModel.material = modelMaterials[i];
        else
            subModel.material = this->defaultMaterial;

        int indexOffset = finalVertices.size();
        for (UINT index : buckets[i].indices)
            finalIndices.push_back(index + indexOffset);

        finalVertices.insert(finalVertices.end(), buckets[i].vertices.begin(), buckets[i].vertices.end());

        newModel->subModels.push_back(subModel);
    }

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * finalVertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = finalVertices.data();

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &newModel->vertexBuffer);
    if (FAILED(result)) {
        LogInfo("Failed to create vertex buffer");
        return nullptr;
    }

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(UINT) * finalIndices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = finalIndices.data();

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &newModel->indexBuffer);
    if (FAILED(result)) {
        LogInfo("Failed to create index buffer");
        return nullptr;
    }

    this->modelCache[path] = newModel;

    LogInfo("Model '%s' loaded successfully\n", path.c_str());

    return newModel;
}

MaterialPtr AssetManager::CreateDefaultMaterialCopy(const std::string &name) {
    MaterialPtr materialPtr = std::make_shared<Material>(*this->defaultMaterial);

    materialPtr->name = name;

    this->materialCache[name] = materialPtr;

    return materialPtr;
}

Pipeline_state *AssetManager::GetDefaultPipelineState() {
    return &this->defaultPipelineState;
}

TexturePtr AssetManager::GetDefaultTexture() {
    return this->defaultTexture;
}

MaterialPtr AssetManager::GetDefaultMaterial() {
    return this->defaultMaterial;
}
