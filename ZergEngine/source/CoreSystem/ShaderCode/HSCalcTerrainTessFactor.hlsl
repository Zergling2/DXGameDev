#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

#define NUM_CONTROL_POINTS 4

float CalcTessFactor(const float3 v)
{
    const float d = distance(v, cb_perCamera.cameraPosW);
    const float s = saturate((d - cb_perCamera.tessMinDist) / (cb_perCamera.tessMaxDist - cb_perCamera.tessMinDist));
    
    return pow(2.0f, round(lerp(cb_perCamera.maxTessExponent, cb_perCamera.minTessExponent, s)));
}

bool TestAabbBehindPlane(Aabb aabb, float4 plane)
{
    const float3 n = plane.xyz;
    
    float s = dot(n, aabb.center) + plane.w;
    float r = dot(abs(n), aabb.extent);
    
    return s + r < 0.0f;
}

bool TestFrustumAabbCollision(Aabb aabb, Frustum frustumW)
{
    for (uint i = 0; i < 6; ++i)
        if (TestAabbBehindPlane(aabb, frustumW.plane[i].m_equation))
            return false;
    
    return true;
}

DSInputQuadPatchTess CHSTerrainRendering(InputPatch<HSInputTerrainPatchCtrlPt, NUM_CONTROL_POINTS> patch,
    uint patchId : SV_PrimitiveID)
{
    DSInputQuadPatchTess output;
    
    // 패치 정점 모양
    // [0]     [1]최대
    //
    // 
    // [2]최소 [3]
    const float3 patchMinCoord = float3(patch[2].posW.x, patch[0].boundsY.x, patch[2].posW.z);
    const float3 patchMaxCoord = float3(patch[1].posW.x, patch[0].boundsY.y, patch[1].posW.z);
    Aabb aabb;
    aabb.center = (patchMinCoord + patchMaxCoord) * 0.5f;
    aabb.extent = (patchMaxCoord - patchMinCoord) * 0.5f;
    
    if (!TestFrustumAabbCollision(aabb, cb_perCamera.frustumW))
    {
        // 패치를 파이프라인에서 제거
        output.edgeTessFactor[0] = 0.0f;
        output.edgeTessFactor[1] = 0.0f;
        output.edgeTessFactor[2] = 0.0f;
        output.edgeTessFactor[3] = 0.0f;
        
        output.insideTessFactor[0] = 0.0f;
        output.insideTessFactor[1] = 0.0f;
    }
    else
    {
        // [0]  e1  [1]
        //
        // e0   c   e2
        //
        // [2]  e3  [3]
    
        // 패치 모서리의 중앙과 카메라의 거리를 기준으로 테셀레이션 정도를 결정한다.
        const float3 edge0 = (patch[2].posW + patch[0].posW) * 0.5f;
        const float3 edge1 = (patch[0].posW + patch[1].posW) * 0.5f;
        const float3 edge2 = (patch[1].posW + patch[3].posW) * 0.5f;
        const float3 edge3 = (patch[3].posW + patch[2].posW) * 0.5f;
        const float3 center = (patch[0].posW + patch[1].posW + patch[2].posW + patch[3].posW) * 0.25f;
	
        output.edgeTessFactor[0] = CalcTessFactor(edge0);
        output.edgeTessFactor[1] = CalcTessFactor(edge1);
        output.edgeTessFactor[2] = CalcTessFactor(edge2);
        output.edgeTessFactor[3] = CalcTessFactor(edge3);
	
        output.insideTessFactor[0] = output.insideTessFactor[1] = CalcTessFactor(center);
    }

	return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]       // 사각 패치는 유지
[patchconstantfunc("CHSTerrainRendering")]
[maxtessfactor(64.0f)]
DSInputTerrainPatchCtrlPt main(InputPatch<HSInputTerrainPatchCtrlPt, NUM_CONTROL_POINTS> patch,
	uint cpId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DSInputTerrainPatchCtrlPt output;
    
    output.posW = patch[cpId].posW;
    output.texCoord = patch[cpId].texCoord;

	return output;
}
