#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPer2DQuad
*/

cbuffer Cb0 : register(b0)
{
    CbPer2DQuad cb_per2DQuad;
}

static const float g_edgeThickness = 1.0f;

PSOutput main(PSInputShadedEdgeQuadFragment input)
{
    PSOutput output;
    
    float2 centerV = cb_per2DQuad.position; // View space quad center
    float2 posV = input.posV;   // View space quad position
    
    const float2 offset = posV - centerV;   // 중앙을 기준으로 한 위치벡터
    
    float2 innerQuadHalfSize = cb_per2DQuad.size * 0.5f - g_edgeThickness.xx;
    
    bool left = offset.x < -innerQuadHalfSize.x;
    bool top = offset.y > innerQuadHalfSize.y;
    bool right = offset.x > innerQuadHalfSize.x;
    bool bottom = offset.y < -innerQuadHalfSize.y;
    
    float4 color = cb_per2DQuad.color;
    
    if (right || bottom)
        color.rgb += cb_per2DQuad.rbColorWeight.rrr;
    else if (left || top)
        color.rgb += cb_per2DQuad.ltColorWeight.rrr;
    
    output.color = color;
    
    return output;
}
