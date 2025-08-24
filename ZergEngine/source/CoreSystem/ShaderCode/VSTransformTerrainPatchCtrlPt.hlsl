#include "ShaderCommon.hlsli"

struct VSInputTerrainPatchCtrlPt
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD0;
    float2 boundsY : TEXCOORD1;
};

HSInputTerrainPatchCtrlPt main(VSInputTerrainPatchCtrlPt input)
{
    HSInputTerrainPatchCtrlPt output;
    
    // ���� ���� �����ʹ� ���� ��ǥ �������� ���ǵǾ��ٰ� ����
    output.posW = input.posL;

    // �ؽ��� ��ǥ�� �ٿ�� ���� Y ���� �״�� ����
    output.texCoord = input.texCoord;
    output.boundsY = input.boundsY;
    
    return output;
}
