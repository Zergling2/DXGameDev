#include "ShaderCommon.hlsli"

PSOutput main(PSInputSkyboxColoring input)
{
    PSOutput output;
    
    output.color = texcube_skyboxCubeMap.Sample(ss_skyboxSampler, input.posL);
    
    return output;
}
