#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <d3d11.h>
#include <string>

class Material {
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;

    bool LoadShader(const std::wstring &path, void *&buffer, SIZE_T &size);

public:
    Material(ID3D11Device *device, const std::wstring &vertexShaderPath, const std::wstring &pixelShaderPath);
    ~Material();

    void Bind(ID3D11DeviceContext *deviceContext);
};

#endif