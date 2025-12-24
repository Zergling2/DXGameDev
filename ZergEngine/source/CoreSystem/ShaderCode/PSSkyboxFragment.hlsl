#include "ShaderCommon.hlsli"

// [Sampler State]
// g_ssBilinear

// [Texture]
TextureCube texcube_skybox : register(t0);

PSOutput main(PSInputSkyboxFragment input)
{
    PSOutput output;
    
    output.color = texcube_skybox.Sample(g_ssBilinear, input.texCoord);  
    
    return output;
}
