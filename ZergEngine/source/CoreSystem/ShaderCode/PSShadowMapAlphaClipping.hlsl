#include "ShaderCommon.hlsli"

// [Texture]
Texture2D tex2d_diffuseMap : register(t0);

void main(PSInputPTFragment input)
{
    float4 diffuse = tex2d_diffuseMap.Sample(ss_common, input.texCoord);
    
    // Don't write transparent pixels to the shadow map.
    clip(diffuse.a - 0.15f);
}
