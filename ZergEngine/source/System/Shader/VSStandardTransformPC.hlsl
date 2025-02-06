#include "ShaderCommon.hlsli"

struct VSInputStandardTransformPC
{
    float3 posL : POSITION;
    float4 color : COLOR;
};

PSInputStandardColoringPC main(VSInputStandardTransformPC input)
{
    PSInputStandardColoringPC output;
    
    output.posH = mul(float4(input.posL, 1.0f), cb_wvp);
    output.color = input.color;
    
    return output;
}
