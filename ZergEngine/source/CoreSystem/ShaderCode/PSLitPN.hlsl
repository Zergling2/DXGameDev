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
    CbMaterial cb_material;
}

PSOutput main(PSInputPNFragment input)
{
    PSOutput output;
    
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
        ComputeDirectionalLight(cb_perFrame.dl[i], cb_material.mtl.specular.w, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    for (i = 0; i < cb_perFrame.plCount; ++i)
    {
        ComputePointLight(cb_perFrame.pl[i], cb_material.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    for (i = 0; i < cb_perFrame.slCount; ++i)
    {
        ComputeSpotLight(cb_perFrame.sl[i], cb_material.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    float4 diffuseMtl = cb_material.mtl.diffuse;
    float4 specularMtl = cb_material.mtl.specular;
    
    float4 diffuseColor = diffuseMtl * (diffuseLight + ambientLight);
    float4 specularColor = specularMtl * specularLight;
    
    output.color = (diffuseColor + specularColor);
    output.color.a = diffuseMtl.a; // 알파 채널은 재질의 diffuse 속성을 사용해야 함.

    output.color = saturate(output.color);
    
    return output;
}
