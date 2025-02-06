#include "ShaderCommon.hlsli"

PSOutput main(PSInputStandardColoringPC input)
{
    PSOutput output;
    
    output.color = input.color;
    
    return output;
}
