#include "ShaderCommon.hlsli"

// [Sampler State]
// g_ssBilinear

// [Constant Buffer]
cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb1 : register(b1)
{
    CbPerTerrain cb_perTerrain;
}

// [Texture]
Texture2D<float1> tex2d_heightMap : register(t0);
Texture2D<float4> tex2d_normalMap : register(t1);

#define NUM_CONTROL_POINTS 4

[domain("quad")]
PSInputTerrainFragment main(DSInputQuadPatchTess patchTess, float2 domain : SV_DomainLocation,
	const OutputPatch<DSInputTerrainPatchCtrlPt, NUM_CONTROL_POINTS> patch)
{
    PSInputTerrainFragment output;
    
    float2 texCoord = lerp(
        lerp(patch[0].texCoord, patch[1].texCoord, domain.x),
        lerp(patch[2].texCoord, patch[3].texCoord, domain.x),
        domain.y
    );
    
    float3 posW = lerp(
        lerp(patch[0].posW, patch[1].posW, domain.x),
        lerp(patch[2].posW, patch[3].posW, domain.x),
        domain.y
    );
    // Displacement mapping
    // HeightMap이 R16_UNORM 텍스쳐를 사용하므로 실제 높이 값을 담지 않음에 유의 (상수 버퍼로 전달된 최대 높이 값을 곱해야 함)
    posW.y = tex2d_heightMap.SampleLevel(g_ssBilinear, texCoord, 0) * cb_perTerrain.maxHeight;
    
    output.posW = posW;
    float4 posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    output.pos = posH;
    
    output.normalW = tex2d_normalMap.SampleLevel(g_ssBilinear, texCoord, 0).xyz;    // XMHALF4로 전달된 xyz 성분
    output.texCoord = texCoord;
    output.tiledTexCoord = texCoord * cb_perTerrain.tilingScale;
    
    return output;
}
