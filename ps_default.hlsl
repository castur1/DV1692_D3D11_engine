SamplerState samplerLinearWrap : register(s0);

Texture2D diffuseTexture : register(t0);

struct Pixel_shader_input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(Pixel_shader_input input) : SV_TARGET {
    float4 textureColour = diffuseTexture.Sample(samplerLinearWrap, input.uv);
    
    return float4(textureColour.rgb, 1.0f);
}