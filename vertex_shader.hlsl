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
    output.position = float4(input.position, 1.0f);
    output.colour = float4(input.colour, 1.0f);
    return output;
}