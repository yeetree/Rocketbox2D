struct VSInput {
    float2 inPosition;
    float2 inCoords;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 coords;
};

struct GlobalUniformData {
    float4x4 viewProjection;
};

struct LocalUniformData {
    float4x4 transform;
    float4 tint;
}

[[vk::binding(0, 0)]]
ConstantBuffer<GlobalUniformData> global;

[[vk::binding(0, 1)]]
ConstantBuffer<LocalUniformData> local;

[[vk::binding(1, 1)]]
Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;

    float4 localPos = float4(input.inPosition, 0.0, 1.0);
    float4 worldPos = mul(localPos, local.transform);
    output.pos = mul(worldPos, global.viewProjection);
    
    output.coords = input.inCoords;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture.Sample(vertIn.coords) * local.tint;
}