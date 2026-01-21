#pragma once

#include <memory>

class btCollisionShape;

namespace ze
{
	enum class ColliderType
	{
		Box,
		Sphere,
		Capsule,
		Cone,
		Cylinder,
		StaticPlane,
		Terrain
	};

	class ICollider
	{
	public:
		ICollider() = default;
		virtual ~ICollider() = default;

		virtual ColliderType GetType() const = 0;
		btCollisionShape* GetCollisionShape() const { return m_upBtCollisionShape.get(); }
	protected:
		std::unique_ptr<btCollisionShape> m_upBtCollisionShape;
	};
}
