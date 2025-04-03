#ifndef __SUBSET_LIGHTING__
#define __SUBSET_LIGHTING__

#include "ShaderCommon.hlsli"

// cbpf
// cbps

// 재질이 없으면 조명 처리도 없다.
// 따라서 조명 계산은 항상 재질 상수버퍼를 참조하도록 한다.
void ComputeDirectionalLight(uint dlIndex, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = cbpf_dl[dlIndex].ambient * cbps_subsetMtl.ambient;
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    const float kd = max(dot(-cbpf_dl[dlIndex].directionW, normal), 0.0f);
    
    // 난반사광이 닿지 않는 표면에는 정반사광이 존재하지 않는다.
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(cbpf_dl[dlIndex].directionW, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cbps_subsetMtl.specular.w);
        
        oD = kd * cbpf_dl[dlIndex].diffuse * cbps_subsetMtl.diffuse;
        oS = ks * cbpf_dl[dlIndex].specular * cbps_subsetMtl.specular;
    }
}

void ComputePointLight(uint plIndex, float3 pos, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 L = cbpf_pl[plIndex].positionW - pos;
    const float d = length(L);
    
    if (d > cbpf_pl[plIndex].range)
        return;
    
    L /= d;
    
    oA = cbpf_pl[plIndex].ambient * cbps_subsetMtl.ambient;
    
    const float kd = max(dot(L, normal), 0.0f);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-L, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cbps_subsetMtl.specular.w);
        
        const float4 D = cbpf_pl[plIndex].diffuse * cbps_subsetMtl.diffuse;
        const float4 S = cbpf_pl[plIndex].specular * cbps_subsetMtl.specular;
        
        const float att = 1.0f / dot(cbpf_pl[plIndex].att, float3(1.0f, d, d * d));
        
        oD = att * kd * D;
        oS = att * ks * S;
    }
}

void ComputeSpotLight(uint slIndex, float3 pos, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 L = cbpf_sl[slIndex].positionW - pos;
    const float d = length(L);
    
    if (d > cbpf_sl[slIndex].range)
        return;
    
    L /= d;
    
    oA = cbpf_sl[slIndex].ambient * cbps_subsetMtl.ambient;
    
    const float kd = max(dot(L, normal), 0.0f);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-L, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cbps_subsetMtl.specular.w);
        
        const float4 D = cbpf_sl[slIndex].diffuse * cbps_subsetMtl.diffuse;
        const float4 S = cbpf_sl[slIndex].specular * cbps_subsetMtl.specular;
        
        const float att = 1.0f / dot(cbpf_sl[slIndex].att, float3(1.0f, d, d * d));
        
        oD = att * kd * D;
        oS = att * ks * S;
    }
    
    const float kspot = pow(max(dot(-L, cbpf_sl[slIndex].directionW), 0.0f), cbpf_sl[slIndex].spotExp);
    
    oA *= kspot;
    oD *= kspot;
    oS *= kspot;
}

#endif
