// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HLSL Structs
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

#define IsUsingMaterial(mtlFlag)    ((mtlFlag) & 0x00000001)
#define IsUsingLightMap(mtlFlag)    ((mtlFlag) & 0x00000002)
#define IsUsingBaseMap(mtlFlag)     ((mtlFlag) & 0x00000004)
#define IsUsingNormalMap(mtlFlag)   ((mtlFlag) & 0x00000008)
#define IsUsingSpecularMap(mtlFlag) ((mtlFlag) & 0x00000010)

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
    uint cb_directionalLightCount;
    uint cb_pointLightCount;
    uint cb_spotLightCount;
    uint cb_terrainMtlCount;

    float cb_terrainTextureTiling;  // 도메인 셰이더에서 Tiled 텍스쳐 좌표 계산 시 필요
    float cb_terrainCellSpacing;    // 픽셀 셰이더에서 노말 계산 시 필요
    float cb_terrainTexelSpacingU;  // 픽셀 셰이더에서 노말 계산 시 필요
    float cb_terrainTexelSpacingV;  // 픽셀 셰이더에서 노말 계산 시 필요
    
    Material cb_terrainMtl[5];

    DirectionalLight cb_dl[4];
    PointLight cb_pl[4];
    SpotLight cb_sl[4];
}

cbuffer PerCamera : register(b1)
{
    float3 cb_camPosW;
    float cb_pad;
    
    float cb_tessMinDist;
    float cb_tessMaxDist;
    float cb_minTessExponent;
    float cb_maxTessExponent;
    
    float4 cb_frustumPlane[6];
}

cbuffer PerMesh : register(b2)
{
    float4x4 cb_w;
    float4x4 cb_wInvTr;
    float4x4 cb_wvp;
};

cbuffer PerSubset : register(b3)
{
    Material cb_subsetMtl;
}

cbuffer PerRenderingResultMerge : register(b4)
{
    float cb_width;
    float cb_height;
    float cb_topLeftX;
    float cb_topLeftY;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Sampler States
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
SamplerState g_meshTexSampler : register(s0);

SamplerState g_heightMapSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState g_skyboxSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Textures
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Texture2DMS<float4> g_renderingResult : register(t0);
TextureCube g_skyboxCubeMap : register(t0);
Texture2D g_heightMap : register(t0);
Texture2D g_lightMap : register(t0);
Texture2D g_baseMap : register(t1);
Texture2D g_normalMap : register(t2);
Texture2D g_specularMap : register(t3);

void ComputeDirectionalLight(uint dlIndex, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = cb_dl[dlIndex].ambient * cb_subsetMtl.ambient;
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    const float kd = max(dot(-cb_dl[dlIndex].directionW, normal), 0.0f);
    
    // 난반사광이 닿지 않는 표면에는 정반사광이 존재하지 않는다.
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(cb_dl[dlIndex].directionW, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cb_subsetMtl.specular.w);
        
        oD = kd * cb_dl[dlIndex].diffuse * cb_subsetMtl.diffuse;
        oS = ks * cb_dl[dlIndex].specular * cb_subsetMtl.specular;
    }
}

void ComputePointLight(uint plIndex, float3 pos, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 L = cb_pl[plIndex].positionW - pos;
    const float d = length(L);
    
    if (d > cb_pl[plIndex].range)
        return;
    
    L /= d;
    
    oA = cb_pl[plIndex].ambient * cb_subsetMtl.ambient;
    
    const float kd = max(dot(L, normal), 0.0f);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-L, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cb_subsetMtl.specular.w);
        
        const float4 D = cb_pl[plIndex].diffuse * cb_subsetMtl.diffuse;
        const float4 S = cb_pl[plIndex].specular * cb_subsetMtl.specular;
        
        const float att = 1.0f / dot(cb_pl[plIndex].att, float3(1.0f, d, d * d));
        
        oD = att * kd * D;
        oS = att * ks * S;
    }
}

void ComputeSpotLight(uint slIndex, float3 pos, float3 normal, float3 toEye, out float4 oA, out float4 oD, out float4 oS)
{
    oA = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oD = float4(0.0f, 0.0f, 0.0f, 0.0f);
    oS = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 L = cb_sl[slIndex].positionW - pos;
    const float d = length(L);
    
    if (d > cb_sl[slIndex].range)
        return;
    
    L /= d;
    
    oA = cb_sl[slIndex].ambient * cb_subsetMtl.ambient;
    
    const float kd = max(dot(L, normal), 0.0f);
    
    [flatten]
    if (kd > 0.0f)
    {
        const float3 r = reflect(-L, normal);
        const float ks = pow(max(dot(toEye, r), 0.0f), cb_subsetMtl.specular.w);
        
        const float4 D = cb_sl[slIndex].diffuse * cb_subsetMtl.diffuse;
        const float4 S = cb_sl[slIndex].specular * cb_subsetMtl.specular;
        
        const float att = 1.0f / dot(cb_sl[slIndex].att, float3(1.0f, d, d * d));
        
        oD = att * kd * D;
        oS = att * ks * S;
    }
    
    const float kspot = pow(max(dot(-L, cb_sl[slIndex].directionW), 0.0f), cb_sl[slIndex].spotExp);
    
    oA *= kspot;
    oD *= kspot;
    oS *= kspot;
}
