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

struct VSInput
{
    float3 position : POSITION;
    float3 normalL : NORMAL;
    float3 tangentL : TANGENT;
    float2 texCoord : TEXCOORD;
};

PSInputPFragment main(VSInput input)
{
    PSInputPFragment output;
    
    output.posH = mul(float4(input.position, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));

    return output;
}
