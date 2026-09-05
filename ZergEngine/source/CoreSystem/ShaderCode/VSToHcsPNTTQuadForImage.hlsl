#include "ShaderCommon.hlsli"
#include "Math.hlsli"

/*
[Constant Buffer]
CbUIRender
CbPer2DQuad
*/

cbuffer Cb0 : register(b0)
{
    CbUIRender cb_UIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPer2DQuad cb_per2DQuad;
}

PSInputPTFragment main(VSInputVertexPNTT input)
{
    PSInputPTFragment output;
    
    float3 posL = FlipBasedOnYAxis(input.posL) * float3(cb_per2DQuad.size, 0.0f);
    float3 posV = posL + float3(cb_per2DQuad.position, 0.0f);
    float4 posH = mul(float4(posV, 1.0f), cb_UIRender.m);
    
    output.pos = posH;
    
    float2 texCoord = input.texCoord * cb_per2DQuad.uvScale + cb_per2DQuad.uvOffset;
    output.texCoord = texCoord;
    
    return output;
}
