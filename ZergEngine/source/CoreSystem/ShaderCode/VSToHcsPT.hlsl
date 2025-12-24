#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
PerMesh
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
};

cbuffer Cb1 : register(b1)
{
    CbPerMesh cb_perMesh;
};

struct VSInputVertexPT
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD;
};

PSInputPTFragment main(VSInputVertexPT input)
{
    PSInputPTFragment output;
    
    float4 posH = mul(float4(input.posL, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));
    output.pos = posH;
    output.texCoord = input.texCoord;
    
    return output;
}
