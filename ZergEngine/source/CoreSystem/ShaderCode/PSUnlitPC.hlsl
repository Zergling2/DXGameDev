#include "ShaderCommon.hlsli"

PSOutput main(PSInputPCFragment input)
{
    PSOutput output;
    
    output.color = input.color;
    
    return output;
}
