#include "Lighting.hlsli"

// [Constant Buffer]
cbuffer Cb0 : register(b0)
{
    CbPerForwardRenderingFrame cb_perFrame;
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
        float3 normalW = normalize(input.normalW);
    
        const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
        float4 ambientLight = float4(cb_perFrame.ambientLight, 0.0f);
        float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);

        uint i;
        float4 oDL;
        float4 oSL;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl.specular.w, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        float4 diffuse = cb_perSubset.mtl.diffuse * (diffuseLight + ambientLight);
        float4 specular = cb_perSubset.mtl.specular * specularLight;

        output.color.rgb = (diffuse + specular).rgb;
        output.color.a = diffuse.a; // 알파 채널은 diffuse 속성에서 가져온다.

        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없음
        output.color = NO_MATERIAL_COLOR;
    }
    
    return output;
}
