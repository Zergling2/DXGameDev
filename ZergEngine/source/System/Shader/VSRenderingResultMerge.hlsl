#include "ShaderCommon.hlsli"

struct VSInputCameraMerger
{
    float3 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

PSInputCameraMerger main(VSInputCameraMerger input)
{
    PSInputCameraMerger output;
    
    // Perspective division ȿ�� ���� �״�� NDC �������� (NDC���� z = 0.0f�� ��鿡 ��� ����)
    output.posH = float4(
        -1.0f + cb_topLeftX * 2.0f + input.pos.x * cb_width * 2.0f,
        1.0f - cb_topLeftY * 2.0f + input.pos.y * cb_height * 2.0f,
        0.0f,
        1.0f
    );
    output.texCoord = input.texCoord;
    
    return output;
}
