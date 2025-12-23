#include "ShaderCommon.hlsli"

struct VSInputTerrainPatchCtrlPt
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD;
    float2 boundsY : YBOUNDS;
};

HSInputTerrainPatchCtrlPt main(VSInputTerrainPatchCtrlPt input)
{
    HSInputTerrainPatchCtrlPt output;
    
    // 지형 정점 데이터는 월드 좌표 기준으로 정의되었다고 가정
    output.posW = input.posL;

    // 텍스쳐 좌표와 바운딩 볼륨 Y 값은 그대로 전달
    output.texCoord = input.texCoord;
    output.boundsY = input.boundsY;
    
    return output;
}
