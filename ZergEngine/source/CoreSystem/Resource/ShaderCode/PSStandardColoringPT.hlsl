#include "ShaderCommon.hlsli"

PSOutput main(PSInputStandardColoringPT input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cbps_subsetMtl.mtlFlag) && IsUsingBaseMap(cbps_subsetMtl.mtlFlag))
        output.color = tex2d_baseMap.Sample(ss_meshTexSampler, input.texCoord);
    else
        output.color = float4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta
    
    return output;
}
