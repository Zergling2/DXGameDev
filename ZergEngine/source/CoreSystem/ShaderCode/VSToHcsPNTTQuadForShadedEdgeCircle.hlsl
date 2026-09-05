#include "ShaderCommon.hlsli"
#include "Math.hlsli"

/*
[Constant Buffer]
CbUIRender
CbPerShadedEdgeCircle
*/

cbuffer Cb0 : register(b0)
{
    CbUIRender cb_UIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerShadedEdgeCircle cb_perShadedEdgeCircle;
}

PSInputShadedEdgeCircleFragment main(VSInputVertexPNTT input)
{
    PSInputShadedEdgeCircleFragment output;
    
    float2 size = (cb_perShadedEdgeCircle.radius * 2.0f).xx;
    
    float3 posL = FlipBasedOnYAxis(input.posL) * float3(size, 0.0f);
    float3 posV = posL + float3(cb_perShadedEdgeCircle.position, 0.0f);
    float4 posH = mul(float4(posV, 1.0f), cb_UIRender.m);
    
    output.pos = posH;
    output.posV = posH.xy;
    
    return output;
}
