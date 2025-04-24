#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

XMVECTOR Math::QuaternionToEuler(XMVECTOR quaternion) noexcept
{
	quaternion = XMQuaternionNormalize(quaternion);

	float qx = XMVectorGetX(quaternion);
	float qy = XMVectorGetY(quaternion);
	float qz = XMVectorGetZ(quaternion);
	float qw = XMVectorGetW(quaternion);

	XMFLOAT3A euler;

	// X (Pitch)
	float sinp = 2.0f * (qw * qx - qy * qz);
	if (std::fabs(sinp) >= 1.0f)
		euler.x = XMConvertToDegrees(copysign(XM_PIDIV2, sinp)); // 90도 or -90도
	else
		euler.x = XMConvertToDegrees(asinf(sinp));

	// Z (Roll)
	float sinr_cosp = 2.0f * (qw * qz + qx * qy);
	float cosr_cosp = 1.0f - 2.0f * (qx * qx + qz * qz);
	euler.z = XMConvertToDegrees(std::atan2f(sinr_cosp, cosr_cosp));

	// Y (Yaw)
	float siny_cosp = 2.0f * (qw * qy + qx * qz);
	float cosy_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
	euler.y = XMConvertToDegrees(std::atan2f(siny_cosp, cosy_cosp));

	// X(Pitch), Y(Yaw), Z(Roll) 순서로 XMVECTOR에 패킹
	return XMLoadFloat3A(&euler);
}

void Math::ExtractFrustumPlanesInWorldSpace(const XMFLOAT4X4A* pViewProjMatrix, XMFLOAT4A planes[6]) noexcept
{
	// Left
	planes[0].x = pViewProjMatrix->_11 + pViewProjMatrix->_14;
	planes[0].y = pViewProjMatrix->_21 + pViewProjMatrix->_24;
	planes[0].z = pViewProjMatrix->_31 + pViewProjMatrix->_34;
	planes[0].w = pViewProjMatrix->_41 + pViewProjMatrix->_44;

	// Right
	planes[1].x = pViewProjMatrix->_14 - pViewProjMatrix->_11;
	planes[1].y = pViewProjMatrix->_24 - pViewProjMatrix->_21;
	planes[1].z = pViewProjMatrix->_34 - pViewProjMatrix->_31;
	planes[1].w = pViewProjMatrix->_44 - pViewProjMatrix->_41;

	// Bottom
	planes[2].x = pViewProjMatrix->_12 + pViewProjMatrix->_14;
	planes[2].y = pViewProjMatrix->_22 + pViewProjMatrix->_24;
	planes[2].z = pViewProjMatrix->_32 + pViewProjMatrix->_34;
	planes[2].w = pViewProjMatrix->_42 + pViewProjMatrix->_44;
	
	// Top
	planes[3].x = pViewProjMatrix->_14 - pViewProjMatrix->_12;
	planes[3].y = pViewProjMatrix->_24 - pViewProjMatrix->_22;
	planes[3].z = pViewProjMatrix->_34 - pViewProjMatrix->_32;
	planes[3].w = pViewProjMatrix->_44 - pViewProjMatrix->_42;
	
	// Near
	planes[4].x = pViewProjMatrix->_13;
	planes[4].y = pViewProjMatrix->_23;
	planes[4].z = pViewProjMatrix->_33;
	planes[4].w = pViewProjMatrix->_42;

	// Far
	planes[5].x = pViewProjMatrix->_14 - pViewProjMatrix->_13;
	planes[5].y = pViewProjMatrix->_24 - pViewProjMatrix->_23;
	planes[5].z = pViewProjMatrix->_34 - pViewProjMatrix->_33;
	planes[5].w = pViewProjMatrix->_44 - pViewProjMatrix->_43;

	for (size_t i = 0; i < 6; ++i)
	{
		const XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&planes[i]));	// 정규화해서 전달 (GPU에서 정규화 생략)
		XMStoreFloat4A(&planes[i], v);
	}
}
