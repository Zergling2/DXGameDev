#include "ShaderCommon.hlsli"
#include "Math.hlsli"

/*
[Constant Buffer]
CbUIRender
CbPerShadedEdgeQuad
*/

cbuffer Cb0 : register(b0)
{
    CbUIRender cb_UIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerShadedEdgeQuad cb_perShadedEdgeQuad;
}

PSInputShadedEdgeQuadFragment main(VSInputVertexPNTT input)
{
    PSInputShadedEdgeQuadFragment output;
    
    float3 posL = FlipBasedOnYAxis(input.posL) * float3(cb_perShadedEdgeQuad.size, 0.0f);
    float3 posV = posL + float3(cb_perShadedEdgeQuad.position, 0.0f);
    float4 posH = mul(float4(posV, 1.0f), cb_UIRender.m);
    
    output.pos = posH;
    output.posV = posV.xy;
    
    return output;
}
