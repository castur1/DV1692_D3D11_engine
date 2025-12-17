#include "mesh_data.hpp"
#include "logging.hpp"

#define SafeRelease(obj) do { if (obj) obj->Release(); } while (0)

MeshData::MeshData(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<UINT> &indices)
    : vertexBuffer(nullptr), vertexCount(0), indexBuffer(nullptr), indexCount(0) {
    this->vertexCount = vertices.size();
    this->indexCount = indices.size();

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * this->vertexCount;
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData{};
    vertexData.pSysMem = vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
    if (FAILED(result)) {
        LogError("Failed to create vertex buffer");
        return;
    }

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * this->indexCount;
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
    if (FAILED(result)) {
        LogError("Failed to create index buffer");
        return;
    }
}

MeshData::~MeshData() {
    SafeRelease(this->indexBuffer);
    SafeRelease(this->vertexBuffer);
}

ID3D11Buffer *MeshData::GetVertexBuffer() const {
    return this->vertexBuffer;
}

UINT MeshData::GetVertexCount() const {
    return this->vertexCount;
}

ID3D11Buffer *MeshData::GetIndexBuffer() const {
    return this->indexBuffer;
}

UINT MeshData::GetIndexCount() const {
    return this->indexCount;
}