#include "ShaderCommon.hlsli"

PSOutput main(PSInputCameraMerger input)
{
    PSOutput output;
    
    float width;
    float height;
    uint sampleCount;   // 멀티샘플링 텍스쳐이므로 하나의 칸에 여러 개의 색상이 들어있음.
    tex2dms_renderResult.GetDimensions(width, height, sampleCount);
    
    for (uint i = 0; i < sampleCount; ++i)
        output.color += tex2dms_renderResult.Load(int2(input.texCoord.x * width, input.texCoord.y * height), i);
    
    output.color /= sampleCount;
    
    return output;
}
