#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <bullet3\btBulletDynamicsCommon.h>

namespace ze
{
	class BtToDX
	{
	public:
		static XMFLOAT3 ConvertVector(const btVector3& v);
		static XMFLOAT4 ConvertQuaternion(const btQuaternion& q);
		static XMFLOAT4X4 ConvertMatrix(const btTransform& t);
		static XMFLOAT3 ForwardVector(const btVector3& v) { return XMFLOAT3(v.x(), v.y(), v.z()); }
		static XMFLOAT4 ForwardQuaternion(const btQuaternion& q) { return XMFLOAT4(q.x(), q.y(), q.z(), q.w()); }
	};

	class DXToBt
	{
	public:
		static btVector3 ConvertVector(const XMFLOAT3& v);
		static btQuaternion ConvertQuaternion(const XMFLOAT4& q);
		static btTransform ConvertMatrix(const XMFLOAT4X4& m);
		static btVector3 ForwardVector(const XMFLOAT3& v) { return btVector3(v.x, v.y, v.z); }
		static btQuaternion ForwardQuaternion(const XMFLOAT4& q) { return btQuaternion(q.x, q.y, q.z, q.w); }
	};
}
