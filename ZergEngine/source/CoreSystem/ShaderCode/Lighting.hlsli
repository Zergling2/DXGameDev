#ifndef __LIGHTING__
#define __LIGHTING__

#include "ShaderCommon.hlsli"

// 거리 기반 감쇠
float DistAtt(float3 att, float dist)
{
    return 1.0f / dot(att, float3(1.0f, dist, dist * dist));
}

// 각도 기반 감쇠
float ComputeSpotLightAngleAtt(float3 spotDirW, float3 toSurfaceW, float innerConeCos, float outerConeCos)
{
    // spotDir(Normalized)
    // toSurface(Normalized)
    const float angleCos = dot(spotDirW, toSurfaceW);
    
    if (angleCos <= outerConeCos)
        return 0.0f;
    
    if (angleCos >= innerConeCos)
        return 1.0f;

    // innerConeAngle < outerConeAngle
    // innerConeCos > outerConeCos
    const float t = saturate((angleCos - outerConeCos) / (innerConeCos - outerConeCos));    // [0,1]
    
    return t * t;       // return pow(t, 2.0f); (지수값 증가로 inner cone <-> outer cone 페이드아웃 조절 가능)
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
        
        float distAtt = DistAtt(pl.att, d);
        const float falloff = saturate(1 - smoothstep(0.75f, 1.0f, d / pl.range));  // 하드 컷오프 제거
        
        distAtt *= falloff; // 거리 감쇠 성분에 하드 컷오프 제거 성분 추가
        
        oD = distAtt * kd * diffuse;
        oS = distAtt * ks * specular;
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
        
        float distAtt = DistAtt(sl.att, d);
        const float falloff = saturate(1 - smoothstep(0.9f, 1.0f, d / sl.range)); // 하드 컷오프 제거
        
        distAtt *= falloff; // 거리 감쇠 성분에 하드 컷오프 제거 성분 추가
        
        const float angleAtt = ComputeSpotLightAngleAtt(sl.directionW, -toLight, sl.innerConeCos, sl.outerConeCos);
        
        oD = angleAtt * distAtt * kd * diffuse;
        oS = angleAtt * distAtt * ks * specular;
    }
}

#endif
