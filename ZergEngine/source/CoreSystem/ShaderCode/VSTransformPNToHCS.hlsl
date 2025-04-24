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
    
    output.posH = mul(float4(input.posL, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));
    output.posW = mul(float4(input.posL, 1.0f), cb_perMesh.w).xyz;
    output.normalW = normalize(mul(float4(input.normalL, 1.0f), cb_perMesh.wInvTr).xyz);
    
    return output;
}
