#include "ShaderCommon.hlsli"
#include "SubsetLighting.hlsli"

/*
[Sampler State]
Mesh Texture Sampler

[Texture]
LightMap t0
DiffuseMap t1
NormalMap t2
SpecularMap t3

[Constant Buffer]
PerFrame
PerCamera
PerSubset
*/

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

Texture2D tex2d_lightMap : register(t0);
Texture2D tex2d_diffuseMap : register(t1);
Texture2D tex2d_normalMap : register(t2);
Texture2D tex2d_specularMap : register(t3);

PSOutput main(PSInputPNTFragment input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cb_perSubset.mtl.mtlFlag))
    {
        // normalize again
        input.normalW = normalize(input.normalW);
    
        const float3 toEye = normalize(cb_perCamera.cameraPosW - input.posW);
    
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        float4 addA, addD, addS;
        uint i;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
        
        if (IsUsingDiffuseMap(cb_perSubset.mtl.mtlFlag))
            output.color = tex2d_diffuseMap.Sample(ss_meshTexSampler, input.texCoord) * (ambient + diffuse) + specular;
        else
            output.color = (ambient + diffuse) + specular;
    
        output.color.a = cb_perSubset.mtl.diffuse.a; // 알파 값은 재질의 diffuse 속성에서 추출 (tr map 미사용시..)
        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없음
        output.color = NO_MATERIAL_COLOR;
    }
    
    return output;
}
