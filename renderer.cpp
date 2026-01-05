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
      samplerStates{},
      viewport{},
      perObjectBuffer{},
      perFrameBuffer{},
      perMaterialBuffer{},
      lightingBuffer{},
      currentFrameData{},
      currentLightingData{},
      width(0),
      height(0) {
    this->clearColour[0] = 0.0f;
    this->clearColour[1] = 0.0f;
    this->clearColour[2] = 0.0f;
    this->clearColour[3] = 1.0f;

    XMStoreFloat4x4(&this->currentFrameData.viewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&this->currentFrameData.projectionMatrix, XMMatrixIdentity());
}

Renderer::~Renderer() {
    this->drawCommands.clear();

    SafeRelease(this->perFrameBuffer);
    SafeRelease(this->perObjectBuffer);
    SafeRelease(this->perMaterialBuffer);
    SafeRelease(this->lightingBuffer);

    for (int i = 0; i < (int)Sampler_state_type::COUNT; ++i)
        SafeRelease(this->samplerStates[i]);

    SafeRelease(this->depthStencilView);
    SafeRelease(this->depthStencilTexture);
    SafeRelease(this->renderTargetView);

    if (this->deviceContext) {
        this->deviceContext->ClearState();
        this->deviceContext->Flush();

        this->deviceContext->Release();
    }

    if (this->swapChain) {
        this->swapChain->SetFullscreenState(false, nullptr);
        this->swapChain->Release();
    }

    if (this->device) {
#ifdef _DEBUG
        ID3D11Debug *debugDevice{};
        HRESULT result = this->device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugDevice);

        if (SUCCEEDED(result)) {
            OutputDebugStringW(L"\n[ReportLiveDeviceObjects start]\n");
            debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
            OutputDebugStringW(L"[ReportLiveDeviceObjects end]\n\n");

            debugDevice->Release();
        }
#endif

        this->device->Release();
    }
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
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    UINT flags = 0;
#if _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG; 
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

bool Renderer::CreateDepthStencil(int width, int height) {
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

    bufferDesc.ByteWidth = sizeof(Per_material_data);
    result = this->device->CreateBuffer(&bufferDesc, nullptr, &this->perMaterialBuffer);
    if (FAILED(result)) {
        LogError("Failed to create per-material constant buffer");
        return false;
    }

    bufferDesc.ByteWidth = sizeof(Lighting_data);
    result = this->device->CreateBuffer(&bufferDesc, nullptr, &this->lightingBuffer);
    if (FAILED(result)) {
        LogError("Failed to create lighting constant buffer");
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

void Renderer::SetViewport(int width, int height) {
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

    if (FAILED(result)) {
        LogInfo("Failed to map per-object buffer\n");
        return;
    }

    *(Per_object_data *)mappedResource.pData = data;
    this->deviceContext->Unmap(this->perObjectBuffer, 0);
}

void Renderer::UpdatePerFrameBuffer(const Per_frame_data &data) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = this->deviceContext->Map(this->perFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result)) {
        LogInfo("Failed to map per-frame buffer\n");
        return;
    }

    *(Per_frame_data *)mappedResource.pData = data;
    this->deviceContext->Unmap(this->perFrameBuffer, 0);
}

void Renderer::UpdatePerMaterialBuffer(const Per_material_data &data) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = this->deviceContext->Map(this->perMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result)) {
        LogInfo("Failed to map per-material buffer\n");
        return;
    }

    *(Per_material_data *)mappedResource.pData = data;
    this->deviceContext->Unmap(this->perMaterialBuffer, 0);
}

void Renderer::UpdateLightingBuffer(const Lighting_data &data) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = this->deviceContext->Map(this->lightingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result)) {
        LogInfo("Failed to map lighting buffer\n");
        return;
    }

    *(Lighting_data *)mappedResource.pData = data;
    this->deviceContext->Unmap(this->lightingBuffer, 0);
}

void Renderer::BindCommonSamplerStates() {
    this->deviceContext->PSSetSamplers(0, (int)Sampler_state_type::COUNT, this->samplerStates);
}

bool Renderer::Initialize(HWND hWnd) {
    LogInfo(" > Creating renderer...\n");

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    this->width = clientRect.right - clientRect.left;
    this->height = clientRect.bottom - clientRect.top;

    if (!this->CreateInterface(hWnd))
        return false;

    if (!this->CreateRenderTargetView())
        return false;

    if (!this->CreateDepthStencil(this->width, this->height))
        return false;

    if (!this->CreateConstantBuffers())
        return false;

    if (!this->CreateCommonSamplerStates())
        return false;

    this->SetViewport(this->width, this->height);

    LogInfo(" > Renderer created\n");

    return true;
}

bool Renderer::Resize(int width, int height) {
    if (width <= 0 || height <= 0)
        return false;

    SafeRelease(this->renderTargetView);
    SafeRelease(this->depthStencilView);
    SafeRelease(this->depthStencilTexture);

    this->deviceContext->ClearState();
    this->deviceContext->Flush();

    HRESULT result = this->swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(result)) {
        LogError("Failed to resize swap chain buffers");
        return false;
    }

    if (!this->CreateRenderTargetView())
        return false;

    if (!this->CreateDepthStencil(width, height))
        return false;

    this->SetViewport(width, height);

    this->width = width;
    this->height = height;

    return true;
}

void Renderer::Submit(const Draw_command &command) {
    this->drawCommands.push_back(command);
}

void Renderer::Flush() {
    this->UpdatePerFrameBuffer(this->currentFrameData);
    this->deviceContext->VSSetConstantBuffers(1, 1, &this->perFrameBuffer);
    this->deviceContext->PSSetConstantBuffers(1, 1, &this->perFrameBuffer);

    this->UpdateLightingBuffer(this->currentLightingData);
    this->deviceContext->PSSetConstantBuffers(2, 1, &this->lightingBuffer);

    Pipeline_state *currentPipelineState{};
    Material *currentMaterial{};
    ID3D11Buffer *currentVertexBuffer{};

    for (const Draw_command &command : this->drawCommands) {
        this->UpdatePerObjectBuffer({command.worldMatrix, command.worldMatrixInverseTranspose});
        this->deviceContext->VSSetConstantBuffers(0, 1, &this->perObjectBuffer);

        Pipeline_state *pipelineState = command.material->pipelineState;
        if (pipelineState != currentPipelineState) {
            currentPipelineState = pipelineState;

            this->deviceContext->IASetInputLayout(pipelineState->inputLayout);
            this->deviceContext->IASetPrimitiveTopology(pipelineState->primitiveTopology);
            this->deviceContext->VSSetShader(pipelineState->vertexShader, nullptr, 0);
            this->deviceContext->PSSetShader(pipelineState->pixelShader, nullptr, 0);
        }

        Material *material = command.material.get();
        if (material != currentMaterial) {
            currentMaterial = material;

            Per_material_data materialData{};
            materialData.materialAmbient = material->ambientColour;
            materialData.materialDiffuse = material->diffuseColour;
            materialData.materialSpecular = material->specularColour;
            materialData.materialSpecularExponent = material->specularExponent;
            this->UpdatePerMaterialBuffer(materialData);

            this->deviceContext->PSSetShaderResources(0, 1, &material->diffuseTexture->shaderResourceView);
            this->deviceContext->PSSetConstantBuffers(3, 1, &this->perMaterialBuffer);
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

int Renderer::GetWidth() const {
    return this->width;
}

int Renderer::GetHeight() const {
    return this->height;
}

float Renderer::GetAspectRatio() const {
    return (float)this->width / this->height;
}

void Renderer::SetViewMatrix(const XMMATRIX &viewMatrix) {
    XMStoreFloat4x4(&this->currentFrameData.viewMatrix, XMMatrixTranspose(viewMatrix));
}

void Renderer::SetProjectionMatrix(const XMMATRIX &projectionMatrix) {
    XMStoreFloat4x4(&this->currentFrameData.projectionMatrix, XMMatrixTranspose(projectionMatrix));
}

void Renderer::SetCameraPosition(const XMFLOAT3 &position) {
    this->currentLightingData.cameraPosition = position;
}

void Renderer::SetAmbientLightColour(const XMFLOAT3 &colour) {
    this->currentLightingData.ambientLight = colour;
}

void Renderer::SetLightSourceData(const std::vector<Light_source_data> &lightsData) {
    this->currentLightingData.lightCount = lightsData.size() < MAX_LIGHTS ? lightsData.size() : MAX_LIGHTS;

    for (int i = 0; i < this->currentLightingData.lightCount; ++i)
        this->currentLightingData.lights[i] = lightsData[i];
}

void Renderer::ToggleFullscreen() {
    this->swapChain->SetFullscreenState(!this->IsFullscreened(), nullptr);
}

bool Renderer::IsFullscreened() {
    BOOL state;
    this->swapChain->GetFullscreenState(&state, nullptr);

    return state;
}