#include <ZergEngine\CoreSystem\BulletDXMath.h>

using namespace ze;

XMFLOAT3 BtToDX::ConvertVector(const btVector3& v)
{
	return XMFLOAT3(v.x(), v.y(), -v.z());
}

XMFLOAT4 BtToDX::ConvertQuaternion(const btQuaternion& q)
{
	return XMFLOAT4(q.x(), q.y(), -q.z(), -q.w());
}

btVector3 DXToBt::ConvertVector(const XMFLOAT3& v)
{
	return btVector3(v.x, v.y, -v.z);
}

btQuaternion DXToBt::ConvertQuaternion(const XMFLOAT4& q)
{
	return btQuaternion(q.x, q.y, -q.z, -q.w);
}
