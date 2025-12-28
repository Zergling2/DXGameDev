#include "ShaderCommon.hlsli"
#include "Math.hlsli"

/*
[Constant Buffer]
Cb2DRender
CbPerShadedEdgeQuad
*/

cbuffer Cb0 : register(b0)
{
    Cb2DRender cb_2DRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerShadedEdgeQuad cb_perShadedEdgeQuad;
}

PSInputShadedEdgeQuadFragment main(VSInputVertexPNTT input)
{
    PSInputShadedEdgeQuadFragment output;
    
    float4x4 m = float4x4(
        float4(cb_perShadedEdgeQuad.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_perShadedEdgeQuad.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perShadedEdgeQuad.position, 0.0f, 1.0f)
    );
    
    float3 posL = FlipBasedOnYAxis(input.posL);     // Position (Local space)
    float3 posV = mul(float4(posL, 1.0f), m).xyz;   // Position (View space)
    
    float2 posH = posV.xy * cb_2DRender.toNDCSpaceRatio;
    output.pos = float4(posH, 0.0f, 1.0f);
    
    output.posV = posV.xy;
    
    return output;
}
