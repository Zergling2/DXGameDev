#ifndef __LIGHTING__
#define __LIGHTING__

#include "ShaderCommon.hlsli"

float Attenuation(float3 att, float dist)
{
    return 1.0f / dot(att, float3(1.0f, dist, dist * dist));
}

void ComputeDirectionalLight(DirectionalLightData dl, MaterialData mtl, float3 normal, float3 toEye,
    out float4 oA, out float4 oD, out float4 oS)
{
    oA = dl.ambient * mtl.ambient;
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    const float kd = max(dot(-dl.directionW, normal), 0.0f);
    
    // 난반사광이 닿지 않는 표면에는 정반사광이 존재하지 않는다.
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(dl.directionW, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), mtl.specular.w);
        
        oD = kd * dl.diffuse * mtl.diffuse;
        oS = ks * dl.specular * mtl.specular;
    }
}

void ComputePointLight(PointLightData pl, MaterialData mtl, float3 pos, float3 normal, float3 toEye,
    out float4 oA, out float4 oD, out float4 oS)
{
    // oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oA = pl.ambient * mtl.ambient;
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toLight = pl.positionW - pos;
    const float d = length(toLight);
    
    if (d > pl.range)
        return;
    
    // Normalize
    toLight /= d;
    
    const float kd = dot(toLight, normal);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-toLight, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), mtl.specular.w);
        
        const float4 diffuse = pl.diffuse * mtl.diffuse;
        const float4 specular = pl.specular * mtl.specular;
        
        const float att = Attenuation(pl.att, d);
        
        oD = att * kd * diffuse;
        oS = att * ks * specular;
    }
}

void ComputeSpotLight(SpotLightData sl, MaterialData mtl, float3 pos, float3 normal, float3 toEye,
    out float4 oA, out float4 oD, out float4 oS)
{
    // oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oA = sl.ambient * mtl.ambient;
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toLight = sl.positionW - pos;
    const float d = length(toLight);
    
    if (d > sl.range)
        return;
    
    // Normalize
    toLight /= d;
    
    const float kd = dot(toLight, normal);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-toLight, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), mtl.specular.w);
        
        const float4 diffuse = sl.diffuse * mtl.diffuse;
        const float4 specular = sl.specular * mtl.specular;
        
        const float att = Attenuation(sl.att, d);
        const float kspot = pow(max(dot(-toLight, sl.directionW), 0.0f), sl.spotExp);
        
        oD = kspot * att * kd * diffuse;
        oS = kspot * att * ks * specular;
    }
}

#endif
