#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerUIRender
CbPerShaded2DQuad
*/

cbuffer Cb0 : register(b0)
{
    CbPerUIRender cb_perUIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerShaded2DQuad cb_perShaded2DQuad;
}

struct VSInputShaded2DQuadVertex
{
    float2 position : POSITION;
    float2 offset : OFFSET;
    float2 colorWeights : COLORWEIGHTS;
};

PSInputPCFragment main(VSInputShaded2DQuadVertex input)
{
    PSInputPCFragment output;
    
    const float colorWeights[2] = { input.colorWeights[0], input.colorWeights[1] };
    
    float4x4 m = float4x4(
        float4(cb_perShaded2DQuad.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_perShaded2DQuad.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perShaded2DQuad.position + input.offset, 0.0f, 1.0f)
    );
    
    const float2 screenPos = mul(float4(input.position, 0.0f, 1.0f), m).xy;
    output.posH = float4(screenPos * cb_perUIRender.toNDCSpaceRatio, 0.0f, 1.0f);
    
    const float3 rgb = cb_perShaded2DQuad.color.rgb + colorWeights[cb_perShaded2DQuad.colorWeightIndex].xxx;
    const float a = cb_perShaded2DQuad.color.a;
    output.color = saturate(float4(rgb, a));
    
    return output;
}
