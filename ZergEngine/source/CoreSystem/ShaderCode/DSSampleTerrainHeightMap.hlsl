#include "ShaderCommon.hlsli"

// [Sampler State]
// ss_bilinear

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
    // HeightMap�� R16_UNORM �ؽ��ĸ� ����ϹǷ� ���� ���� ���� ���� ������ ���� (��� ���۷� ���޵� �ִ� ���� ���� ���ؾ� ��)
    posW.y = tex2d_heightMap.SampleLevel(ss_bilinear, texCoord, 0) * cb_perTerrain.maxHeight;
    
    output.posW = posW;
    output.posH = mul(float4(posW, 1.0f), cb_perCamera.vp);
    
    output.normalW = tex2d_normalMap.SampleLevel(ss_bilinear, texCoord, 0).xyz;
    
    output.texCoord = texCoord;
    output.tiledTexCoord = texCoord * cb_perTerrain.tilingScale;
    
    return output;
}
