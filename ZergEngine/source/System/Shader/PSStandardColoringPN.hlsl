#include "ShaderCommon.hlsli"

PSOutput main(PSInputStandardColoringPN input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cb_subsetMtl.mtlFlag))
    {
        // normalize again
        input.normalW = normalize(input.normalW);
    
        const float3 toEye = normalize(cb_camPosW - input.posW);
    
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        float4 addA, addD, addS;
        uint i;
        for (i = 0; i < cb_directionalLightCount; ++i)
        {
            ComputeDirectionalLight(i, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_pointLightCount; ++i)
        {
            ComputePointLight(i, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_spotLightCount; ++i)
        {
            ComputeSpotLight(i, input.posW, input.normalW, toEye, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        output.color = ambient + diffuse + specular;
        output.color.a = cb_subsetMtl.diffuse.a; // 알파 값은 재질의 diffuse 속성에서 추출 (tr map 미사용시..)
        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없는 렌더링
        output.color = float4(1.0f, 0.0f, 1.0f, 1.0f);
    }
    
    return output;
}
