#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCameraMerge
*/

cbuffer Cb0 : register(b0)
{
    CbPerCameraMerge cb_perCameraMerge;
}

#define DIRECTX_NDC_SIZE 2.0f
#define LEFT_BOTTOM float2(0.0f, -DIRECTX_NDC_SIZE)
#define LEFT_TOP float2(0.0f, 0.0f)
#define RIGHT_BOTTOM float2(DIRECTX_NDC_SIZE, -DIRECTX_NDC_SIZE)
#define RIGHT_TOP float2(DIRECTX_NDC_SIZE, 0.0f)

PSInputPTFragment main(uint vertexId : SV_VertexID)
{
    PSInputPTFragment output;
    
    // TRIANGLESTRIP
    float2 ndcQuad[4] =
    {
        LEFT_BOTTOM,
        LEFT_TOP,
        RIGHT_BOTTOM,
        RIGHT_TOP
    };
    
    float2 texCoord[4] =
    {
        float2(0.0f, 1.0f),     // Left bottm (Tex coord system)
        float2(0.0f, 0.0f),     // Left top (Tex coord system)
        float2(1.0f, 1.0f),     // Right bottom (Tex coord system)
        float2(1.0f, 0.0f)      // Right up (Tex coord system)
    };
    
    // Perspective division ȿ�� ���� �״�� NDC �������� (NDC���� z = 0.0f�� ��鿡 ��� ����)
    // ����ͻ󿡼��� ����Ʈ�� ����ȭ�� ������ ��Ÿ���Ƿ� �ս��� ��ȯ ����.
    output.posH = float4(
        -1.0f + DIRECTX_NDC_SIZE * cb_perCameraMerge.topLeftX + ndcQuad[vertexId].x * cb_perCameraMerge.width,
        +1.0f - DIRECTX_NDC_SIZE * cb_perCameraMerge.topLeftY + ndcQuad[vertexId].y * cb_perCameraMerge.height,
        0.0f,
        1.0f
    );
    output.texCoord = texCoord[vertexId];
    
    return output;
}
