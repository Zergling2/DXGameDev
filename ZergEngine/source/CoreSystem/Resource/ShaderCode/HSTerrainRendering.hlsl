#include "ShaderCommon.hlsli"

#define NUM_CONTROL_POINTS 4

float CalcTessFactor(const float3 v)
{
    const float d = distance(v, cbpc_cameraPosW);
    const float s = saturate((d - cbpc_tessMinDist) / (cbpc_tessMaxDist - cbpc_tessMinDist));
    
    return pow(2, lerp(cbpc_maxTessExponent, cbpc_minTessExponent, s));
}

bool IsAabbBehindPlane(float3 aabbCenter, float3 aabbExtents, float4 plane)
{
    const float3 n = abs(plane.xyz);  // plane�� ����ȭ�Ǿ��ִ� ����̴�.
    
    const float r = dot(aabbExtents, n);  // n ���� ����ȭ �����̹Ƿ� extents�� n�� �����ϸ� extent�� ���� n�� ������ ���̸� ���� �� �ִ�.
    
    // ����� ���� ���Ϳ� Aabb center ��ǥ�� ���� + �������κ��� �������� �Ÿ�
    const float s = dot(float4(aabbCenter, 1.0f), plane);
    
    return s + r < 0.0f;
}

bool IsAabbOutsideFrustum(float3 aabbCenter, float3 aabbExtents, float4 frustumPlane[6])
{
    for (uint i = 0; i < 6; ++i)
        if (IsAabbBehindPlane(aabbCenter, aabbExtents, frustumPlane[i]))
            return true;
    
    return false;
}

DSInputQuadPatchTess CHSTerrainRendering(InputPatch<HSInputTerrainPatchControlPoint, NUM_CONTROL_POINTS> patch, 
    uint patchId : SV_PrimitiveID)
{
    DSInputQuadPatchTess output;
    
    // ��ġ ���� ���
    // [0]     [1]�ִ�
    //
    // 
    // [2]�ּ� [3]
    const float3 patchMinCoord = float3(patch[2].posW.x, patch[0].boundsY.x, patch[2].posW.z);
    const float3 patchMaxCoord = float3(patch[1].posW.x, patch[0].boundsY.y, patch[1].posW.z);
    const float3 aabbCenter = (patchMinCoord + patchMaxCoord) * 0.5f;
    const float3 aabbExtents = (patchMaxCoord - patchMinCoord) * 0.5f;
    
    if (IsAabbOutsideFrustum(aabbCenter, aabbExtents, cbpc_frustumPlane))
    {
        // ��ġ�� ���������ο��� ����
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
    
        // ��ġ �𼭸��� �߾Ӱ� ī�޶��� �Ÿ��� �������� �׼����̼� ������ �����Ѵ�.
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
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CHSTerrainRendering")]
[maxtessfactor(64.0f)]
DSInputTerrainPatchControlPoint main(InputPatch<HSInputTerrainPatchControlPoint, NUM_CONTROL_POINTS> patch,
	uint cpId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DSInputTerrainPatchControlPoint output;
    
    output.posW = patch[cpId].posW;
    output.texCoord = patch[cpId].texCoord;

	return output;
}
