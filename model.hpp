#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

#define SafeRelease(obj) do { if (obj) obj->Release(); } while (0)

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];

    // For std::map used for vertex deduplication in the OBJ loader
    bool operator<(const Vertex& other) const {
        if (position[0] != other.position[0]) return position[0] < other.position[0];
        if (position[1] != other.position[1]) return position[1] < other.position[1];
        if (position[2] != other.position[2]) return position[2] < other.position[2];

        if (normal[0] != other.normal[0]) return normal[0] < other.normal[0];
        if (normal[1] != other.normal[1]) return normal[1] < other.normal[1];
        if (normal[2] != other.normal[2]) return normal[2] < other.normal[2];

        if (uv[0] != other.uv[0]) return uv[0] < other.uv[0];
        return uv[1] < other.uv[1];
    }
};

struct Pipeline_state {
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;

    // TODO: Fixed-function states?

    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    ~Pipeline_state() {
        SafeRelease(this->inputLayout);
        SafeRelease(this->pixelShader);
        SafeRelease(this->vertexShader);
    }
};

struct Texture {
    ID3D11ShaderResourceView *shaderResourceView;

    ~Texture() {
        SafeRelease(this->shaderResourceView);
    }
};

using TexturePtr = std::shared_ptr<Texture>;

struct Material {
    std::string name;

    Pipeline_state *pipelineState;

    TexturePtr diffuseTexture;

    XMFLOAT4 diffuseColour;
    float specularExponent;
};

using MaterialPtr = std::shared_ptr<Material>;

struct Model {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;

    struct Mesh {
        UINT indexCount;
        UINT startIndex;
        UINT baseVertex;
    };

    struct Sub_model {
        Mesh mesh;
        MaterialPtr material;
    };

    std::vector<Sub_model> subModels;

    ~Model() {
        SafeRelease(this->indexBuffer);
        SafeRelease(this->vertexBuffer);
    }
};

using ModelPtr = std::shared_ptr<Model>;

#undef SafeRelease

#endif