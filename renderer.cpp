#include "renderer.hpp"
#include "logging.hpp"

#define SafeRelease(obj) do { if (obj) obj->Release(); } while (0)

Renderer::Renderer()
    : device{},
      deviceContext{},
      swapChain{},
      renderTargetView{},
      depthStencilTexture{},
      depthStencilView{},
      viewport{} {
    this->clearColour[0] = 0.3f;
    this->clearColour[1] = 0.0f;
    this->clearColour[2] = 0.0f;
    this->clearColour[3] = 1.0f;
}

Renderer::~Renderer() {
    SafeRelease(this->depthStencilView);
    SafeRelease(this->depthStencilTexture);
    SafeRelease(this->renderTargetView);
    SafeRelease(this->swapChain);
    SafeRelease(this->deviceContext);
    SafeRelease(this->device);
}

bool Renderer::CreateInterface(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};

    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

#if _DEBUG
    UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT flags = 0;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0
    };

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &this->swapChain,
        &this->device,
        nullptr,
        &this->deviceContext
    );

    if (FAILED(result)) {
        LogError("Failed to create the device or swap chain");
        return false;
    }

    LogInfo("   > Device and swap chain created\n");

    return true;
}

bool Renderer::CreateRenderTargetView() {
    ID3D11Texture2D *backbuffer{};
    HRESULT result = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbuffer);

    if (FAILED(result)) {
        LogError("Failed to create the backbuffer");
        return false;
    }

    LogInfo("   > Backbuffer created\n");

    result = this->device->CreateRenderTargetView(backbuffer, nullptr, &this->renderTargetView);
    backbuffer->Release();

    if (FAILED(result)) {
        LogError("Failed to create the Render Target View");
        return false;
    }

    LogInfo("   > Render Target View created\n");

    return true;
}

bool Renderer::CreateDepthStencil(HWND hWnd) {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    UINT width = clientRect.right - clientRect.left;
    UINT height = clientRect.bottom - clientRect.top;

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT result = this->device->CreateTexture2D(&textureDesc, nullptr, &this->depthStencilTexture);

    if (FAILED(result)) {
        LogError("Failed to create the Depth Stencil texture");
        return false;
    }

    LogInfo("   > Depth stencil texture created\n");

    result = this->device->CreateDepthStencilView(this->depthStencilTexture, nullptr, &this->depthStencilView);

    if (FAILED(result)) {
        LogError("Failed to create the Depth Stencil view");
        return false;
    }

    LogInfo("   > Depth stencil view created\n");

    return true;
}

void Renderer::SetViewport(HWND hWnd) {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    UINT width = clientRect.right - clientRect.left;
    UINT height = clientRect.bottom - clientRect.top;

    this->viewport.TopLeftX = 0.0f;
    this->viewport.TopLeftY = 0.0f;
    this->viewport.Width = width;
    this->viewport.Height = height;
    this->viewport.MinDepth = 0.0f;
    this->viewport.MaxDepth = 1.0f;
}

bool Renderer::Initialize(HWND hWnd) {
    LogInfo(" > Creating renderer...\n");

    if (!this->CreateInterface(hWnd))
        return false;

    if (!this->CreateRenderTargetView())
        return false;

    if (!this->CreateDepthStencil(hWnd))
        return false;

    this->SetViewport(hWnd);

    LogInfo(" > Renderer created\n");

    return true;
}

void Renderer::Begin() {
    this->deviceContext->ClearRenderTargetView(this->renderTargetView, this->clearColour);
    this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::End() {
    this->swapChain->Present(0, 0);
}