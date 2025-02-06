#include "ShaderCommon.hlsli"

struct VSInputSkyboxTransform
{
    float3 posL : POSITION;
};

PSInputSkyboxColoring main(VSInputSkyboxTransform input)
{
    PSInputSkyboxColoring output;
    
    output.posH = mul(float4(input.posL, 1.0f), cb_wvp).xyww; // z = w, 모든 프래그먼트를 far plane상에 위치시킨다. 
    output.posL = input.posL;
    
    return output;
}
