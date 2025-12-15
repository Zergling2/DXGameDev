#include "ShaderCommon.hlsli"

// [Sampler State]
// ss_common

// [Texture]
Texture2D tex2d_tex0 : register(t0);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    output.color = tex2d_tex0.Sample(ss_common, input.texCoord);
    
    return output;
}
