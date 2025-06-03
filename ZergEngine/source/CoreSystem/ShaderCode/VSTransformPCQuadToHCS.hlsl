#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerUIRender
CbPerPCQuad
*/

cbuffer Cb0 : register(b0)
{
    CbPerUIRender cb_perUIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerPCQuad cb_perPCQuad;
}

// TRIANGLESTRIP
static const float2 g_quad[4] =
{
    float2(-0.5f, -0.5f),
    float2(-0.5f, +0.5f),
    float2(+0.5f, -0.5f),
    float2(+0.5f, +0.5f)
};

PSInputPCFragment main(uint vertexId : SV_VertexID)
{
    PSInputPCFragment output;
    
    float4x4 m = float4x4(
        float4(cb_perPCQuad.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_perPCQuad.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perPCQuad.position, 0.0f, 1.0f)
    );
    const float2 screenPos = mul(float4(g_quad[vertexId], 0.0f, 1.0f), m).xy;
    output.posH = float4(screenPos * cb_perUIRender.toNDCSpaceRatio, 0.0f, 1.0f);
    output.color = cb_perPCQuad.color;
    
    return output;
}
