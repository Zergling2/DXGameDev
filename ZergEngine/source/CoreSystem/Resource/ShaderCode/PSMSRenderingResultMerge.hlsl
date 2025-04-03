#include "ShaderCommon.hlsli"

PSOutput main(PSInputCameraMerger input)
{
    PSOutput output;
    
    float width;
    float height;
    uint sampleCount;   // ��Ƽ���ø� �ؽ����̹Ƿ� �ϳ��� ĭ�� ���� ���� ������ �������.
    tex2dms_renderResult.GetDimensions(width, height, sampleCount);
    
    for (uint i = 0; i < sampleCount; ++i)
        output.color += tex2dms_renderResult.Load(int2(input.texCoord.x * width, input.texCoord.y * height), i);
    
    output.color /= sampleCount;
    
    return output;
}
