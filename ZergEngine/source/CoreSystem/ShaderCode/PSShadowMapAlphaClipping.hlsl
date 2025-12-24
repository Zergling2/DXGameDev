#include "ShaderCommon.hlsli"

// [Sampler State]
// g_ssCommon

// [Texture]
Texture2D tex2d_diffuseMap : register(t0);

void main(PSInputPTFragment input)
{
    float4 diffuse = tex2d_diffuseMap.Sample(g_ssCommon, input.texCoord);
    
    // Don't write transparent pixels to the shadow map.
    clip(diffuse.a - 0.1f);
}
