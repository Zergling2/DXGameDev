#include "ShaderCommon.hlsli"

// [Sampler State]
// ss_bilinear

// [Texture]
TextureCube texcube_skybox : register(t0);

PSOutput main(PSInputSkyboxFragment input)
{
    PSOutput output;
    
    output.color = texcube_skybox.Sample(ss_bilinear, input.posL);
    
    return output;
}
