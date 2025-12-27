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
      viewport{},
      perObjectBuffer{},
      perFrameBuffer{},
      currentFrameData{} {
    this->clearColour[0] = 0.3f;
    this->clearColour[1] = 0.0f;
    this->clearColour[2] = 0.0f;
    this->clearColour[3] = 1.0f;

    XMStoreFloat4x4(&this->currentFrameData.viewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&this->currentFrameData.projectionMatrix, XMMatrixIdentity());
}

Renderer::~Renderer() {
    SafeRelease(this->perFrameBuffer);
    SafeRelease(this->perObjectBuffer);

    for (int i = 0; i < (int)Sampler_state_type::COUNT; ++i)
        SafeRelease(this->samplerStates[i]);

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

bool Renderer::CreateConstantBuffers() {
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    bufferDesc.ByteWidth = sizeof(Per_object_data);
    HRESULT result = this->device->CreateBuffer(&bufferDesc, nullptr, &this->perObjectBuffer);
    if (FAILED(result)) {
        LogError("Failed to create per-object constant buffer");
        return false;
    }

    bufferDesc.ByteWidth = sizeof(Per_frame_data);
    result = this->device->CreateBuffer(&bufferDesc, nullptr, &this->perFrameBuffer);
    if (FAILED(result)) {
        LogError("Failed to create per-frame constant buffer");
        return false;
    }

    LogInfo("   > Constant buffers created\n");

    return true;
}

bool Renderer::CreateCommonSamplerStates() {
    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    HRESULT result = this->device->CreateSamplerState(
        &samplerDesc, 
        &this->samplerStates[(int)Sampler_state_type::LINEAR_WRAP]
    );
    if (FAILED(result)) {
        LogError("Failed to create linear wrap sampler state");
        return false;
    }

    LogInfo("   > Common sampler states created\n");

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

void Renderer::UpdatePerObjectBuffer(const Per_object_data &data) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = this->deviceContext->Map(this->perObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
        return;

    *(Per_object_data *)mappedResource.pData = data;
    this->deviceContext->Unmap(this->perObjectBuffer, 0);
}

void Renderer::UpdatePerFrameBuffer() {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = this->deviceContext->Map(this->perFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
        return;

    *(Per_frame_data *)mappedResource.pData = this->currentFrameData;
    this->deviceContext->Unmap(this->perFrameBuffer, 0);
}

void Renderer::BindCommonSamplerStates() {
    this->deviceContext->PSSetSamplers(0, (int)Sampler_state_type::COUNT, this->samplerStates);
}

bool Renderer::Initialize(HWND hWnd) {
    LogInfo(" > Creating renderer...\n");

    if (!this->CreateInterface(hWnd))
        return false;

    if (!this->CreateRenderTargetView())
        return false;

    if (!this->CreateDepthStencil(hWnd))
        return false;

    if (!this->CreateConstantBuffers())
        return false;

    if (!this->CreateCommonSamplerStates())
        return false;

    this->SetViewport(hWnd);

    LogInfo(" > Renderer created\n");

    return true;
}

void Renderer::Submit(const Draw_command &command) {
    this->drawCommands.push_back(command);
}

void Renderer::Flush() {
    this->UpdatePerFrameBuffer();
    this->deviceContext->VSSetConstantBuffers(1, 1, &this->perFrameBuffer);

    Pipeline_state *currentPipelineState{};
    Material *currentMaterial{};
    ID3D11Buffer *currentVertexBuffer{};

    for (const Draw_command &command : this->drawCommands) {
        this->UpdatePerObjectBuffer({command.worldMatrix});
        this->deviceContext->VSSetConstantBuffers(0, 1, &this->perObjectBuffer);

        Pipeline_state *pipelineState = command.material->pipelineState;
        if (pipelineState != currentPipelineState) {
            currentPipelineState = pipelineState;

            this->deviceContext->IASetInputLayout(pipelineState->inputLayout);
            this->deviceContext->IASetPrimitiveTopology(pipelineState->primitiveTopology);
            this->deviceContext->VSSetShader(pipelineState->vertexShader, nullptr, 0);
            this->deviceContext->PSSetShader(pipelineState->pixelShader, nullptr, 0);
        }

        Material *material = command.material;
        if (material != currentMaterial) {
            currentMaterial = material;

            this->deviceContext->PSSetShaderResources(0, 1, &material->diffuseTexture->shaderResourceView);
        }

        ID3D11Buffer *vertexBuffer = command.vertexBuffer;
        if (vertexBuffer != currentVertexBuffer) {
            currentVertexBuffer = vertexBuffer;

            UINT stride = sizeof(Vertex);
            UINT offset = 0;

            this->deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
            this->deviceContext->IASetIndexBuffer(command.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        }

        this->deviceContext->DrawIndexed(command.indexCount, command.startIndex, command.baseVertex);
    }

    this->drawCommands.clear();
}

void Renderer::Begin() {
    this->deviceContext->ClearRenderTargetView(this->renderTargetView, this->clearColour);
    this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);
    this->deviceContext->RSSetViewports(1, &this->viewport);

    this->BindCommonSamplerStates();
}

void Renderer::End() {
    this->swapChain->Present(0, 0);
}

ID3D11Device *Renderer::GetDevice() const {
    return this->device;
}

void Renderer::SetViewMatrix(const XMMATRIX &viewMatrix) {
    XMStoreFloat4x4(&this->currentFrameData.viewMatrix, XMMatrixTranspose(viewMatrix));
}

void Renderer::SetProjectionMatrix(const XMMATRIX &projectionMatrix) {
    XMStoreFloat4x4(&this->currentFrameData.projectionMatrix, XMMatrixTranspose(projectionMatrix));
}