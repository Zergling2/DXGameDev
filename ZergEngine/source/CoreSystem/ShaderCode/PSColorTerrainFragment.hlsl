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

void ComputeTerrainFragmentDirectionalLighting(DirectionalLightData dl, float3 normalW, out float4 oD)
{
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    const float kd = max(dot(-dl.directionW, normalW), 0.0f);
    
    // 난반사광이 닿지 않는 표면에는 정반사광이 존재하지 않는다.
    [flatten]
    if (kd > 0.0f)
        oD = kd * dl.diffuse;
}

void ComputeTerrainFragmentPointLighting(PointLightData pl, float3 posW, float3 normalW, out float4 oD)
{
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toLight = pl.positionW - posW;
    const float d = length(toLight);
    
    if (d > pl.range)
        return;
    
    // Normalize
    toLight /= d;
    
    const float kd = dot(toLight, normalW);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float att = 1.0f / dot(pl.att, float3(1.0f, d, d * d));
        
        oD = att * kd * pl.diffuse;
    }
}

void ComputeTerrainFragmentSpotLighting(SpotLightData sl, float3 posW, float3 normalW, float3 toEye, out float4 oD)
{
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toLight = sl.positionW - posW;
    const float d = length(toLight);
    
    if (d > sl.range)
        return;
    
    // Normalize
    toLight /= d;
    
    const float kd = dot(toLight, normalW);
    const float kspot = pow(max(dot(-toLight, sl.directionW), 0.0f), sl.spotExp);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float att = 1.0f / dot(sl.att, float3(1.0f, d, d * d));
        
        oD = kspot * att * kd * sl.diffuse;
    }
}

PSOutput main(PSInputTerrainFragment input)
{
    PSOutput output;
    
    uint i;
    const float3 toEye = normalize(cb_perCamera.cameraPosW - input.posW);
    
    float index = 0.0f;
    
    float4 bf = tex2d_blendMap.Sample(ss_bilinear, input.texCoord);
    float bfArr[4] = { bf.r, bf.g, bf.b, bf.a };
    
    float4 diffuse = tex2d_diffuseMapLayer.Sample(ss_bilinear, float3(input.tiledTexCoord, index)); // 레이어 0으로 초기화
    for (i = 1; i < cb_perTerrain.layerArraySize; ++i)
    {
        index += 1.0f;

        float4 diffuseLayerColor = tex2d_diffuseMapLayer.Sample(ss_bilinear, float3(input.tiledTexCoord, index));
        diffuse = lerp(diffuse, diffuseLayerColor, bfArr[i - 1]);
    }
    
    float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (i = 0; i < cb_perFrame.dlCount; ++i)
    {
        float4 oD;
        ComputeTerrainFragmentDirectionalLighting(cb_perFrame.dl[i], input.normalW, oD);
        
        diffuseLight += oD;
    }
    
    for (i = 0; i < cb_perFrame.plCount; ++i)
    {
        float4 oD;
        ComputeTerrainFragmentPointLighting(cb_perFrame.pl[i], input.posW, input.normalW, oD);
        
        diffuseLight += oD;
    }
    
    for (i = 0; i < cb_perFrame.slCount; ++i)
    {
        float4 oD;
        ComputeTerrainFragmentSpotLighting(cb_perFrame.sl[i], input.posW, input.normalW, toEye, oD);
        
        diffuseLight += oD;
    }
    
    output.color = diffuse * diffuseLight;
    output.color.a = 1.0f;
    
    return output;
}
