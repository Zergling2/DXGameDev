#include "ShaderCommon.hlsli"

// [Sampler State]
// ss_mesh

// [Texture]
Texture2D tex2d_tex0 : register(t0);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    output.color = tex2d_tex0.Sample(ss_mesh, input.texCoord);
    
    return output;
}
