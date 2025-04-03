#include "ShaderCommon.hlsli"

struct VSInputStandardTransformP
{
    float3 posL : POSITION;
};

PSInputStandardColoringP main(VSInputStandardTransformP input)
{
    PSInputStandardColoringP output;
    
    output.posH = mul(float4(input.posL, 1.0f), cbpm_wvp);
    
    return output;
}
