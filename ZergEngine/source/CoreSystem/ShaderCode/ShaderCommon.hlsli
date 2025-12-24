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
#define XMFLOAT2 float2
#define XMFLOAT2A float2
#define XMFLOAT3 float3
#define XMFLOAT3A float3
#define XMFLOAT4A float4
#define XMFLOAT4X4A float4x4

#define NO_MATERIAL_COLOR float4(1.0f, 1.0f, 1.0f, 1.0f)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HLSL Structs
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// #define IsUsingMaterial(mtlFlag)    ((mtlFlag) & 0x80000000)
// #define IsUsingDiffuseMap(mtlFlag)  ((mtlFlag) & 0x00000001)
// #define IsUsingSpecularMap(mtlFlag) ((mtlFlag) & 0x00000002)
// #define IsUsingNormalMap(mtlFlag)   ((mtlFlag) & 0x00000004)
// 
// #define IsUsingDiffuseLayer(layerFlag)	((layerFlag) & 0x00000001)
// #define IsUsingSpecularLayer(layerFlag)	((layerFlag) & 0x00000002)
// #define IsUsingNormalLayer(layerFlag)	((layerFlag) & 0x00000004)

hlslstruct MaterialData
{
	// Material
	XMFLOAT4A diffuse;
	XMFLOAT4A specular; // r/g/b/p
	XMFLOAT4A reflect;
};

hlslstruct DirectionalLightData
{
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 directionW;
	HLSLPad pad0;
};

hlslstruct PointLightData
{
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 positionW;
    FLOAT range;

	XMFLOAT3 att; // a0/a1/a2     a0 + a1d + a2d^2
	HLSLPad pad0;
};

hlslstruct SpotLightData
{
	XMFLOAT4A diffuse;
	XMFLOAT4A specular;

	XMFLOAT3 positionW;
	FLOAT range;

	XMFLOAT3 directionW;
	FLOAT innerConeCos;

	XMFLOAT3 att; // a0/a1/a2     a0 + a1d + a2d^2
	FLOAT outerConeCos;
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Constant Buffers
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
hlslstruct CbPerForwardRenderingFrame
{
	uint32_t dlCount;
	uint32_t plCount;
	uint32_t slCount;
	HLSLPad pad0;

	XMFLOAT3 ambientLight;
	HLSLPad pad1;

    DirectionalLightData dl[MAX_GLOBAL_LIGHT_COUNT];
    PointLightData pl[MAX_GLOBAL_LIGHT_COUNT];
    SpotLightData sl[MAX_GLOBAL_LIGHT_COUNT];
};

hlslstruct CbPerDeferredRenderingFrame
{
	XMFLOAT3 ambientLight;
	HLSLPad pad0;
};

hlslstruct CbPerCamera
{
	XMFLOAT3 cameraPosW;
	HLSLPad pad0;

	FLOAT tessMinDist;
	FLOAT tessMaxDist;
	FLOAT minTessExponent;
	FLOAT maxTessExponent;

    XMFLOAT4A worldSpaceFrustumPlane[6];

    XMFLOAT4X4A vp; // View * Proj
};

hlslstruct CbPerArmature
{
    XMFLOAT4X4A finalTransform[96];
};

hlslstruct CbPerMesh
{
	XMFLOAT4X4A w; // World
	XMFLOAT4X4A wInvTr; // Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)
};

hlslstruct CbPerTerrain
{
	FLOAT maxHeight;
	FLOAT tilingScale;
	uint32_t layerArraySize;
	HLSLPad pad0;
};

hlslstruct CbPerCameraMerge
{
	FLOAT width;
	FLOAT height;
	FLOAT topLeftX;
	FLOAT topLeftY;
};

hlslstruct CbMaterial
{
    MaterialData mtl;
};

hlslstruct CbPerBillboard
{
    XMFLOAT4X4A w;
    XMFLOAT4X4A wInvTr;
    
    // 텍스쳐 팔레트용 정보
	XMFLOAT2 uvOffset; // UV 위치 오프셋
	XMFLOAT2 uvScale; // UV 스케일
};

hlslstruct Cb2DRender
{
    XMFLOAT2 toNDCSpaceRatio;
};

hlslstruct CbPer2DQuad
{
	XMFLOAT2 size;
	XMFLOAT2 position;

	// 텍스쳐 팔레트용 정보
	XMFLOAT2 uvOffset; // UV 위치 오프셋
	XMFLOAT2 uvScale; // UV 스케일

    XMFLOAT4A color;
	
	FLOAT ltColorWeight; // 음영 쿼드 렌더링 시 사용
	FLOAT rbColorWeight; // 음영 쿼드 렌더링 시 사용
};

/*
hlslstruct CbPerShaded2DQuad
{
    XMFLOAT4A color;
    
	XMFLOAT2 size;
	XMFLOAT2 position;
    
	uint32_t colorWeightIndex; // [0] Convex shade [1] Concave shade
};
*/

hlslstruct CbPerCheckbox
{
	XMFLOAT4A boxColor;
	XMFLOAT4A checkColor;
	
	XMFLOAT2 size;
	XMFLOAT2 position;
};

hlslstruct Aabb
{
	float3 center;
	float3 extent;
};

struct VSInputVertexPNTT
{
    float3 posL : POSITION;
    float3 normalL : NORMAL; // Local space normal vector
    float3 tangentL : TANGENT; // Local space tangent vector
    float2 texCoord : TEXCOORD;
};

struct HSInputTerrainPatchCtrlPt
{
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD;
    float2 boundsY : YBOUNDS;
};

struct DSInputQuadPatchTess
{
    float edgeTessFactor[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

struct DSInputTerrainPatchCtrlPt
{
    float3 posW : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInputPFragment
{
    float4 pos : SV_Position;
};

struct PSInputPCFragment
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct PSInputPNFragment
{
    float4 pos : SV_Position;
    float3 posW : POSITION;         // World space position
    float3 normalW : NORMAL;
};

struct PSInputPTFragment
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct PSInputPNTFragment
{
    float4 pos : SV_Position;
    float3 posW : POSITION;		// World space position
    float3 normalW : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct PSInputPNTTFragment
{
    float4 pos : SV_Position;
    float3 posW : POSITION;		// World space position
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

struct PSInputPNTTShadowFragment
{
    float4 pos : SV_Position;
    float3 posW : POSITION; // World space position
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
    float4 shadowPosH : SHADOWPOS;
};

struct PSInputSkyboxFragment
{
    float4 pos : SV_Position;
    float3 texCoord : TEXCOORD;
};

struct PSInputTerrainFragment
{
    float4 pos : SV_Position;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 texCoord : TEXCOORD0;
    float2 tiledTexCoord : TEXCOORD1;
};

struct PSInputShadedEdgeQuadFragment
{
    float4 pos : SV_Position;
    float2 posV : POSITION;		// View space position
    float2 texCoord : TEXCOORD;
};

struct PSOutput
{
    float4 color : SV_Target;
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Sampler States
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Global Samplers
SamplerState g_ssCommon : register(s0);
SamplerState g_ssNormalMap : register(s1);
SamplerState g_ssBilinear : register(s2);

#endif
