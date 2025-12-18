cbuffer Per_object : register(b0) {
    float4x4 worldMatrix;
}

cbuffer Per_frame : register(b1) {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

struct Vertex_shader_input {
    float3 position : POSITION;
    float3 colour : COLOUR;
};

struct Vertex_shader_output {
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
};

Vertex_shader_output main(Vertex_shader_input input) {
    Vertex_shader_output output;
    
    float4 position = float4(input.position, 1.0f);
    position = mul(position, worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projectionMatrix); // TODO: Combine view and projection matrices
    output.position = position;
    
    output.colour = float4(input.colour, 1.0f);
    
    return output;
}