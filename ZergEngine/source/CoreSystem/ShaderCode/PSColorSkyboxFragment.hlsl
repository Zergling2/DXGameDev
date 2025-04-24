#include "ShaderCommon.hlsli"

/*
[Sampler State]
Mesh Texture Sampler

[Texture]
SkyboxCubeMap t0
*/

TextureCube texcube_skyboxCubeMap : register(t0);

PSOutput main(PSInputSkyboxFragment input)
{
    PSOutput output;
    
    output.color = texcube_skyboxCubeMap.Sample(ss_skyboxSampler, input.posL);
    
    return output;
}
