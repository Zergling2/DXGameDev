#include "ShaderCommon.hlsli"

// [Texture]
Texture2DMS<float4> tex2dms_tex0 : register(t0);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    float width;
    float height;
    uint sampleCount;   // ��Ƽ���ø� �ؽ����̹Ƿ� �ϳ��� ĭ�� ���� ���� ������ �������.
    tex2dms_tex0.GetDimensions(width, height, sampleCount);
    
    for (uint i = 0; i < sampleCount; ++i)
        output.color += tex2dms_tex0.Load(int2(input.texCoord.x * width, input.texCoord.y * height), i);
    
    output.color /= sampleCount;
    
    return output;
}
