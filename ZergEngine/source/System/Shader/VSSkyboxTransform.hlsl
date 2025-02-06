#include "ShaderCommon.hlsli"

struct VSInputSkyboxTransform
{
    float3 posL : POSITION;
};

PSInputSkyboxColoring main(VSInputSkyboxTransform input)
{
    PSInputSkyboxColoring output;
    
    output.posH = mul(float4(input.posL, 1.0f), cb_wvp).xyww; // z = w, ��� �����׸�Ʈ�� far plane�� ��ġ��Ų��. 
    output.posL = input.posL;
    
    return output;
}
