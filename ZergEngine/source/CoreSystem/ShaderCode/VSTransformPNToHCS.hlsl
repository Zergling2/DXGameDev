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

struct VSInputVertexPN
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
};

PSInputPNFragment main(VSInputVertexPN input)
{
    PSInputPNFragment output;
    
    float3 posW = mul(float4(input.posL, 1.0f), cb_perMesh.w).xyz;
    output.posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.posW = posW;
    output.normalW = normalize(mul(input.normalL, (float3x3)cb_perMesh.wInvTr));
    
    return output;
}
