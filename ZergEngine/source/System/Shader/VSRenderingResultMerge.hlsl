#include "ShaderCommon.hlsli"

struct VSInputCameraMerger
{
    float3 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

PSInputCameraMerger main(VSInputCameraMerger input)
{
    PSInputCameraMerger output;
    
    // Perspective division 효과 없이 그대로 NDC 공간으로 (NDC공간 z = 0.0f인 평면에 모두 투영)
    output.posH = float4(
        -1.0f + cb_topLeftX * 2.0f + input.pos.x * cb_width * 2.0f,
        1.0f - cb_topLeftY * 2.0f + input.pos.y * cb_height * 2.0f,
        0.0f,
        1.0f
    );
    output.texCoord = input.texCoord;
    
    return output;
}
