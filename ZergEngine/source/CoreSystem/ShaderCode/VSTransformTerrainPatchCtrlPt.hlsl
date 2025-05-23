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
    
    // 지형 정점 데이터는 월드 좌표 기준으로 정의되었다고 가정
    output.posW = input.posL;
    
    // 패치 모서리와 카메라 거리를 더 정확하게 측정하기 위해 정점 셰이더에서 미리 높이값을 전달
    output.posW.y = tex2d_heightMap.SampleLevel(ss_heightmapSampler, input.texCoord, 0.0f).r; // DXGI_FORMAT_R16_FLOAT

    // 텍스쳐 좌표와 바운딩 볼륨 Y 값은 그대로 전달
    output.texCoord = input.texCoord;
    output.boundsY = input.boundsY;
    
    return output;
}
