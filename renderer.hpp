#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <Windows.h>
#include <d3d11.h>

class MeshData;
class Material;

struct Draw_command {
    MeshData *mesh;
    Material *material;
};

class Renderer {
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11Texture2D *depthStencilTexture;
    ID3D11DepthStencilView *depthStencilView;
    D3D11_VIEWPORT viewport;

    std::vector<Draw_command> drawCommands;

    bool CreateInterface(HWND hWnd);
    bool CreateRenderTargetView();
    bool CreateDepthStencil(HWND hWnd);
    void SetViewport(HWND hWnd);

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
};

#endif