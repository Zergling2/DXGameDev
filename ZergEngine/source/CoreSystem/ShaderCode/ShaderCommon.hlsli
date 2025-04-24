#ifndef __SHADER_COMMON__
#define __SHADER_COMMON__

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Constants & Macro
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define MAX_GLOBAL_LIGHT_COUNT 4
#define hlslstruct struct
#define HLSLPad float
#define uint32_t uint
#define FLOAT float
#define XMFLOAT3 float3
#define XMFLOAT3A float3
#define XMFLOAT4A float4
#define XMFLOAT4X4A float4x4

#define NO_MATERIAL_COLOR float4(1.0f, 1.0f, 1.0f, 1.0f)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HLSL Structs
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define IsUsingMaterial(mtlFlag)    ((mtlFlag) & 0x80000000)
#define IsUsingLightMap(mtlFlag)    ((mtlFlag) & 0x00000001)
#define IsUsingDiffuseMap(mtlFlag)  ((mtlFlag) & 0x00000002)
#define IsUsingNormalMap(mtlFlag)   ((mtlFlag) & 0x00000004)
#define IsUsingSpecularMap(mtlFlag) ((mtlFlag) & 0x00000008)
hlslstruct MaterialData
{
    uint32_t mtlFlag;
	HLSLPad pad0;
	HLSLPad pad1;
	HLSLPad pad2;

	// Material
	XMFLOAT4A ambient;
	XMFLOAT4A diffuse;
	XMFLOAT4A specular; // r/g/b/p
	XMFLOAT4A reflect;
};

hlslstruct DirectionalLightData
{
    XMFLOAT4A ambient;
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 directionW;
	HLSLPad pad;
};

hlslstruct PointLightData
{
    XMFLOAT4A ambient;
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 positionW;
    FLOAT range;

	XMFLOAT3 att; // a0/a1/a2     a0 + a1d + a2d^2
	HLSLPad pad;
};

hlslstruct SpotLightData
{
    XMFLOAT4A ambient;
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 positionW;
	FLOAT range;

	XMFLOAT3 directionW;
	FLOAT spotExp;

	XMFLOAT3 att; // a0/a1/a2     a0 + a1d + a2d^2
	HLSLPad pad;
};

struct HSInputTerrainPatchCtrlPt
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

struct DSInputTerrainPatchCtrlPt
{
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PSInputPFragment
{
    float4 posH : SV_Position;      // No lighting, No texturing
};

struct PSInputPCFragment
{
    float4 posH : SV_Position;
    float4 color : COLOR;
};

struct PSInputPNFragment
{
    float4 posH : SV_Position;      // Homogeneous clip space position
    float3 posW : POSITION;         // World space position (조명 처리를 위해서)
    float3 normalW : NORMAL;
};

struct PSInputPTFragment
{
    float4 posH : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct PSInputPNTFragment
{
    float4 posH : SV_Position;      // Homogeneous clip space position
    float3 posW : POSITION;         // World space position (조명 처리를 위해서)
    float3 normalW : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct PSInputSkyboxFragment
{
    float4 posH : SV_Position;
    float3 posL : POSITION;         // 로컬 벡터를 그대로 큐브맵 샘플링 벡터로 사용하면 됨
};

struct PSInputTerrainFragment
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
hlslstruct CbPerFrame
{
	uint32_t dlCount;
	uint32_t plCount;
	uint32_t slCount;

    DirectionalLightData dl[MAX_GLOBAL_LIGHT_COUNT];
    PointLightData pl[MAX_GLOBAL_LIGHT_COUNT];
    SpotLightData sl[MAX_GLOBAL_LIGHT_COUNT];
};

hlslstruct CbPerCamera
{
	XMFLOAT3 cameraPosW;
	HLSLPad pad0;

	FLOAT tessMinDist;
	FLOAT tessMaxDist;
	FLOAT minTessExponent;
	FLOAT maxTessExponent;

	XMFLOAT4A frustumPlane[6];

    XMFLOAT4X4A vp; // View * Proj
};

hlslstruct CbPerMesh
{
	XMFLOAT4X4A w; // World
	XMFLOAT4X4A wInvTr; // Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)
};

hlslstruct CbPerTerrain
{
	XMFLOAT4X4A w; // World
	XMFLOAT4X4A wInvTr; // Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)

	FLOAT terrainTextureTiling; // 도메인 셰이더에서 Tiled 텍스쳐 좌표 계산 시 필요
	FLOAT terrainCellSpacing; // 픽셀 셰이더에서 노말 계산 시 필요
	FLOAT terrainTexelSpacingU; // 픽셀 셰이더에서 노말 계산 시 필요
	FLOAT terrainTexelSpacingV; // 픽셀 셰이더에서 노말 계산 시 필요

	uint32_t terrainMtlCount;
    MaterialData terrainMtl[5];
};

hlslstruct CbPerCameraMerge
{
	FLOAT width;
	FLOAT height;
	FLOAT topLeftX;
	FLOAT topLeftY;
};

hlslstruct CbPerSubset
{
    MaterialData mtl;
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Sampler States
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
SamplerState ss_meshTexSampler : register(s0);
SamplerState ss_skyboxSampler : register(s1);
SamplerState ss_terrainTexSampler : register(s2);
SamplerState ss_heightmapSampler : register(s3);

#endif
