#include "ShaderCommon.hlsli"

struct VSInputStandardTransformPT
{
    float3 posL : POSITION;
    float2 texCoord : TEXCOORD;
};

PSInputStandardColoringPT main(VSInputStandardTransformPT input)
{
    PSInputStandardColoringPT output;
    
    output.posH = mul(float4(input.posL, 1.0f), cbpm_wvp);
    output.texCoord = input.texCoord;
    
    return output;
}
