#include "ShaderCommon.hlsli"

PSOutput main(PSInputStandardColoringPT input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cb_subsetMtl.mtlFlag) && IsUsingBaseMap(cb_subsetMtl.mtlFlag))
        output.color = g_baseMap.Sample(g_meshTexSampler, input.texCoord);
    else
        output.color = float4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta
    
    return output;
}
