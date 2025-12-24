#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerCamera
CbPerBillboard
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb1 : register(b1)
{
    CbPerBillboard cb_perBillboard;
}

PSInputPNTTFragment main(VSInputVertexPNTT input)
{
    // 빌보드는 3D 오브젝트이기 때문에 3D 데이터들을 출력해야 함.
    PSInputPNTTFragment output;
    
    float3 posW = mul(float4(input.posL, 1.0f), cb_perBillboard.w).xyz;
    float4 posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.pos = posH;
    output.posW = posW;
    output.normalW = normalize(mul(input.normalL, (float3x3) cb_perBillboard.wInvTr));
    output.tangentW = normalize(mul(input.tangentL, (float3x3) cb_perBillboard.wInvTr));
    
    float2 texCoord = input.texCoord * cb_perBillboard.uvScale + cb_perBillboard.uvOffset;
    output.texCoord = texCoord;
    
    return output;
}
