#include <fstream>

#include "material.hpp"
#include "logging.hpp"

#define SafeRelease(obj) do { if (obj) obj->Release(); } while (0)

Material::Material(ID3D11Device *device, const std::wstring &vertexShaderPath, const std::wstring &pixelShaderPath)
    : vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr) {
    void *vsBuffer{};
    size_t vsSize{};

    if (!this->LoadShader(vertexShaderPath, &vsBuffer, &vsSize))
        return;

    HRESULT result = device->CreateVertexShader(vsBuffer, vsSize, nullptr, &this->vertexShader);
    if (FAILED(result)) {
        free(vsBuffer);
        LogError("Failed to create vertex shader");
        return;
    }

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    result = device->CreateInputLayout(inputElements, 2, vsBuffer, vsSize, &this->inputLayout);
    free(vsBuffer);

    if (FAILED(result)) {
        LogError("Failed to create input layout");
        return;
    }

    void *psBuffer{};
    size_t psSize{};

    if (!this->LoadShader(pixelShaderPath, &psBuffer, &psSize))
        return;

    result = device->CreatePixelShader(psBuffer, psSize, nullptr, &this->pixelShader);
    free(psBuffer);

    if (FAILED(result)) {
        LogError("Failed to create pixel shader");
        return;
    }
}

Material::~Material() {
    SafeRelease(this->inputLayout);
    SafeRelease(this->pixelShader);
    SafeRelease(this->vertexShader);
}

bool Material::LoadShader(const std::wstring &path, void **buffer, size_t *size) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        LogError("Failed to open shader file");
        return false;
    }

    *size = file.tellg();
    *buffer = malloc(*size);

    if (*buffer == nullptr) {
        LogError("Failed to allocate memory for shader");
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read((char *)*buffer, *size);
    file.close();

    return true;
}

void Material::Bind(ID3D11DeviceContext *deviceContext) {
    deviceContext->IASetInputLayout(this->inputLayout);
    deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
    deviceContext->PSSetShader(this->pixelShader, nullptr, 0);
}