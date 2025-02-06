#include "ShaderCommon.hlsli"

PSOutput main(PSInputCameraMerger input)
{
    PSOutput output;
    
    float width;
    float height;
    uint sampleCount;   // ��Ƽ���ø� �ؽ����̹Ƿ� �ϳ��� ĭ�� ���� ���� ������ �������.
    g_renderingResult.GetDimensions(width, height, sampleCount);
    
    for (uint i = 0; i < sampleCount; ++i)
        output.color += g_renderingResult.Load(int2(input.texCoord.x * width, input.texCoord.y * height), i);
    
    output.color /= sampleCount;
    
    return output;
}
