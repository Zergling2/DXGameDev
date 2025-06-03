#include "ShaderCommon.hlsli"

/*
[Sampler State]
Mesh Texture Sampler

[Texture]
Image Texture
*/

Texture2D tex2d_image : register(t0);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    output.color = tex2d_image.Sample(ss_meshTexSampler, input.texCoord);
    
    return output;
}
