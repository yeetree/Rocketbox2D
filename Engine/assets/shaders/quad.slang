struct VSInput {
    [[vk::location(0)]] float2 inPosition;
    [[vk::location(1)]] float2 inCoords;
};

struct GlobalUniformData {
    float4x4 viewProjection;
};

struct LocalUniformData {
    float4x4 transform;
    float4 tint;
};

struct VSOutput {
    float4 pos : SV_Position;
    float2 coords;
};

[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniformData> global;
[[vk::binding(0, 1)]] ConstantBuffer<LocalUniformData> local;
[[vk::binding(1, 1)]] Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;

    output.pos = output.pos = mul(global.viewProjection, mul(local.transform, float4(input.inPosition, 0.0, 1.0)));
    output.coords = input.inCoords;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture.Sample(vertIn.coords);
}