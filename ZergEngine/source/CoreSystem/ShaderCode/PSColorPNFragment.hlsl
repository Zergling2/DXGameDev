#include "Lighting.hlsli"

// [Constant Buffer]
cbuffer Cb0 : register(b0)
{
    CbPerFrame cb_perFrame;
}

cbuffer Cb1 : register(b1)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb2 : register(b2)
{
    CbPerSubset cb_perSubset;
}

PSOutput main(PSInputPNFragment input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cb_perSubset.mtl.mtlFlag))
    {
        // normalize again
        input.normalW = normalize(input.normalW);
    
        const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        float4 oA;
        float4 oD;
        float4 oS;
        uint i;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl, input.normalW, toEyeW, oA, oD, oS);
            ambient += oA;
            diffuse += oD;
            specular += oS;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl, input.posW, input.normalW, toEyeW, oA, oD, oS);
            ambient += oA;
            diffuse += oD;
            specular += oS;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl, input.posW, input.normalW, toEyeW, oA, oD, oS);
            ambient += oA;
            diffuse += oD;
            specular += oS;
        }
    
        output.color = ambient + diffuse + specular;
        output.color.a = cb_perSubset.mtl.diffuse.a;
        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없음
        output.color = NO_MATERIAL_COLOR;
    }
    
    return output;
}
