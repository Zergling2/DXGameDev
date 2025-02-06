#include "ShaderCommon.hlsli"

PSOutput main(PSInputTerrainColoring input)
{
    PSOutput output;
    
    const float2 leftTexCoord = float2(input.texCoord.x - cb_terrainTexelSpacingU, input.texCoord.y);
    const float2 rightTexCoord = float2(input.texCoord.x + cb_terrainTexelSpacingU, input.texCoord.y);
    const float2 topTexCoord = float2(input.texCoord.x, input.texCoord.y - cb_terrainTexelSpacingV);
    const float2 bottomTexCoord = float2(input.texCoord.x, input.texCoord.y + cb_terrainTexelSpacingV);
    
    const float leftY = g_heightMap.SampleLevel(g_heightMapSampler, leftTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float rightY = g_heightMap.SampleLevel(g_heightMapSampler, rightTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float topY = g_heightMap.SampleLevel(g_heightMapSampler, topTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    const float bottomY = g_heightMap.SampleLevel(g_heightMapSampler, bottomTexCoord, 0).r; // DXGI_FORMAT_R16_FLOAT
    
    const float3 tangentW = normalize(float3(2.0f * cb_terrainCellSpacing, rightY - leftY, 0.0f));
    const float3 bitangentW = normalize(float3(0.0f, bottomY - topY, -2.0f * cb_terrainCellSpacing));
    const float3 normalW = cross(tangentW, bitangentW);
    
    const float3 toEye = normalize(cb_camPosW - input.posW);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Test
    float4 addA, addD, addS;
    uint i;
    for (i = 0; i < cb_directionalLightCount; ++i)
    {
        ComputeDirectionalLight(i, normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    for (i = 0; i < cb_pointLightCount; ++i)
    {
        ComputePointLight(i, input.posW, normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    for (i = 0; i < cb_spotLightCount; ++i)
    {
        ComputeSpotLight(i, input.posW, normalW, toEye, addA, addD, addS);
        ambient += addA;
        diffuse += addD;
        specular += addS;
    }
    
    output.color = ambient + diffuse + specular;
    output.color.a = cb_subsetMtl.diffuse.a;
    output.color = saturate(output.color);
    
    output.color = float4(normalW, 1.0f);
    
    return output;
}
