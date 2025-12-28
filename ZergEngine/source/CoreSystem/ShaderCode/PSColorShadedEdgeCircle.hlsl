#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerShadedEdgeCircle
*/

cbuffer Cb0 : register(b0)
{
    CbPerShadedEdgeCircle cb_perShadedEdgeCircle;
}

static const float g_edgeThickness = 2.0f;
static const float2 g_lightDir = float2(+0.7071068f, -0.7071068f);  // (+1, -1)의 정규화 버전

PSOutput main(PSInputShadedEdgeCircleFragment input)
{
    PSOutput output;
    
    float2 centerV = cb_perShadedEdgeCircle.position;
    float2 posV = input.posV;
    
    const float2 offset = posV - centerV; // 중앙을 기준으로 한 위치벡터
    
    const float radius = cb_perShadedEdgeCircle.radius;
    const float dist = length(offset);
    
    clip(radius - dist);
    
    bool isEdge = dist >= radius - g_edgeThickness;
    
    float4 color = cb_perShadedEdgeCircle.color;
    if (isEdge)
    {
        float2 offsetNorm = normalize(offset);
        
        float shadeWeight = dot(offsetNorm, g_lightDir) * 0.5f + 0.5f; // lerp 매개변수로 사용하기 위해 [-1.0f, +1.0f] -> [0.0f, 1.0f]로 변환
        
        shadeWeight = saturate(shadeWeight);
        
        float3 weight = lerp(cb_perShadedEdgeCircle.ltColorWeight, cb_perShadedEdgeCircle.rbColorWeight, shadeWeight);
        color.rgb += weight;
    }
    
    output.color = color;
    
    return output;
}
