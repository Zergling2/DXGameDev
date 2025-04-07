#ifndef __SHADER_COMMON__
#define __SHADER_COMMON__

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Constants & Macro
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define MAX_GLOBAL_LIGHT_COUNT 4

#define IsUsingMaterial(mtlFlag)    ((mtlFlag) & 0x00000001)
#define IsUsingLightMap(mtlFlag)    ((mtlFlag) & 0x00000002)
#define IsUsingBaseMap(mtlFlag)     ((mtlFlag) & 0x00000004)
#define IsUsingNormalMap(mtlFlag)   ((mtlFlag) & 0x00000008)
#define IsUsingSpecularMap(mtlFlag) ((mtlFlag) & 0x00000010)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HLSL Structs
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
struct Material
{
    uint mtlFlag;
    
    float4 ambient;
    float4 diffuse;
    float4 specular; // r/g/b/p
    float4 reflect;
};

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 directionW;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 positionW;
    float range;
    
    float3 att;     // a0/a1/a2     a0 + a1d + a2d^2
    float pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 positionW;
    float range;
    
    float3 directionW;
    float spotExp;
    
    float3 att;     // a0/a1/a2     a0 + a1d + a2d^2
    float pad;
};

struct HSInputTerrainPatchControlPoint
{
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD0;
    float2 boundsY : TEXCOORD1;
};

struct DSInputQuadPatchTess
{
    float edgeTessFactor[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

struct DSInputTerrainPatchControlPoint
{
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PSInputStandardColoringP
{
    float4 posH : SV_Position;      // No lighting, No texturing
};

struct PSInputStandardColoringPC
{
    float4 posH : SV_Position;
    float4 color : COLOR;
};

struct PSInputStandardColoringPN
{
    float4 posH : SV_Position;      // Homogeneous clip space position
    float3 posW : POSITION;         // World space position (조명 처리를 위해서)
    float3 normalW : NORMAL;
};

struct PSInputStandardColoringPT
{
    float4 posH : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct PSInputStandardColoringPNT
{
    float4 posH : SV_Position;      // Homogeneous clip space position
    float3 posW : POSITION;         // World space position (조명 처리를 위해서)
    float3 normalW : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct PSInputSkyboxColoring
{
    float4 posH : SV_Position;
    float3 posL : POSITION;         // 로컬 벡터를 그대로 큐브맵 샘플링 벡터로 사용하면 됨
};

struct PSInputCameraMerger
{
    float4 posH : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct PSInputTerrainColoring
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD0;
    float2 tiledTexCoord : TEXCOORD1;
};

struct PSOutput
{
    float4 color : SV_Target;
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Constant Buffers
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
cbuffer PerFrame : register(b0)
{
    uint cbpf_dlCount;
    uint cbpf_plCount;
    uint cbpf_slCount;

    DirectionalLight cbpf_dl[MAX_GLOBAL_LIGHT_COUNT];
    PointLight cbpf_pl[MAX_GLOBAL_LIGHT_COUNT];
    SpotLight cbpf_sl[MAX_GLOBAL_LIGHT_COUNT];
}

cbuffer PerCamera : register(b1)
{
    float3 cbpc_cameraPosW;
    float cbpc_pad;
    
    float cbpc_tessMinDist;
    float cbpc_tessMaxDist;
    float cbpc_minTessExponent;
    float cbpc_maxTessExponent;
    
    float4 cbpc_frustumPlane[6];
}

cbuffer PerMesh : register(b2)
{
    float4x4 cbpm_w;
    float4x4 cbpm_wInvTr;
    float4x4 cbpm_wvp;
};

cbuffer PerSkybox : register(b2)
{
    float4x4 cbpsky_w;
    float4x4 cbpsky_wInvTr;
    float4x4 cbpsky_wvp;
};

cbuffer PerTerrain : register(b2)
{
    float4x4 cbpt_w;                      // World
    float4x4 cbpt_wInvTr;                 // Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)
    float4x4 cbpt_wvp;                    // World * View * Proj

    uint cbpt_terrainMtlCount;

    float cbpt_terrainTextureTiling;      // 도메인 셰이더에서 Tiled 텍스쳐 좌표 계산 시 필요
    float cbpt_terrainCellSpacing;        // 픽셀 셰이더에서 노말 계산 시 필요
    float cbpt_terrainTexelSpacingU;      // 픽셀 셰이더에서 노말 계산 시 필요
    float cbpt_terrainTexelSpacingV; // 픽셀 셰이더에서 노말 계산 시 필요

    Material cbpt_terrainMtl[5];
};

cbuffer PerSubset : register(b3)
{
    Material cbps_subsetMtl;
}

cbuffer PerCameraMerge : register(b4)
{
    // 정규화된 값
    float cbpcm_width;
    float cbpcm_height;
    float cbpcm_topLeftX;
    float cbpcm_topLeftY;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Sampler States
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
SamplerState ss_meshTexSampler : register(s0);
SamplerState ss_skyboxSampler : register(s1);
SamplerState ss_terrainTexSampler : register(s2);
SamplerState ss_heightmapSampler : register(s3);

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Textures
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Texture2DMS<float4> tex2dms_renderResult : register(t0);
TextureCube texcube_skyboxCubeMap : register(t0);
Texture2D tex2d_heightMap : register(t0);
Texture2D tex2d_lightMap : register(t0);
Texture2D tex2d_baseMap : register(t1);
Texture2D tex2d_normalMap : register(t2);
Texture2D tex2d_specularMap : register(t3);

#endif
