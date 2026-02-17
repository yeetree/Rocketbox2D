struct VSInput {
    float2 inPosition;
    float3 inColor;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
};

struct PushData {
    float4x4 transform;
    float4 tint;
};

// Map it to the push_constant slot
[[vk::push_constant]]
ConstantBuffer<PushData> pc;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(pc.transform, float4(input.inPosition, 0.0, 1.0));
    output.color = input.inColor;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return float4(vertIn.color, 1.0) * pc.tint;
}