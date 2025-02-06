#include "ShaderCommon.hlsli"

PSOutput main(PSInputSkyboxColoring input)
{
    PSOutput output;
    
    output.color = g_skyboxCubeMap.Sample(g_skyboxSampler, input.posL);
    
    return output;
}
