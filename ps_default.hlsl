struct Pixel_shader_input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(Pixel_shader_input input) : SV_TARGET {
    return float4(input.normal.xyz, 1.0f);
}