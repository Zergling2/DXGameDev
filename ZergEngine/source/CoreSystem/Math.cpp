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
	// Ray 표현식
	// O + tD (O = 시작점, D = 방향벡터, t = [0, INF))

	// ex)
	// AABB의 min과 교차하는지 검사
	// -> O + tD = AABB.min 
	// x, y, z 각 성분별로 방정식을 세우면
	// O.x + tD.x = AABB.min.x	=> t에 대해서 정리하면 t = (AABB.min.x - O.x) / D.x
	// 나머지 y, z도 동일
	// t = (AABB.min.y - O.y) / D.y
	// t = (AABB.min.z - O.z) / D.z

	// AABB의 max와 교차 계산도 동일 (AABB.min만 AABB.max로 교체하면 방정식 완성)

	XMVECTOR origin = XMLoadFloat3A(&ray.m_origin);
	XMVECTOR dir = XMLoadFloat3A(&ray.m_direction);
	XMVECTOR extent = XMLoadFloat3A(&aabb.m_extent);
	XMVECTOR aabbMin = XMVectorSubtract(XMLoadFloat3A(&aabb.m_center), extent);
	XMVECTOR aabbMax = XMVectorAdd(XMLoadFloat3A(&aabb.m_center), extent);

	// Ray의 방향벡터 역수 (Direction으로 나누기 대신 곱셈으로 대체 가능)
	XMVECTOR invDir = XMVectorReciprocal(dir);

	// SIMD 벡터화 연산으로 한번에 x, y, z 성분에 대한 방정식 모두 계산
	// t = (AABB.min - O) / D
	// t = (AABB.max - O) / D
	XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(aabbMin, origin), invDir);
	XMVECTOR t2 = XMVectorMultiply(XMVectorSubtract(aabbMax, origin), invDir);

	// t1, t2 각각에서 t값이 작은 것이 AABB 진입점, 큰 것이 AABB 나가는 점 (각 성분별로 진입점과 나가는 점이 t1과 t2에 섞여있을 수 있음)
	XMVECTOR tMinVec = XMVectorMin(t1, t2);
	XMVECTOR tMaxVec = XMVectorMax(t1, t2);

	const float tMinX = XMVectorGetX(tMinVec);
	const float tMinY = XMVectorGetY(tMinVec);
	const float tMinZ = XMVectorGetZ(tMinVec);

	const float tMaxX = XMVectorGetX(tMaxVec);
	const float tMaxY = XMVectorGetY(tMaxVec);
	const float tMaxZ = XMVectorGetZ(tMaxVec);

	// tMinVec 중에서 가장 큰 값, tMaxVec 중에서 가장 작은 값 사이가 공통 구간(박스 내에 Ray가 지나가는 구간)이다.
	float tMin = std::max(std::max(tMinX, tMinY), tMinZ);
	float tMax = std::min(std::min(tMaxX, tMaxY), tMaxZ);

	if (tMax < 0.0f) // AABB가 ray 뒤쪽
		return false;

	if (tMin > tMax) // 교차 구간 없음
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
	// 원리
	// 평면의 법선 방향으로 가장 멀리있는 AABB의 정점을 알아내야 한다.
	// 하지만 굳이 8개 정점을 검색할 필요는 없다.
	// 우선 평면의 법선 방향으로 가장 멀리 있는 AABB 정점의 투영 길이는 알아낼 수 있다.
	// 이건 평면 법선의 절댓값과 AABB의 extent를 내적하면 된다. (둘 다 positive 성분만 있기 때문에 최장 투영 길이를 구할 수 있음)
	// 그리고 그 정점의 대각선으로 마주보는 정점이 평면의 반대쪽에 있는지를 알아내면 된다.

	XMVECTOR planeEquation = XMLoadFloat4A(&plane.m_equation);
	XMVECTOR n = XMVectorSetW(planeEquation, 0.0f);
	
	XMVECTOR center = XMLoadFloat3A(&aabb.m_center);
	XMVECTOR extent = XMLoadFloat3A(&aabb.m_extent);

	float r = XMVectorGetX(XMVector3Dot(XMVectorAbs(n), extent));					// 가장 먼 정점의 투영 길이
	float s = XMVectorGetX(XMVector3Dot(n, center)) + XMVectorGetW(planeEquation);	// center가 평면으로부터 떨어져있는 거리
	
	return s + r < 0.0f;
}

// https://gist.github.com/cmf028/81e8d3907035640ee0e3fdd69ada543f 참고
// Second Optimized implementation
// transform_aabb_optimized_abs_center_extents 함수를 DirectXMath 버전으로 변환
const Aabb XM_CALLCONV Math::TransformAabb(const Aabb& aabb, FXMMATRIX m)
{
	// Transform center
	XMVECTOR tCenter = XMVector3TransformCoord(XMLoadFloat3A(&aabb.m_center), m);

	// Transform extent
	XMMATRIX absMat(XMVectorAbs(m.r[0]), XMVectorAbs(m.r[1]), XMVectorAbs(m.r[2]), XMVectorZero());
	XMVECTOR tExtent = XMVector3TransformNormal(XMLoadFloat3A(&aabb.m_extent), absMat);

	return Aabb(tCenter, tExtent);
}
