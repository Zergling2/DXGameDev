#include "ShaderCommon.hlsli"
#include "SubsetLighting.hlsli"

PSOutput main(PSInputStandardColoringPNT input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cbps_subsetMtl.mtlFlag))
    {
        // normalize again
        input.normalW = normalize(input.normalW);
    
        const float3 toEye = normalize(cbpc_cameraPosW - input.posW);
    
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        float4 addA, addD, addS;
        uint i;
        for (i = 0; i < cbpf_dlCount; ++i)
        {
            ComputeDirectionalLight(i, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cbpf_plCount; ++i)
        {
            ComputePointLight(i, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cbpf_slCount; ++i)
        {
            ComputeSpotLight(i, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
   
        if (IsUsingBaseMap(cbps_subsetMtl.mtlFlag))
            output.color = tex2d_baseMap.Sample(ss_meshTexSampler, input.texCoord) * (ambient + diffuse) + specular;
        else
            output.color = (ambient + diffuse) + specular;
    
        output.color.a = cbps_subsetMtl.diffuse.a; // 알파 값은 재질의 diffuse 속성에서 추출 (tr map 미사용시..)
        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없는 렌더링
        output.color = float4(1.0f, 0.0f, 1.0f, 1.0f);
    }
    
    return output;
}
