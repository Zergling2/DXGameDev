#include "Lighting.hlsli"

// [Sampler State]
// ss_common

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

// [Texture]
// Texture2D tex2d_diffuseMap : register(t0);
// Texture2D tex2d_specularMap : register(t1);
Texture2D tex2d_normalMap : register(t2);

PSOutput main(PSInputPNTTFragment input)
{
    PSOutput output;
    
    // 래스터라이저 보간으로 인해 비정규화 되었을 수 있으므로 노말을 다시 정규화
    float3 normalW = normalize(input.normalW);
    float3 tangentW = input.tangentW - dot(input.tangentW, normalW) * normalW;
    float3 normalT = tex2d_normalMap.Sample(ss_normalMap, input.texCoord).rgb * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
    
    normalW = ComputeNormalMapping(normalW, tangentW, normalT);
        
    // 라이팅 계산
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
    
    return output;
}
