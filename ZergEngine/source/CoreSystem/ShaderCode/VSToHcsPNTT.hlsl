#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
PerMesh
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb1 : register(b1)
{
    CbPerMesh cb_perMesh;
}

PSInputPNTTFragment main(VSInputVertexPNTT input)
{
    PSInputPNTTFragment output;
    
    float3 posW = mul(float4(input.posL, 1.0f), cb_perMesh.w).xyz;
    float4 posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.pos = posH;
    output.posW = posW;
    output.normalW = normalize(mul(input.normalL, (float3x3)cb_perMesh.wInvTr));
    output.tangentW = normalize(mul(input.tangentL, (float3x3) cb_perMesh.wInvTr));
    output.texCoord = input.texCoord;
    
    return output;
}
