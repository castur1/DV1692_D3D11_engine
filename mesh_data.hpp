#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <vector>
#include "d3d11.h"

struct Vertex {
    float position[3];
    float colour[3];
};

class MeshData {
    ID3D11Buffer *vertexBuffer;
    UINT vertexCount;

    ID3D11Buffer *indexBuffer;
    UINT indexCount;

public:
    MeshData(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<UINT> &indices);
    ~MeshData();

    ID3D11Buffer *GetVertexBuffer() const;
    UINT GetVertexCount() const;
    ID3D11Buffer *GetIndexBuffer() const;
    UINT GetIndexCount() const;
};

#endif