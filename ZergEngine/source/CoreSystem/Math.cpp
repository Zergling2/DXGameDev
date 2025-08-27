#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\DataStructure\Ray.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>
#include <ZergEngine\CoreSystem\DataStructure\Frustum.h>
#include <ZergEngine\CoreSystem\DataStructure\Triangle.h>

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

void XM_CALLCONV Math::CalcWorldFrustumFromViewProjMatrix(FXMMATRIX viewProj, Frustum& frustum) noexcept
{
	XMFLOAT4X4A viewProjMatrix;
	XMStoreFloat4x4A(&viewProjMatrix, viewProj);

	// Left
	frustum.m_plane[0].m_equation.x = viewProjMatrix._11 + viewProjMatrix._14;
	frustum.m_plane[0].m_equation.y = viewProjMatrix._21 + viewProjMatrix._24;
	frustum.m_plane[0].m_equation.z = viewProjMatrix._31 + viewProjMatrix._34;
	frustum.m_plane[0].m_equation.w = viewProjMatrix._41 + viewProjMatrix._44;

	// Right
	frustum.m_plane[1].m_equation.x = viewProjMatrix._14 - viewProjMatrix._11;
	frustum.m_plane[1].m_equation.y = viewProjMatrix._24 - viewProjMatrix._21;
	frustum.m_plane[1].m_equation.z = viewProjMatrix._34 - viewProjMatrix._31;
	frustum.m_plane[1].m_equation.w = viewProjMatrix._44 - viewProjMatrix._41;

	// Bottom
	frustum.m_plane[2].m_equation.x = viewProjMatrix._12 + viewProjMatrix._14;
	frustum.m_plane[2].m_equation.y = viewProjMatrix._22 + viewProjMatrix._24;
	frustum.m_plane[2].m_equation.z = viewProjMatrix._32 + viewProjMatrix._34;
	frustum.m_plane[2].m_equation.w = viewProjMatrix._42 + viewProjMatrix._44;
	
	// Top
	frustum.m_plane[3].m_equation.x = viewProjMatrix._14 - viewProjMatrix._12;
	frustum.m_plane[3].m_equation.y = viewProjMatrix._24 - viewProjMatrix._22;
	frustum.m_plane[3].m_equation.z = viewProjMatrix._34 - viewProjMatrix._32;
	frustum.m_plane[3].m_equation.w = viewProjMatrix._44 - viewProjMatrix._42;
	
	// Near
	frustum.m_plane[4].m_equation.x = viewProjMatrix._13;
	frustum.m_plane[4].m_equation.y = viewProjMatrix._23;
	frustum.m_plane[4].m_equation.z = viewProjMatrix._33;
	frustum.m_plane[4].m_equation.w = viewProjMatrix._43;

	// Far
	frustum.m_plane[5].m_equation.x = viewProjMatrix._14 - viewProjMatrix._13;
	frustum.m_plane[5].m_equation.y = viewProjMatrix._24 - viewProjMatrix._23;
	frustum.m_plane[5].m_equation.z = viewProjMatrix._34 - viewProjMatrix._33;
	frustum.m_plane[5].m_equation.w = viewProjMatrix._44 - viewProjMatrix._43;

	for (size_t i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&frustum.m_plane[i].m_equation));	// 정규화해서 전달 (GPU에서 정규화 생략)
		XMStoreFloat4A(&frustum.m_plane[i].m_equation, v);
	}
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
	for (size_t i = 0; i < 6; ++i)
		if (Math::TestAabbBehindPlane(aabb, frustum.m_plane[i]))
			return false;

	return true;
}

bool Math::TestAabbBehindPlane(const Aabb& aabb, const Plane& plane)
{
	PlaneIntersectionType pit = aabb.Intersects(XMLoadFloat4A(&plane.m_equation));

	return pit == BACK;
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

const Aabb XM_CALLCONV Math::TransformAabb(const Aabb& aabb, FXMMATRIX m)
{
	// Load center and extents.
	Aabb result;
	aabb.Transform(result, m);

	return result;
}
