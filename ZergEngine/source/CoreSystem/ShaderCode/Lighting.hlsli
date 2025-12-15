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

void ComputeDirectionalLight(DirectionalLightData dl, float specExp, float3 normal, float3 toEye, out float4 oDL, out float4 oSL)
{
    oDL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oSL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    const float kd = max(dot(-dl.directionW, normal), 0.0f);
    
    // 난반사광이 닿지 않는 표면에는 정반사광이 존재하지 않는다.
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(dl.directionW, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), specExp);
        
        oDL = kd * dl.diffuse;
        oSL = ks * dl.specular;
    }
}

void ComputePointLight(PointLightData pl, float specExp, float3 pos, float3 normal, float3 toEye, out float4 oDL, out float4 oSL)
{
    oDL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oSL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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
        const float ks = pow(max(dot(toEye, r), 0.0f), specExp);
        
        float distAtt = DistAtt(pl.att, d);
        const float falloff = saturate(1 - smoothstep(0.75f, 1.0f, d / pl.range));  // 하드 컷오프 제거
        
        distAtt *= falloff; // 거리 감쇠 성분에 하드 컷오프 제거 성분 추가
        
        oDL = distAtt * kd * pl.diffuse;
        oSL = distAtt * ks * pl.specular;
    }
}

void ComputeSpotLight(SpotLightData sl, float specExp, float3 pos, float3 normal, float3 toEye, out float4 oDL, out float4 oSL)
{
    oDL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oSL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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
        const float ks = pow(max(dot(toEye, r), 0.0f), specExp);
        
        float distAtt = DistAtt(sl.att, d);
        const float falloff = saturate(1 - smoothstep(0.9f, 1.0f, d / sl.range)); // 하드 컷오프 제거
        
        distAtt *= falloff; // 거리 감쇠 성분에 하드 컷오프 제거 성분 추가
        
        const float angleAtt = ComputeSpotLightAngleAtt(sl.directionW, -toLight, sl.innerConeCos, sl.outerConeCos);
        
        oDL = angleAtt * distAtt * kd * sl.diffuse;
        oSL = angleAtt * distAtt * ks * sl.specular;
    }
}

float3 ComputeNormalMapping(float3 normalW, float3 tangentW, float3 normalT)
{
    // T, B, N이 모두 정규화된 벡터일 경우
    // T x B = N
    // B x N = T
    // N x T = B
    // 이 성립한다.
    // (T x B = N, B x N = T, N x T = B)
    float3 bitangentW = cross(normalW, tangentW);
            
    // input에 들어온 Normal과 Tangent는 월드 공간 기준의 벡터이다. (버텍스 셰이더에서 월드 변환시켜서 넘어옴.)
    // 따라서 T, B, N을 열으로 배치하여 행렬
    // Tx Bx Nx
    // Ty By Ny
    // Tz Bz Nz
    // 을 만들면 이 행렬은 (월드 공간 -> 탄젠트 공간) 변환 행렬이 된다.
    
    // 그런데 지금 필요한 것은 노말 텍스쳐에서 샘플링한 벡터(탄젠트 공간에서의 노말)를 월드 공간으로 변환하는 것이므로
    // 위의 행렬의 역행렬이 필요하다.
    // 그런데 위 행렬은 정규직교 행렬이었으므로 전치하는 것만으로 빠르게 역행렬을 구할 수 있다.
    
    // 따라서 위 행렬을 전치시킨 행렬
    // Tx Ty Tz
    // Bx By Bz
    // Nx Ny Nz
    // 는 (탄젠트 공간 -> 월드 공간) 변환 행렬이 된다.
    float3x3 tbn = float3x3(
        tangentW,
        bitangentW,
        normalW
    ); // Tangent -> World
    
    // return normalize(mul(normalT, tbn)); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
    return mul(normalT, tbn); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
}

#endif
