#include <ZergEngine\CoreSystem\BulletDXMath.h>

using namespace ze;

XMFLOAT4X4 BtToDX::ConvertMatrix(const btTransform& t)
{
	const btMatrix3x3& basis = t.getBasis();
	const btVector3& origin = t.getOrigin();

	// M = diag(1, 1, -1) (Z축에 대해 반전 행렬)

	// 회전은 M * R * M

	DirectX::XMFLOAT4X4 m(
		basis[0][0], basis[0][1], -basis[0][2], 0.0f,
		basis[1][0], basis[1][1], -basis[1][2], 0.0f,
		-basis[2][0], -basis[2][1], basis[2][2], 0.0f,
		origin.x(), origin.y(), -origin.z(), 1.0f
	);

	return m;
}

btTransform DXToBt::ConvertMatrix(const XMFLOAT4X4& m)
{
	btTransform t;

	// M = diag(1, 1, -1) (Z축에 대해 반전 행렬)

	// 회전은 M * R * M
	btMatrix3x3 basis(
		m._11, m._12, -m._13,
		m._21, m._22, -m._23,
		-m._31, -m._32, m._33
	);
	btVector3 origin(m._41, m._42, -m._43);	// 벡터는 M * v

	t.setBasis(basis);
	t.setOrigin(origin);

	return t;
}
