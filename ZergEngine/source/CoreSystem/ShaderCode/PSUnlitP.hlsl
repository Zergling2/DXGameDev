#include "ShaderCommon.hlsli"

PSOutput main(PSInputPFragment input)
{
    PSOutput output;
    
    output.color = float4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
    
    return output;
}
