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

cbuffer Cb2 : register(b2)
{
    CbPerArmature cb_perArmature;
}

struct VSInputVertexPNTTSkinned
{
    float3 posL : POSITION;
    float3 normalL : NORMAL; // Local space normal vector
    float3 tangentL : TANGENT; // Local space tangent vector
    float2 texCoord : TEXCOORD;
    float4 weights : WEIGHTS;
    uint4 boneIndices : BONEINDICES;
};

PSInputPNTTFragment main(VSInputVertexPNTTSkinned input)
{
    PSInputPNTTFragment output;
    
    float3 animatedPosL = float3(0.0f, 0.0f, 0.0f);
    float3 animatedNormalL = float3(0.0f, 0.0f, 0.0f);
    float3 animatedTangentL = float3(0.0f, 0.0f, 0.0f);
    uint unpackedBoneIndices[4] = { input.boneIndices.r, input.boneIndices.g, input.boneIndices.b, input.boneIndices.a };
    
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        // 애니메이션 비균등 스케일링 지원 X 코드
        animatedPosL += input.weights[i] * mul(float4(input.posL, 1.0f), cb_perArmature.finalTransform[unpackedBoneIndices[i]]).xyz;
        animatedNormalL += input.weights[i] * mul(input.normalL, (float3x3) cb_perArmature.finalTransform[unpackedBoneIndices[i]]);
        animatedTangentL += input.weights[i] * mul(input.tangentL, (float3x3) cb_perArmature.finalTransform[unpackedBoneIndices[i]]);
    }
    
    float3 posW = mul(float4(animatedPosL, 1.0f), cb_perMesh.w).xyz;
    float4 posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.pos = posH;
    output.posW = posW;
    output.normalW = normalize(mul(animatedNormalL, (float3x3) cb_perMesh.wInvTr));
    output.tangentW = normalize(mul(animatedTangentL, (float3x3) cb_perMesh.wInvTr));
    output.texCoord = input.texCoord;
    
    return output;
}
