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
    float3 normalL : NORMAL; // Local space normal vector
    float3 tangentL : TANGENT; // Local space tangent vector
    float2 texCoord : TEXCOORD;
};

PSInputPTFragment main(VSInputVertexPNTT input)
{
    PSInputPTFragment output;
    
    output.posH = mul(float4(input.posL, 1.0f), mul(cb_perMesh.w, cb_perCamera.vp));
    output.texCoord = input.texCoord;   // 섀도우 맵 렌더링 시 알파 클리핑을 하는 경우 텍스처 참조시 필요

    return output;
}
