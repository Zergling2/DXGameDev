#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerUIRender
CbPerCheckbox
*/

cbuffer Cb0 : register(b0)
{
    CbPerUIRender cb_perUIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerCheckbox cb_perCheckbox;
}

struct VSInputCheckboxVertex
{
    XMFLOAT2 position : POSITION;
	XMFLOAT2 offset : OFFSET;
	FLOAT colorWeight : COLORWEIGHT;    // 배경 박스는 항상 오목 상태이므로 가중치 불변
	uint32_t colorIndex : COLORINDEX;   // 박스와 체크 심볼의 색상 분리 (Box color: [0], Check symbol color: [1])
};

PSInputPCFragment main(VSInputCheckboxVertex input)
{
    PSInputPCFragment output;
    
    const float colorWeight = input.colorWeight;
    const float4 color[2] = { cb_perCheckbox.boxColor, cb_perCheckbox.checkColor };
    
    float4x4 m = float4x4(
        float4(cb_perCheckbox.size.x, 0.0f, 0.0f, 0.0f),
        float4(0.0f, cb_perCheckbox.size.y, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perCheckbox.position + input.offset, 0.0f, 1.0f)
    );
    
    const float2 screenPos = mul(float4(input.position, 0.0f, 1.0f), m).xy;
    output.posH = float4(screenPos * cb_perUIRender.toNDCSpaceRatio, 0.0f, 1.0f);
    
    const float3 rgb = color[input.colorIndex].rgb + colorWeight.xxx;
    const float a = color[input.colorIndex].a;
    output.color = saturate(float4(rgb, a));
    
    return output;
}
