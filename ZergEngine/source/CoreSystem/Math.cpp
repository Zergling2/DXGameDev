#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\DataStructure\Ray.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>
#include <ZergEngine\CoreSystem\DataStructure\Frustum.h>

using namespace ze;

const XMVECTORF32 Math::Vector3::UP = XMVECTORF32{ 0.0f, 1.0f, 0.0f, 0.0f };
const XMVECTORF32 Math::Vector3::RIGHT = XMVECTORF32{ 1.0f, 0.0f, 0.0f, 0.0f };
const XMVECTORF32 Math::Vector3::FORWARD = XMVECTORF32{ 0.0f, 0.0f, 1.0f, 0.0f };

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
		euler.x = std::copysign(XM_PIDIV2, sinp); // 90�� or -90��
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

	// X(Pitch), Y(Yaw), Z(Roll) ������ XMVECTOR�� ��ŷ
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
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&frustum.m_plane[i].m_equation));	// ����ȭ�ؼ� ���� (GPU���� ����ȭ ����)
		XMStoreFloat4A(&frustum.m_plane[i].m_equation, v);
	}
}

bool Math::TestRayAabbCollision(const Ray& ray, const Aabb& aabb)
{
	// Ray ǥ����
	// O + tD (O = ������, D = ���⺤��, t = [0, INF))

	// ex)
	// AABB�� min�� �����ϴ��� �˻�
	// -> O + tD = AABB.min 
	// x, y, z �� ���к��� �������� �����
	// O.x + tD.x = AABB.min.x	=> t�� ���ؼ� �����ϸ� t = (AABB.min.x - O.x) / D.x
	// ������ y, z�� ����
	// t = (AABB.min.y - O.y) / D.y
	// t = (AABB.min.z - O.z) / D.z

	// AABB�� max�� ���� ��굵 ���� (AABB.min�� AABB.max�� ��ü�ϸ� ������ �ϼ�)

	XMVECTOR origin = XMLoadFloat3A(&ray.m_origin);
	XMVECTOR dir = XMLoadFloat3A(&ray.m_direction);
	XMVECTOR extent = XMLoadFloat3A(&aabb.m_extent);
	XMVECTOR aabbMin = XMVectorSubtract(XMLoadFloat3A(&aabb.m_center), extent);
	XMVECTOR aabbMax = XMVectorAdd(XMLoadFloat3A(&aabb.m_center), extent);

	// Ray�� ���⺤�� ���� (Direction���� ������ ��� �������� ��ü ����)
	XMVECTOR invDir = XMVectorReciprocal(dir);

	// SIMD ����ȭ �������� �ѹ��� x, y, z ���п� ���� ������ ��� ���
	// t = (AABB.min - O) / D
	// t = (AABB.max - O) / D
	XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(aabbMin, origin), invDir);
	XMVECTOR t2 = XMVectorMultiply(XMVectorSubtract(aabbMax, origin), invDir);

	// t1, t2 �������� t���� ���� ���� AABB ������, ū ���� AABB ������ �� (�� ���к��� �������� ������ ���� t1�� t2�� �������� �� ����)
	XMVECTOR tMinVec = XMVectorMin(t1, t2);
	XMVECTOR tMaxVec = XMVectorMax(t1, t2);

	const float tMinX = XMVectorGetX(tMinVec);
	const float tMinY = XMVectorGetY(tMinVec);
	const float tMinZ = XMVectorGetZ(tMinVec);

	const float tMaxX = XMVectorGetX(tMaxVec);
	const float tMaxY = XMVectorGetY(tMaxVec);
	const float tMaxZ = XMVectorGetZ(tMaxVec);

	// tMinVec �߿��� ���� ū ��, tMaxVec �߿��� ���� ���� �� ���̰� ���� ����(�ڽ� ���� Ray�� �������� ����)�̴�.
	float tMin = std::max(std::max(tMinX, tMinY), tMinZ);
	float tMax = std::min(std::min(tMaxX, tMaxY), tMaxZ);

	if (tMax < 0.0f) // AABB�� ray ����
		return false;

	if (tMin > tMax) // ���� ���� ����
		return false;

	return true;
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
	// ����
	// ����� ���� �������� ���� �ָ��ִ� AABB�� ������ �˾Ƴ��� �Ѵ�.
	// ������ ���� 8�� ������ �˻��� �ʿ�� ����.
	// �켱 ����� ���� �������� ���� �ָ� �ִ� AABB ������ ���� ���̴� �˾Ƴ� �� �ִ�.
	// �̰� ��� ������ ���񰪰� AABB�� extent�� �����ϸ� �ȴ�. (�� �� positive ���и� �ֱ� ������ ���� ���� ���̸� ���� �� ����)
	// �׸��� �� ������ �밢������ ���ֺ��� ������ ����� �ݴ��ʿ� �ִ����� �˾Ƴ��� �ȴ�.

	XMVECTOR planeEquation = XMLoadFloat4A(&plane.m_equation);
	XMVECTOR n = XMVectorSetW(planeEquation, 0.0f);
	
	XMVECTOR center = XMLoadFloat3A(&aabb.m_center);
	XMVECTOR extent = XMLoadFloat3A(&aabb.m_extent);

	float r = XMVectorGetX(XMVector3Dot(XMVectorAbs(n), extent));					// ���� �� ������ ���� ����
	float s = XMVectorGetX(XMVector3Dot(n, center)) + XMVectorGetW(planeEquation);	// center�� ������κ��� �������ִ� �Ÿ�
	
	return s + r < 0.0f;
}

// https://gist.github.com/cmf028/81e8d3907035640ee0e3fdd69ada543f ����
// Second Optimized implementation
// transform_aabb_optimized_abs_center_extents �Լ��� DirectXMath �������� ��ȯ
const Aabb XM_CALLCONV Math::TransformAabb(const Aabb& aabb, FXMMATRIX m)
{
	// Transform center
	XMVECTOR tCenter = XMVector3TransformCoord(XMLoadFloat3A(&aabb.m_center), m);

	// Transform extent
	XMMATRIX absMat(XMVectorAbs(m.r[0]), XMVectorAbs(m.r[1]), XMVectorAbs(m.r[2]), XMVectorZero());
	XMVECTOR tExtent = XMVector3TransformNormal(XMLoadFloat3A(&aabb.m_extent), absMat);

	return Aabb(tCenter, tExtent);
}
