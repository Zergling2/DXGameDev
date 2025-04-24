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

struct VSInputVertexPC
{
    float3 posL : POSITION;
    float4 color : COLOR;
};

PSInputPCFragment main(VSInputVertexPC input)
{
    PSInputPCFragment output;
    
    output.posH = mul(float4(input.posL, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));
    output.color = input.color;
    
    return output;
}
