#include "ShaderCommon.hlsli"

PSOutput main(PSInputStandardColoringP input)
{
    PSOutput output;
    
    output.color = float4(1.0f, 0.0f, 1.0f, 1.0f);
    
    return output;
}
