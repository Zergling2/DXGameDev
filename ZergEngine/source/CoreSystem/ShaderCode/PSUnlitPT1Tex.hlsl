#include "ShaderCommon.hlsli"

// [Sampler State]
// g_ssCommon

// [Texture]
Texture2D tex2d_tex0 : register(t0);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    output.color = tex2d_tex0.Sample(g_ssCommon, input.texCoord);
    
    return output;
}
