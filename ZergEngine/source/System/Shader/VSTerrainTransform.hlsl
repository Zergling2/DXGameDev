#include "ShaderCommon.hlsli"

struct VSInputTerrainRendering
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD0;
    float2 boundsY : TEXCOORD1;
};

HSInputTerrainPatchControlPoint main(VSInputTerrainRendering input)
{
    HSInputTerrainPatchControlPoint output;
    
    // ���� ���� �����ʹ� ���� ��ǥ �������� ���ǵǾ��ٰ� �����Ѵ�.
    output.posW = input.posL;
    
    // ��ġ �𼭸��� ī�޶� �Ÿ��� �� ��Ȯ�ϰ� �����ϱ� ���� ���� ���̴����� �̸� ���̰��� �������ش�.
    output.posW.y = g_heightMap.SampleLevel(g_heightMapSampler, input.texCoord, 0.0f).r; // DXGI_FORMAT_R16_FLOAT

    // �ؽ��� ��ǥ�� �ٿ�� ���� Y ���� �״�� �����Ѵ�.
    output.texCoord = input.texCoord;
    output.boundsY = input.boundsY;
    
    return output;
}
