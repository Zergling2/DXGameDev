#include "ShaderCommon.hlsli"

/*
[Sampler State]
HeightMap Sampler

[Texture]
HeightMap t0
*/

Texture2D tex2d_heightMap : register(t0);

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
    
    // ��ġ �𼭸��� ī�޶� �Ÿ��� �� ��Ȯ�ϰ� �����ϱ� ���� ���� ���̴����� �̸� ���̰��� ����
    output.posW.y = tex2d_heightMap.SampleLevel(ss_heightmapSampler, input.texCoord, 0.0f).r; // DXGI_FORMAT_R16_FLOAT

    // �ؽ��� ��ǥ�� �ٿ�� ���� Y ���� �״�� ����
    output.texCoord = input.texCoord;
    output.boundsY = input.boundsY;
    
    return output;
}
