struct VSInput {
    float2 inPosition;
    float2 inCoords;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 coords;
};

struct PushData {
    float4x4 transform;
    float4 tint;
};

struct UniformData {
    float4 viewProjection;
};

[[vk::push_constant]]
ConstantBuffer<PushData> pc;

[[vk::binding(0, 0)]]
ConstantBuffer<UniformData> ub;

[[vk::binding(0, 1)]]
Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(ub.viewProjection, mul(pc.transform, float4(input.inPosition, 0.0, 1.0)));
    output.coords = input.inCoords;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture.Sample(vertIn.coords) * pc.tint;
}