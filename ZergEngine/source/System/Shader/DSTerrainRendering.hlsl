#include "ShaderCommon.hlsli"

#define NUM_CONTROL_POINTS 4

[domain("quad")]
PSInputTerrainColoring main(DSInputQuadPatchTess patchTess, float2 domain : SV_DomainLocation,
	const OutputPatch<DSInputTerrainPatchControlPoint, NUM_CONTROL_POINTS> patch)
{
    PSInputTerrainColoring output;
    
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
    
    output.tiledTexCoord = output.texCoord * cb_terrainTextureTiling;
    
    // Displacement mapping
    output.posW.y = g_heightMap.SampleLevel(g_heightMapSampler, output.texCoord, 0.0f).r; // DXGI_FORMAT_R16_FLOAT
    output.posH = mul(float4(output.posW, 1.0f), cb_wvp);
    
    return output;
}
