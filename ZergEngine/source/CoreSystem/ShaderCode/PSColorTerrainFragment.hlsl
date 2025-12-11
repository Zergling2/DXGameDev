#include "ShaderCommon.hlsli"
#include "Lighting.hlsli"

// [Sampler State]
// ss_bilinear

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
    CbPerTerrain cb_perTerrain;
}

// [Texture]
Texture2DArray<float4> tex2d_diffuseMapLayer : register(t0);
Texture2DArray<float4> tex2d_normalMapLayer : register(t1);
Texture2D<float4> tex2d_blendMap : register(t2);

PSOutput main(PSInputTerrainFragment input)
{
    PSOutput output;
    
    float4 blendFactor = tex2d_blendMap.Sample(ss_bilinear, input.texCoord);
    const float blendFactors[4] = { blendFactor.r, blendFactor.g, blendFactor.b, blendFactor.a };
    
    uint i;
    float index = 0.0f;
    
    float4 terrainMaterialDiffuse = tex2d_diffuseMapLayer.Sample(ss_common, float3(input.tiledTexCoord, index)); // 레이어 0으로 초기화
    for (i = 1; i < cb_perTerrain.layerArraySize; ++i)  // 나머지 레이어(레이어 1 ~ ) 보간
    {
        index += 1.0f;

        float4 nextTerrainMaterialDiffuse = tex2d_diffuseMapLayer.Sample(ss_common, float3(input.tiledTexCoord, index));
        terrainMaterialDiffuse = lerp(terrainMaterialDiffuse, nextTerrainMaterialDiffuse, blendFactors[i - 1]);
    }
    
    input.normalW = normalize(input.normalW);
    
    const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    MaterialData md;
    md.ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    md.diffuse = terrainMaterialDiffuse;
    md.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float4 oA;
    float4 oD;
    float4 oS;
    for (i = 0; i < cb_perFrame.dlCount; ++i)
    {
        ComputeDirectionalLight(cb_perFrame.dl[i], md, input.normalW, toEyeW, oA, oD, oS);
        ambient += oA;
        diffuse += oD;
        specular += oS;
    }
    
    for (i = 0; i < cb_perFrame.plCount; ++i)
    {
        ComputePointLight(cb_perFrame.pl[i], md, input.posW, input.normalW, toEyeW, oA, oD, oS);
        ambient += oA;
        diffuse += oD;
        specular += oS;
    }
    
    for (i = 0; i < cb_perFrame.slCount; ++i)
    {
        ComputeSpotLight(cb_perFrame.sl[i], md, input.posW, input.normalW, toEyeW, oA, oD, oS);
        ambient += oA;
        diffuse += oD;
        specular += oS;
    }
    
    output.color = (ambient + diffuse) + specular;
    output.color = saturate(output.color);
    
    return output;
}
