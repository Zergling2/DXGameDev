#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerShadedEdgeQuad
*/

cbuffer Cb0 : register(b0)
{
    CbPerShadedEdgeQuad cb_perShadedEdgeQuad;
}

static const float g_edgeThickness = 1.0f;

PSOutput main(PSInputShadedEdgeQuadFragment input)
{
    PSOutput output;
    
    float2 centerV = cb_perShadedEdgeQuad.position;
    float2 posV = input.posV;
    
    const float2 offset = posV - centerV;   // 중앙을 기준으로 한 위치벡터
    
    float2 innerQuadHalfSize = cb_perShadedEdgeQuad.size * 0.5f - g_edgeThickness.xx;
    
    bool isLeftEdge = offset.x < -innerQuadHalfSize.x;
    bool isTopEdge = offset.y > innerQuadHalfSize.y;
    bool isRightEdge = offset.x > innerQuadHalfSize.x;
    bool isBottomEdge = offset.y < -innerQuadHalfSize.y;
    
    float4 color = cb_perShadedEdgeQuad.color;
    
    if (isRightEdge || isBottomEdge)
        color.rgb += cb_perShadedEdgeQuad.rbColorWeight.rrr;
    else if (isLeftEdge || isTopEdge)
        color.rgb += cb_perShadedEdgeQuad.ltColorWeight.rrr;
    
    output.color = color;
    
    return output;
}
