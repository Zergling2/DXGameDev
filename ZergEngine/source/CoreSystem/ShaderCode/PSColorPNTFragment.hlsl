#include "Lighting.hlsli"

// [Sampler State]
// ss_common

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

// [Texture]
Texture2D tex2d_diffuseMap : register(t0);
Texture2D tex2d_specularMap : register(t1);

PSOutput main(PSInputPNTFragment input)
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
    
        float4 addA, addD, addS;
        uint i;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl, input.normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl, input.posW, input.normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl, input.posW, input.normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
        
        if (IsUsingDiffuseMap(cb_perSubset.mtl.mtlFlag))
            output.color = tex2d_diffuseMap.Sample(ss_common, input.texCoord) * (ambient + diffuse) + specular;
        else
            output.color = (ambient + diffuse) + specular;
    
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
