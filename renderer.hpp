#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "model.hpp"

#define MAX_LIGHTS 8

using namespace DirectX;

struct Draw_command {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;

    UINT indexCount;
    UINT startIndex;
    INT baseVertex;

    MaterialPtr material;

    XMFLOAT4X4 worldMatrix;
    XMFLOAT4X4 worldMatrixInverseTranspose;
};

struct Light_source_data {
    XMFLOAT3 position;
    float intensity;
    XMFLOAT3 direction;
    int type;
    XMFLOAT3 colour;
    float spotLightCosHalfAngle;
};

// cbuffer
struct Per_object_data {
    XMFLOAT4X4 worldMatrix;
    XMFLOAT4X4 worldMatrixInverseTranspose;
};

// cbuffer
// TODO: Combine matrices before sending to the GPU?
struct Per_frame_data {
    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;
};

// cbuffer
struct Per_material_data {
    XMFLOAT3 materialAmbient;
    float pad0;
    XMFLOAT3 materialDiffuse;
    float pad1;
    XMFLOAT3 materialSpecular;
    float materialSpecularExponent;
};

// cbuffer
struct Lighting_data {
    XMFLOAT3 cameraPosition;
    int lightCount;

    XMFLOAT3 ambientLight;
    float pad0;

    Light_source_data lights[MAX_LIGHTS];
};

enum class Sampler_state_type {
    LINEAR_WRAP = 0,
    COUNT
};

class Renderer {
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11Texture2D *depthStencilTexture;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11SamplerState *samplerStates[(int)Sampler_state_type::COUNT];
    D3D11_VIEWPORT viewport;

    ID3D11Buffer *perObjectBuffer;
    ID3D11Buffer *perFrameBuffer;
    ID3D11Buffer *perMaterialBuffer;
    ID3D11Buffer *lightingBuffer;

    Per_frame_data currentFrameData;
    Lighting_data currentLightingData;
    std::vector<Draw_command> drawCommands;

    int width;
    int height;

    bool CreateInterface(HWND hWnd);
    bool CreateRenderTargetView();
    bool CreateDepthStencil(int width, int height);
    bool CreateConstantBuffers();
    bool CreateCommonSamplerStates();
    void SetViewport(int width, int height);

    void UpdatePerObjectBuffer(const Per_object_data &data);
    void UpdatePerFrameBuffer(const Per_frame_data &data);
    void UpdatePerMaterialBuffer(const Per_material_data &data);
    void UpdateLightingBuffer(const Lighting_data &data);
    void BindCommonSamplerStates();

public:
    float clearColour[4];

    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd);
    bool Resize(int width, int height);

    void Submit(const Draw_command &drawCommand);
    void Flush();
    void Begin();
    void End();

    ID3D11Device *GetDevice() const;
    int GetWidth() const;
    int GetHeight() const;
    float GetAspectRatio() const;

    void SetViewMatrix(const XMMATRIX &viewMatrix);
    void SetProjectionMatrix(const XMMATRIX &projectionMatrix);
    void SetCameraPosition(const XMFLOAT3 &position);
    void SetAmbientLightColour(const XMFLOAT3 &colour);
    void SetLightSourceData(const std::vector<Light_source_data> &lightsData);

    void ToggleFullscreen();
    bool IsFullscreened();
};

#endif