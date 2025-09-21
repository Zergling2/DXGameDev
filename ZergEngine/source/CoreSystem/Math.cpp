#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\DataStructure\Ray.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>
#include <ZergEngine\CoreSystem\DataStructure\Frustum.h>
#include <ZergEngine\CoreSystem\DataStructure\Triangle.h>
#include <cmath>

using namespace ze;

XMVECTOR XM_CALLCONV Math::QuaternionToEulerNormal(FXMVECTOR quaternion) noexcept
{
	float qx = XMVectorGetX(quaternion);
	float qy = XMVectorGetY(quaternion);
	float qz = XMVectorGetZ(quaternion);
	float qw = XMVectorGetW(quaternion);

	XMFLOAT3A euler;
	// X (Pitch)
	float sinp = 2.0f * (qw * qx - qy * qz);
	if (std::fabs(sinp) >= 1.0f)
		euler.x = std::copysign(XM_PIDIV2, sinp); // 90도 or -90도
	else
		euler.x = std::asin(sinp);

	// Z (Roll)
	float sinr_cosp = 2.0f * (qw * qz + qx * qy);
	float cosr_cosp = 1.0f - 2.0f * (qx * qx + qz * qz);
	euler.z = std::atan2(sinr_cosp, cosr_cosp);

	// Y (Yaw)
	float siny_cosp = 2.0f * (qw * qy + qx * qz);
	float cosy_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
	euler.y = std::atan2(siny_cosp, cosy_cosp);

	// X(Pitch), Y(Yaw), Z(Roll) 순서로 XMVECTOR에 패킹
	return XMLoadFloat3A(&euler);
}

bool Math::TestRayAabbCollision(const Ray& ray, const Aabb& aabb)
{
	XMVECTOR origin = XMLoadFloat3A(&ray.m_origin);
	XMVECTOR direction = XMLoadFloat3A(&ray.m_direction);
	float dist;

	return aabb.Intersects(origin, direction, dist);
}

bool Math::TestFrustumAabbCollision(const Frustum& frustum, const Aabb& aabb)
{
	return frustum.Intersects(aabb);
}

bool Math::TestRayTriangleCollision(const Ray& ray, const Triangle& tri)
{
	XMVECTOR origin = XMLoadFloat3A(&ray.m_origin);
	XMVECTOR direction = XMLoadFloat3A(&ray.m_direction);
	XMVECTOR v0 = XMLoadFloat3A(&tri.m_v[0]);
	XMVECTOR v1 = XMLoadFloat3A(&tri.m_v[1]);
	XMVECTOR v2 = XMLoadFloat3A(&tri.m_v[2]);
	float dist;

	return DirectX::TriangleTests::Intersects(origin, direction, v0, v1, v2, dist);
}

void XM_CALLCONV Math::CalcFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4* pPlanes)
{
	XMFLOAT4X4A viewProjMatrix;
	XMStoreFloat4x4A(&viewProjMatrix, viewProj);

	// Left
	pPlanes[0].x = viewProjMatrix._11 + viewProjMatrix._14;
	pPlanes[0].y = viewProjMatrix._21 + viewProjMatrix._24;
	pPlanes[0].z = viewProjMatrix._31 + viewProjMatrix._34;
	pPlanes[0].w = viewProjMatrix._41 + viewProjMatrix._44;

	// Right
	pPlanes[1].x = viewProjMatrix._14 - viewProjMatrix._11;
	pPlanes[1].y = viewProjMatrix._24 - viewProjMatrix._21;
	pPlanes[1].z = viewProjMatrix._34 - viewProjMatrix._31;
	pPlanes[1].w = viewProjMatrix._44 - viewProjMatrix._41;

	// Bottom
	pPlanes[2].x = viewProjMatrix._12 + viewProjMatrix._14;
	pPlanes[2].y = viewProjMatrix._22 + viewProjMatrix._24;
	pPlanes[2].z = viewProjMatrix._32 + viewProjMatrix._34;
	pPlanes[2].w = viewProjMatrix._42 + viewProjMatrix._44;

	// Top
	pPlanes[3].x = viewProjMatrix._14 - viewProjMatrix._12;
	pPlanes[3].y = viewProjMatrix._24 - viewProjMatrix._22;
	pPlanes[3].z = viewProjMatrix._34 - viewProjMatrix._32;
	pPlanes[3].w = viewProjMatrix._44 - viewProjMatrix._42;

	// Near
	pPlanes[4].x = viewProjMatrix._13;
	pPlanes[4].y = viewProjMatrix._23;
	pPlanes[4].z = viewProjMatrix._33;
	pPlanes[4].w = viewProjMatrix._43;

	// Far
	pPlanes[5].x = viewProjMatrix._14 - viewProjMatrix._13;
	pPlanes[5].y = viewProjMatrix._24 - viewProjMatrix._23;
	pPlanes[5].z = viewProjMatrix._34 - viewProjMatrix._33;
	pPlanes[5].w = viewProjMatrix._44 - viewProjMatrix._43;

	for (size_t i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&pPlanes[i]));	// 정규화해서 전달 (GPU에서 정규화 생략)
		XMStoreFloat4(&pPlanes[i], v);
	}
}

void XM_CALLCONV Math::CalcFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4A* pPlanes)
{
	XMFLOAT4X4A viewProjMatrix;
	XMStoreFloat4x4A(&viewProjMatrix, viewProj);

	// Left
	pPlanes[0].x = viewProjMatrix._11 + viewProjMatrix._14;
	pPlanes[0].y = viewProjMatrix._21 + viewProjMatrix._24;
	pPlanes[0].z = viewProjMatrix._31 + viewProjMatrix._34;
	pPlanes[0].w = viewProjMatrix._41 + viewProjMatrix._44;

	// Right
	pPlanes[1].x = viewProjMatrix._14 - viewProjMatrix._11;
	pPlanes[1].y = viewProjMatrix._24 - viewProjMatrix._21;
	pPlanes[1].z = viewProjMatrix._34 - viewProjMatrix._31;
	pPlanes[1].w = viewProjMatrix._44 - viewProjMatrix._41;

	// Bottom
	pPlanes[2].x = viewProjMatrix._12 + viewProjMatrix._14;
	pPlanes[2].y = viewProjMatrix._22 + viewProjMatrix._24;
	pPlanes[2].z = viewProjMatrix._32 + viewProjMatrix._34;
	pPlanes[2].w = viewProjMatrix._42 + viewProjMatrix._44;

	// Top
	pPlanes[3].x = viewProjMatrix._14 - viewProjMatrix._12;
	pPlanes[3].y = viewProjMatrix._24 - viewProjMatrix._22;
	pPlanes[3].z = viewProjMatrix._34 - viewProjMatrix._32;
	pPlanes[3].w = viewProjMatrix._44 - viewProjMatrix._42;

	// Near
	pPlanes[4].x = viewProjMatrix._13;
	pPlanes[4].y = viewProjMatrix._23;
	pPlanes[4].z = viewProjMatrix._33;
	pPlanes[4].w = viewProjMatrix._43;

	// Far
	pPlanes[5].x = viewProjMatrix._14 - viewProjMatrix._13;
	pPlanes[5].y = viewProjMatrix._24 - viewProjMatrix._23;
	pPlanes[5].z = viewProjMatrix._34 - viewProjMatrix._33;
	pPlanes[5].w = viewProjMatrix._44 - viewProjMatrix._43;

	for (size_t i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&pPlanes[i]));	// 정규화해서 전달 (GPU에서 정규화 생략)
		XMStoreFloat4A(&pPlanes[i], v);
	}
}
