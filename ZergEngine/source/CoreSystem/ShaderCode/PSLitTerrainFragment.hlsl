#include "ShaderCommon.hlsli"
#include "Lighting.hlsli"

// [Sampler State]
// g_ssBilinear
// g_ssCommon

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
    CbPerTerrain cb_perTerrain;
}

// [Texture]
Texture2DArray tex2d_diffuseMapLayer : register(t0);
Texture2DArray tex2d_specularMapLayer : register(t1);
Texture2DArray tex2d_normalMapLayer : register(t2);
Texture2D tex2d_blendMap : register(t3);

PSOutput main(PSInputTerrainFragment input)
{
    PSOutput output;
    
    float4 blendFactor = tex2d_blendMap.Sample(g_ssBilinear, input.texCoord);
    const float blendFactors[4] = { blendFactor.r, blendFactor.g, blendFactor.b, blendFactor.a };
    
    uint i;
    float index = 0.0f;
    
    float4 terrainMaterialDiffuse = tex2d_diffuseMapLayer.Sample(g_ssCommon, float3(input.tiledTexCoord, index)); // 레이어 0으로 초기화
    for (i = 1; i < cb_perTerrain.layerArraySize; ++i)  // 나머지 레이어(레이어 1 ~ ) 보간
    {
        index += 1.0f;

        float4 nextTerrainMaterialDiffuse = tex2d_diffuseMapLayer.Sample(g_ssCommon, float3(input.tiledTexCoord, index));
        terrainMaterialDiffuse = lerp(terrainMaterialDiffuse, nextTerrainMaterialDiffuse, blendFactors[i - 1]);
    }
    
    float3 normalW = normalize(input.normalW);
    
    const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
    MaterialData mtl;
    mtl.diffuse = terrainMaterialDiffuse;
    mtl.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float4 ambientLight = float4(cb_perFrame.ambientLight, 0.0f);
    float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 oDL;
    float4 oSL;
    for (i = 0; i < cb_perFrame.dlCount; ++i)
    {
        ComputeDirectionalLight(cb_perFrame.dl[i], mtl.specular.w, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    for (i = 0; i < cb_perFrame.plCount; ++i)
    {
        ComputePointLight(cb_perFrame.pl[i], mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    for (i = 0; i < cb_perFrame.slCount; ++i)
    {
        ComputeSpotLight(cb_perFrame.sl[i], mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
        diffuseLight += oDL;
        specularLight += oSL;
    }
    
    float4 diffuseMtl = mtl.diffuse;
    float4 specularMtl = mtl.specular;
    
    float4 diffuseColor = diffuseMtl * (diffuseLight + ambientLight);
    float4 specularColor = specularMtl * specularLight;
    
    output.color = (diffuseColor + specularColor);
    output.color.a = diffuseMtl.a; // 알파 채널은 재질의 diffuse 속성을 사용해야 함.

    output.color = saturate(output.color);
    
    return output;
}
