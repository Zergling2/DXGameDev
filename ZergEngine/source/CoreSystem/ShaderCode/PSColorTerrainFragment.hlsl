#include "ShaderCommon.hlsli"
#include "SubsetLighting.hlsli"

/*
[Sampler State]
HeightMap Sampler

[Texture]
HeightMap t0

[Constant Buffer]
PerFrame
PerCamera
PerTerrain
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
    CbPerTerrain cb_perTerrain;
}

Texture2D tex2d_heightMap : register(t0);

PSOutput main(PSInputTerrainFragment input)
{
    PSOutput output;
    
    const float2 leftTexCoord = float2(input.texCoord.x - cb_perTerrain.terrainTexelSpacingU, input.texCoord.y);
    const float2 rightTexCoord = float2(input.texCoord.x + cb_perTerrain.terrainTexelSpacingU, input.texCoord.y);
    const float2 topTexCoord = float2(input.texCoord.x, input.texCoord.y - cb_perTerrain.terrainTexelSpacingV);
    const float2 bottomTexCoord = float2(input.texCoord.x, input.texCoord.y + cb_perTerrain.terrainTexelSpacingV);
    
    const float leftY = tex2d_heightMap.SampleLevel(ss_heightmapSampler, leftTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float rightY = tex2d_heightMap.SampleLevel(ss_heightmapSampler, rightTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float topY = tex2d_heightMap.SampleLevel(ss_heightmapSampler, topTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float bottomY = tex2d_heightMap.SampleLevel(ss_heightmapSampler, bottomTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    
    const float3 tangentW = normalize(float3(2.0f * cb_perTerrain.terrainCellSpacing, rightY - leftY, 0.0f));
    const float3 bitangentW = normalize(float3(0.0f, bottomY - topY, -2.0f * cb_perTerrain.terrainCellSpacing));
    const float3 normalW = cross(tangentW, bitangentW);
    
    const float3 toEye = normalize(cb_perCamera.cameraPosW - input.posW);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Test
    float4 addA, addD, addS;
    uint i;
    for (i = 0; i < cb_perFrame.dlCount; ++i)
    {
        ComputeDirectionalLight(cb_perFrame.dl[i], cb_perTerrain.terrainMtl[0], normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    for (i = 0; i < cb_perFrame.plCount; ++i)
    {
        ComputePointLight(cb_perFrame.pl[i], cb_perTerrain.terrainMtl[0], input.posW, normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    for (i = 0; i < cb_perFrame.slCount; ++i)
    {
        ComputeSpotLight(cb_perFrame.sl[i], cb_perTerrain.terrainMtl[0], input.posW, normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    // 임시 코드
    output.color = ambient + diffuse + specular;
    output.color.a = cb_perTerrain.terrainMtl[0].diffuse.a;
    output.color = saturate(output.color);
    
    // 일단 노말을 색상으로 출력 (테스트)cb_perFrame.
    output.color = float4(normalW, 1.0f);
    
    return output;
}
