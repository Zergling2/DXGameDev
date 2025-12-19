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

// TRIANGLESTRIP (1.0f x 1.0f Quad, +Z forwarding)
static const float3 g_quad[4] =
{
    float3(+0.5f, -0.5f, 0.0f),
    float3(+0.5f, +0.5f, 0.0f),
    float3(-0.5f, -0.5f, 0.0f),
    float3(-0.5f, +0.5f, 0.0f)
};

// DirectX 텍스쳐 좌표 시스템 (우하단으로 증가)
static const float2 g_texCoords[4] =
{
    float2(0.0f, 1.0f), // Left bottm (Tex coord system)
    float2(0.0f, 0.0f), // Left top (Tex coord system)
    float2(1.0f, 1.0f), // Right bottom (Tex coord system)
    float2(1.0f, 0.0f)  // Right up (Tex coord system)
};

static const float3 g_normalL =
{
    float3(0.0f, 0.0f, 1.0f)
};

static const float3 g_tangentL =
{
    float3(-1.0f, 0.0f, 0.0f)
};

/*
static const float3 g_bitangentL =
{
    float3(0.0f, -1.0f, 0.0f)
};
*/

PSInputPNTTFragment main(uint vertexId : SV_VertexID)
{
    PSInputPNTTFragment output;
    
    float3 posL = g_quad[vertexId];
    float2 texCoord = g_texCoords[vertexId];
    
    float3 posW = mul(float4(posL, 1.0f), cb_perBillboard.w).xyz;
    output.posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.posW = posW;
    output.normalW = normalize(mul(g_normalL, (float3x3) cb_perBillboard.wInvTr));
    output.tangentW = normalize(mul(g_tangentL, (float3x3) cb_perBillboard.wInvTr));
    output.texCoord = texCoord;
    
    return output;
}
