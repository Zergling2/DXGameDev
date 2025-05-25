#include "ShaderCommon.hlsli"

PSOutput main(PSInputButtonFragment input)
{
    PSOutput output;
    
    output.color = input.color;
    
    return output;
}
