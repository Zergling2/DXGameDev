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

struct VSInputVertexPNTT
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;        // Local space normal vector
    float3 tangentL : TANGENT;      // Local space tangent vector
    float2 texCoord : TEXCOORD;
};

PSInputPNTTFragment main(VSInputVertexPNTT input)
{
    PSInputPNTTFragment output;
    
    output.posH = mul(float4(input.posL, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));
    output.posW = mul(float4(input.posL, 1.0f), cb_perMesh.w).xyz;
    output.normalW = normalize(mul(input.normalL, (float3x3)cb_perMesh.wInvTr));
    output.tangentW = normalize(mul(input.tangentL, (float3x3) cb_perMesh.wInvTr));
    output.texCoord = input.texCoord;
    
    return output;
}
