#include "ShaderCommon.hlsli"

struct VSInputStandardTransformPNT
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    float2 texCoord : TEXCOORD;
};

PSInputStandardColoringPNT main(VSInputStandardTransformPNT input)
{
    PSInputStandardColoringPNT output;
    
    output.posH = mul(float4(input.posL, 1.0f), cb_wvp);
    output.posW = mul(float4(input.posL, 1.0f), cb_w).xyz;
    output.normalW = normalize(mul(float4(input.normalL, 1.0f), cb_wInvTr).xyz);
    output.texCoord = input.texCoord;
    
    return output;
}
