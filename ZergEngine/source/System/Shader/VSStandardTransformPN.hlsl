#include "ShaderCommon.hlsli"

struct VSInputStandardTransformPN
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
};

PSInputStandardColoringPN main(VSInputStandardTransformPN input)
{
    PSInputStandardColoringPN output;
    
    output.posH = mul(float4(input.posL, 1.0f), cb_wvp);
    output.posW = mul(float4(input.posL, 1.0f), cb_w).xyz;
    output.normalW = normalize(mul(float4(input.normalL, 1.0f), cb_wInvTr).xyz);
    
    return output;
}
