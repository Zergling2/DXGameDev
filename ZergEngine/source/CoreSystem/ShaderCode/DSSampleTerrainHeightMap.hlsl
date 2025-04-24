#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
PerTerrain

[Texture]
HeightMap t0

[Sampler State]
HeightMap Sampler
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb1 : register(b1)
{
    CbPerTerrain cb_perTerrain;
}

#define NUM_CONTROL_POINTS 4

Texture2D tex2d_heightMap : register(t0);

[domain("quad")]
PSInputTerrainFragment main(DSInputQuadPatchTess patchTess, float2 domain : SV_DomainLocation,
	const OutputPatch<DSInputTerrainPatchCtrlPt, NUM_CONTROL_POINTS> patch)
{
    PSInputTerrainFragment output;
    
    output.posW = lerp(
        lerp(patch[0].posW, patch[1].posW, domain.x),
        lerp(patch[2].posW, patch[3].posW, domain.x),
        domain.y
    );
    
    output.texCoord = lerp(
        lerp(patch[0].texCoord, patch[1].texCoord, domain.x),
        lerp(patch[2].texCoord, patch[3].texCoord, domain.x),
        domain.y
    );
    
    output.tiledTexCoord = output.texCoord * cb_perTerrain.terrainTextureTiling;
    
    // Displacement mapping
    output.posW.y = tex2d_heightMap.SampleLevel(ss_heightmapSampler, output.texCoord, 0.0f).r; // DXGI_FORMAT_R16_FLOAT
    output.posH = mul(float4(output.posW, 1.0f), cb_perCamera.vp);
    
    return output;
}
