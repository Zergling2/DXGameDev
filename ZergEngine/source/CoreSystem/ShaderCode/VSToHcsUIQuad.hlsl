#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
CbPerUIRender
CbPerPTQuad
*/

cbuffer Cb0 : register(b0)
{
    CbPerUIRender cb_perUIRender;
}

cbuffer Cb1 : register(b1)
{
    CbPerPTQuad cb_perPTQuad;
}

// TRIANGLESTRIP
static const float2 g_quad[4] =
{
    float2(-0.5f, -0.5f),
    float2(-0.5f, +0.5f),
    float2(+0.5f, -0.5f),
    float2(+0.5f, +0.5f)
};

// DirectX 텍스쳐 좌표 시스템 (우하단으로 증가)
static const float2 g_texCoords[4] =
{
    float2(0.0f, 1.0f), // Left bottm (Tex coord system)
    float2(0.0f, 0.0f), // Left top (Tex coord system)
    float2(1.0f, 1.0f), // Right bottom (Tex coord system)
    float2(1.0f, 0.0f)  // Right up (Tex coord system)
};

PSInputPTFragment main(uint vertexId : SV_VertexID)
{
    PSInputPTFragment output;
    
    float4x4 m = float4x4(
        float4(cb_perPTQuad.size.x,     0.0f,                       0.0f,   0.0f),
        float4(0.0f,                    cb_perPTQuad.size.y,        0.0f,   0.0f),
        float4(0.0f,                    0.0f,                       1.0f,   0.0f),
        float4(cb_perPTQuad.position.x, cb_perPTQuad.position.y,    0.0f,   1.0f)
    );
    
    const float2 screenPos = mul(float4(g_quad[vertexId], 0.0f, 1.0f), m).xy;
    
    output.posH = float4(screenPos * cb_perUIRender.toNDCSpaceRatio, 0.0f, 1.0f);
    output.texCoord = g_texCoords[vertexId];
    
    return output;
}
