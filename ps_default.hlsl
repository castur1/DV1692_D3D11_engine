SamplerState samplerLinearWrap : register(s0);

Texture2D diffuseTexture : register(t0);

#define MAX_LIGHTS 8

struct Light_data {
    float3 position;
    float intensity;
    float3 direction;
    int type;
    float3 colour;
    float spotLightAngle;
};

cbuffer Per_frame : register(b1) {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

cbuffer Lighting : register(b2) {
    float3 cameraPosition;
    int lightCount;

    float3 ambientLight;
    float pad0;

    Light_data lights[MAX_LIGHTS];
};

struct Pixel_shader_input {
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(Pixel_shader_input input) : SV_TARGET {
    float4 textureColour = diffuseTexture.Sample(samplerLinearWrap, input.uv);
     
    float3 N = normalize(input.normal);
    float3 V = normalize(cameraPosition - input.worldPosition);
    
    float3 directLighting = float3(0, 0, 0);

    for (int i = 0; i < lightCount; ++i) {
        Light_data L = lights[i];

        // Light Vector (Pixel -> Light)
        float3 lightVec = L.position - input.worldPosition;
        float dist = length(lightVec);
        
        // Skip if out of range (Simple optimization)
        if (dist > 100.0f)
            continue;

        float3 Ldir = lightVec / dist; // Normalize

        // --- Attenuation (Inverse Square Law) ---
        // +0.01 prevents divide by zero
        float att = 1.0f / (dist * dist + 0.01f);
        
        // --- Diffuse (Lambert) ---
        float diff = saturate(dot(N, Ldir));
        
        // --- Specular (Phong) ---
        // R is the reflection of the Light Vector (-Ldir) around Normal
        float3 R = reflect(-Ldir, N);
        float spec = pow(saturate(dot(R, V)), 32.0f);

        // Accumulate
        float3 lightColor = L.colour * L.intensity * att;
        
        directLighting += (lightColor * diff); // Diffuse part
        directLighting += (lightColor * spec * 0.5f); // Specular part
    }

    // 3. Combine with Ambient
    float3 finalColor = (ambientLight + directLighting) * textureColour.rgb;

    return float4(finalColor, 1.0f);
}