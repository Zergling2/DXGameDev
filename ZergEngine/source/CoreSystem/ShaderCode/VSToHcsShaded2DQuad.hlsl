#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerUIRender
CbPerButton
*/

cbuffer Cb0 : register(b0)
{
    CbPerUIRender cb_perUIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerShaded2DQuad cb_perShaded2DQuad;
}

struct VSInputButtonVertex
{
    XMFLOAT2 position : POSITION;
    XMFLOAT2 offset : TEXCOORD0;        // 버튼 정점 변환 오프셋
    XMFLOAT2 shadeWeights : COLOR0;
};

PSInputButtonFragment main(VSInputButtonVertex input)
{
    PSInputButtonFragment output;
    
    const float shadeWeights[2] = { input.shadeWeights[0], input.shadeWeights[1] };
    
    float4x4 m = float4x4(
        float4(cb_perShaded2DQuad.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_perShaded2DQuad.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perShaded2DQuad.position + input.offset, 0.0f, 1.0f)
    );
    
    const float2 screenPos = mul(float4(input.position, 0.0f, 1.0f), m).xy;
    output.posH = float4(screenPos * cb_perUIRender.toNDCSpaceRatio, 0.0f, 1.0f);
    
    const float3 rgb = cb_perShaded2DQuad.color.rgb + shadeWeights[cb_perShaded2DQuad.shadeWeightIndex].xxx;
    const float a = cb_perShaded2DQuad.color.a;
    output.color = saturate(float4(rgb, a));
    
    return output;
}
