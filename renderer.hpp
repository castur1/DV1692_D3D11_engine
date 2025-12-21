#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "model.hpp"

using namespace DirectX;

struct Draw_command {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;

    UINT indexCount;
    UINT startIndex;
    INT baseVertex;

    Material *material;

    XMFLOAT4X4 worldMatrix;
};

// cbuffer element
struct Per_object_data {
    XMFLOAT4X4 worldMatrix;
};

// cbuffer element
// TODO: Combine these before sending to the GPU?
struct Per_frame_data {
    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projectionMatrix;
};

class Renderer {
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11Texture2D *depthStencilTexture;
    ID3D11DepthStencilView *depthStencilView;
    D3D11_VIEWPORT viewport;

    ID3D11Buffer *perObjectBuffer;
    ID3D11Buffer *perFrameBuffer;

    Per_frame_data currentFrameData;
    std::vector<Draw_command> drawCommands;

    bool CreateInterface(HWND hWnd);
    bool CreateRenderTargetView();
    bool CreateDepthStencil(HWND hWnd);
    bool CreateConstantBuffers();
    void SetViewport(HWND hWnd);

    void UpdatePerObjectBuffer(const Per_object_data &data);
    void UpdatePerFrameBuffer();

public:
    float clearColour[4];

    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd);

    void Submit(const Draw_command &drawCommand);
    void Flush();
    void Begin();
    void End();

    ID3D11Device *GetDevice() const;

    void SetViewMatrix(const XMMATRIX &viewMatrix);
    void SetProjectionMatrix(const XMMATRIX &projectionMatrix);
};

#endif