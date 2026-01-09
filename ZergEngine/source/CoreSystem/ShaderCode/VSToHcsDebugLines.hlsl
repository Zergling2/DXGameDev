#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

struct VSInputVertexPC
{
    float3 posL : POSITION;
    float4 color : COLOR;
};

PSInputPCFragment main(VSInputVertexPC input)
{
    PSInputPCFragment output;
    
    float3 posW = input.posL;   // 디버그 라인 정점들은 월드 공간상에서 정의되므로 그대로 사용
    
    float4 posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.pos = posH;
    output.color = input.color;
    
    return output;
}
