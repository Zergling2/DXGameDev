#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\DataStructure\Ray.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>
#include <ZergEngine\CoreSystem\DataStructure\Frustum.h>
#include <ZergEngine\CoreSystem\DataStructure\Triangle.h>
#include <cmath>

using namespace ze;

int Math::WrapInt(int val, int min, int max)
{
	const int range = max - min;
	if (range == 0)
		return val;

	int newVal = (val - min) % range;
	if (newVal < 0)
		newVal += range;

	return min + newVal;
}

float Math::WrapFloat(float val, float max)
{
	if (max <= 0.0f)
		return val;

	float newVal = std::fmodf(val, max);
	if (newVal < 0.0f)
		newVal += max;

	return newVal;
}

float Math::WrapFloat(float val, float min, float max)
{
	const float range = max - min;
	if (range <= 0.0f)
		return val;

	float newVal = std::fmodf(val - min, range);
	if (newVal < 0.0f)
		newVal += range;

	return min + newVal;
}

double Math::WrapDouble(double val, double max)
{
	if (max <= 0.0)
		return val;

	double newVal = std::fmod(val, max);
	if (newVal < 0.0)
		newVal += max;

	return newVal;
}

double Math::WrapDouble(double val, double min, double max)
{
	const double range = max - min;
	if (range <= 0.0)
		return val;

	double newVal = std::fmod(val - min, range);
	if (newVal < 0.0)
		newVal += range;

	return min + newVal;
}

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

bool Math::TestFrustumObbCollision(const Frustum& frustum, const Obb& obb)
{
	return frustum.Intersects(obb);
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

void XM_CALLCONV Math::ComputeFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4* pPlanes)
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

void XM_CALLCONV Math::ComputeFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4A* pPlanes)
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

XMMATRIX XM_CALLCONV Math::ComputeBillboardSphericalMatrix(FXMVECTOR billboardPosW, FXMVECTOR billboardScaleW, FXMVECTOR cameraPosW, GXMVECTOR cameraUpW)
{
	XMVECTOR dir = XMVector3Normalize(cameraPosW - billboardPosW);
	SphericalCoord sc = Math::ToSphericalCoord(dir);
	float pitch = -sc.theta;
	// float yaw = -(sc.phi - XM_PIDIV2);	// 극좌표계 기준으로 빌보드가 이미 90도 회전한 상태이기 때문에 + 왼손좌표계 회전으로 변환 (negate)
	float yaw = XM_PIDIV2 - sc.phi;	// 연산 최적화 (Subtract x1 + Multiply x1) -> Subtract x1
	constexpr float roll = 0.0f;

	XMMATRIX m = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);	// 회전 순서: Roll(z) -> Pitch(x) -> Yaw(y)
	
	m = XMMatrixScalingFromVector(billboardScaleW) * m;	// S x R
	m.r[3] = XMVectorSetW(billboardPosW, 1.0f);		// T 결합
	
	return m;
}

XMMATRIX XM_CALLCONV Math::ComputeBillboardCylindricalYMatrix(FXMVECTOR billboardPosW, FXMVECTOR billboardScaleW, FXMVECTOR cameraPosW)
{
	// 빌보드의 기저 벡터 계산
	XMVECTOR up = Vector3::Up();	// up 벡터는 월드 up 벡터
	XMVECTOR forward = XMVector3Normalize(XMVectorAndInt(cameraPosW - billboardPosW, g_XMSelect1011));	// XMVectorAndInt -> XMVectorSetY Zero
	if (XMVector3Equal(forward, XMVectorZero()))
		forward = Vector3::Forward();
	XMVECTOR right = XMVector3Cross(up, forward);	// right 벡터는 up, forward로 도출

	XMMATRIX m;
	m.r[0] = right;
	m.r[1] = up;
	m.r[2] = forward;
	m.r[3] = Math::IdentityR3();

	m = XMMatrixScalingFromVector(billboardScaleW) * m;	// S x R
	m.r[3] = XMVectorSetW(billboardPosW, 1.0f);		// T 결합

	return m;
}

XMMATRIX XM_CALLCONV Math::ComputeBillboardScreenAlignedMatrix(FXMMATRIX billboardRotationW, HXMVECTOR billboardPosW, HXMVECTOR billboardScaleW)
{
	// Screen Aligned Billboard는 모든 빌보드가 단일 카메라에 대해 동일한 회전 행렬을 가진다.
	// 따라서 외부에서 한번 계산한 값(billboardRotationW 매개변수)을 재활용해 계산한다.

	XMMATRIX m = XMMatrixScalingFromVector(billboardScaleW) * billboardRotationW;	// S x R
	m.r[3] = XMVectorSetW(billboardPosW, 1.0f);		// T 결합

	return m;
}

/*
XMMATRIX XM_CALLCONV Math::ComputeBillboardScreenAlignedMatrix(FXMVECTOR billboardPosW, FXMVECTOR billboardScaleW, CXMMATRIX viewMatrix)
{
	// 뷰 행렬 = 카메라의 월드 배치 행렬의 역행렬
	// S * R * T ---Inverse---> T(Inv) * R(Inv) * S(Inv)
	// 따라서 뷰 행렬의 역행렬은 카메라의 월드 배치 행렬이다.
	// 그런데 필요한 것은 카메라 월드 배치 행렬의 '회전' 성분만이므로 3x3 부분의 역행렬만 구하면 된다.
	// 그런데 3x3 부분은 직교 행렬이므로 이것만 추출해서 전치시키면 저비용으로 회전 성분들을 구할 수 있다.

	// XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMMATRIX invViewMatrix;
	// 3x3 회전 성분만을 담은 뒤 전치시킨다.
	invViewMatrix.r[0] = viewMatrix.r[0];
	invViewMatrix.r[1] = viewMatrix.r[1];
	invViewMatrix.r[2] = viewMatrix.r[2];
	invViewMatrix.r[3] = Math::IdentityR3();	// 필요없는 부분
	invViewMatrix = XMMatrixTranspose(invViewMatrix);

	// invViewMatrix의 3x3 행렬은 카메라의 기저 벡터.


	// 빌보드의 기저 벡터 계산
	XMVECTOR up = invViewMatrix.r[1];	// up 벡터는 카메라의 up 벡터와 동일
	XMVECTOR forward = XMVectorNegate(invViewMatrix.r[2]);	// 빌보드의 forward는 camera의 forward와 반대방향
	XMVECTOR right = XMVector3Cross(up, forward);	// right 벡터는 up, forward로 도출
	
	assert(Math::IsVector3LengthEstNear(up, 1.0f) == true);
	assert(Math::IsVector3LengthEstNear(forward, 1.0f) == true);
	assert(Math::IsVector3LengthEstNear(right, 1.0f) == true);

	XMMATRIX m;
	m.r[0] = right;
	m.r[1] = up;
	m.r[2] = forward;
	m.r[3] = Math::IdentityR3();

	m = XMMatrixScalingFromVector(billboardScaleW) * m;	// S x R
	m.r[3] = XMVectorSetW(billboardPosW, 1.0f);		// T 결합

	return m;
}
*/

XMVECTOR XM_CALLCONV Quaternion::FromToRotation(FXMVECTOR from, FXMVECTOR to)
{
	return Quaternion::FromToRotationNorm(XMVector3Normalize(from), XMVector3Normalize(to));
}

XMVECTOR XM_CALLCONV Quaternion::FromToRotationNorm(FXMVECTOR from, FXMVECTOR to)
{
	assert(Math::IsVector3LengthNear(from, 1.0f) == true);
	assert(Math::IsVector3LengthNear(to, 1.0f) == true);

	// 내적 결과 (코사인 각)[0.0, 0.05], [179.95, 180.0] 구간의 경우 예외처리
	constexpr float EPSILON_COS_ANGLE = 0.99999964f;	// cos(0.05도)

	// 내적 계산
	const float cosRotAngle = XMVectorGetX(XMVector3Dot(from, to));

	// 회전축 정의하기 힘든 경우 예외처리
	// 1. 거의 같은 방향
	if (cosRotAngle > EPSILON_COS_ANGLE)
		return XMQuaternionIdentity();

	// 2. 거의 정반대 방향
	if (cosRotAngle < -EPSILON_COS_ANGLE)
	{
		// 회전축 생성 (from벡터를 가지고 from과 수직인 벡터를 생성)
		// (x, y, z) -> (z, x, y) 오른쪽으로 한바퀴 로테이션 시키면 항상 수직인 벡터 획득
		// 외적하면
		// (x, y, z)
		// (z, x, y)
		// ---------
		// (y^2 - xz, z^2 - xy, x^2 - yz) 이다.
		
		// 원본 벡터 (x, y, z)와 (y^2 - xz, z^2 - xy, x^2 - yz)를 내적해보면 xy^2 - x^2z + yz^2 - xy^2 + x^2z - yz^2 = 0이다.
		// 내적 결과가 0이므로 두 벡터는 수직이다.
		XMVECTOR rotationAxis = XMVectorSwizzle(from, 2, 0, 1, 3);	// from의 z, x, y, w 값을 선택
		return XMQuaternionRotationNormal(rotationAxis, XM_PI);
	}


	// 일반적인 경우
	// XMQuaternionRotationNormal 호출하려면 회전각도를 계산해야 하는데 arccos 함수 호출이 필요.
	// 회전각을 a라고 할때,
	// 쿼터니언은 (sin(a/2) * RotationAxis, cos(a/2))이다.
	// 삼각함수 반각 공식을 활용해 cos(a)와 제곱근 연산만으로 cos(a/2)를 구할 수 있다.
	// 
	// 반각 공식 1
	// sin(a/2)^2 = (1 - cos(a)) / 2
	// 반각 공식 2
	// cos(a/2)^2 = (1 + cos(a)) / 2
	
	XMVECTOR rotationAxis = XMVector3Normalize(XMVector3Cross(from, to));
	XMVECTOR sincosHalfRotAngleSquared = XMVectorSet((1.0f - cosRotAngle) * 0.5f, (1.0f + cosRotAngle) * 0.5f, 0.0f, 0.0f);
	XMVECTOR sincosHalfRotAngle = XMVectorSqrt(sincosHalfRotAngleSquared);	// sqrt 연산 한번에
	
	const float sinHalfRotAngle = XMVectorGetX(sincosHalfRotAngle);
	const float cosHalfRotAngle = XMVectorGetY(sincosHalfRotAngle);
	
	XMVECTOR q = XMVectorScale(rotationAxis, sinHalfRotAngle);	// 쿼터니언 x, y, z 성분 먼저 계산
	q = XMVectorSetW(q, cosHalfRotAngle);	// w성분 설정
	
	return XMQuaternionNormalize(q);
}

SphericalCoord XM_CALLCONV Math::ToSphericalCoord(FXMVECTOR v)
{
	SphericalCoord result;

	result.r = XMVectorGetX(XMVector3Length(v));

	// 길이가 0인 경우 예외 처리
	if (result.r == 0.0f)
	{
		result.theta = 0.0f;
		result.phi = 0.0f;
		return result;
	}

	XMFLOAT3A vec;
	XMStoreFloat3A(&vec, v);

	result.theta = std::asin(vec.y / result.r);
	result.phi = std::atan2(vec.z, vec.x);

	if (result.phi < 0.0f)
		result.phi += XM_2PI;

	return result;
}
