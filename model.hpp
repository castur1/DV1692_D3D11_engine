#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
};

struct Pipeline_state {
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;

    // TODO: Fixed-function states?

    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

struct Material {
    std::string name;

    Pipeline_state *pipelineState;

    ID3D11ShaderResourceView *diffuseTexture;

    XMFLOAT4 diffuseColour;
    float specularExponent;
};

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
        Material *material;
    };

    std::vector<Sub_model> subModels;
};

#endif