#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCameraMerge
*/

cbuffer Cb0 : register(b0)
{
    CbPerCameraMerge cb_perCameraMerge;
}

#define DIRECTX_NDC_WIDTH 2.0f
#define DIRECTX_NDC_HEIGHT 2.0f
#define LEFT_BOTTOM float2(0.0f, -DIRECTX_NDC_SIZE)
#define LEFT_TOP float2(0.0f, 0.0f)
#define RIGHT_BOTTOM float2(DIRECTX_NDC_SIZE, -DIRECTX_NDC_SIZE)
#define RIGHT_TOP float2(DIRECTX_NDC_SIZE, 0.0f)

// TRIANGLESTRIP!
// 1사분면에 위치시킨다.
static const float2 g_ndcQuad[4] =
{
    float2(0.0f, 0.0f), // LEFT_BOTTOM
    float2(0.0f, DIRECTX_NDC_HEIGHT), // LEFT_TOP
    float2(DIRECTX_NDC_WIDTH, 0.0f), // RIGHT_BOTTOM
    float2(DIRECTX_NDC_WIDTH, DIRECTX_NDC_HEIGHT) // RIGHT_TOP
};

static const float2 g_texCoords[4] =
{
    float2(0.0f, 1.0f), // Left bottm (Tex coord system)
    float2(0.0f, 0.0f), // Left top (Tex coord system)
    float2(1.0f, 1.0f), // Right bottom (Tex coord system)
    float2(1.0f, 0.0f)  // Right up (Tex coord system)
};

// Viewport origin (좌하단)
static const float2 g_origin = float2(-1.0f, -1.0f);

PSInputPTFragment main(uint vertexId : SV_VertexID)
{
    PSInputPTFragment output;
    
    // Perspective division 효과 없이 그대로 NDC 공간으로 (w = 1.0f), NDC공간 z = 0.0f인 평면에 모두 투영
    // 모니터상에서의 뷰포트를 정규화된 값으로 나타내므로 손쉽게 변환 가능.
    
    // 1. 1사분면에 NDC 크기만큼의 쿼드를 위치시키고 스케일링
    float2 vertex = float2(g_ndcQuad[vertexId].x * cb_perCameraMerge.width, g_ndcQuad[vertexId].y * cb_perCameraMerge.height);
    
    float2 offset = float2(
        DIRECTX_NDC_WIDTH * cb_perCameraMerge.topLeftX,
        DIRECTX_NDC_HEIGHT * cb_perCameraMerge.topLeftY
    );
    
    // 2. NDC 좌하단 (-1.0, -1.0)을 기준으로 offset 값만큼 이동
    vertex = vertex + g_origin + offset;
    
    float4 ndcPos = float4(vertex, 0.0f, 1.0f); // w = 1.0f, z = 0.0f
    
    output.pos = ndcPos;
    output.texCoord = g_texCoords[vertexId];
    
    return output;
}
