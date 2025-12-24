#include "ShaderCommon.hlsli"
#include "Math.hlsli"

/*
[Constant Buffer]
Cb2DRender
CbPer2DQuad
*/

cbuffer Cb0 : register(b0)
{
    Cb2DRender cb_2DRender;
}

cbuffer Cb1 : register(b1)
{
    CbPer2DQuad cb_per2DQuad;
}

PSInputShadedEdgeQuadFragment main(VSInputVertexPNTT input)
{
    PSInputShadedEdgeQuadFragment output;
    
    float4x4 m = float4x4(
        float4(cb_per2DQuad.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_per2DQuad.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_per2DQuad.position, 0.0f, 1.0f)
    );
    
    float3 posL = FlipBasedOnYAxis(input.posL);     // Position (Local space)
    float3 posV = mul(float4(posL, 1.0f), m).xyz;   // Position (View space)
    
    float2 posH = posV.xy * cb_2DRender.toNDCSpaceRatio;
    output.pos = float4(posH, 0.0f, 1.0f);
    
    output.posV = posV.xy;
    
    float2 texCoord = input.texCoord * cb_per2DQuad.uvScale + cb_per2DQuad.uvOffset;
    output.texCoord = texCoord;
    
    return output;
}
