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
		Terrain
	};

	class ICollider
	{
	public:
		ICollider() = default;
		virtual ~ICollider() = default;

		virtual ColliderType GetType() const = 0;
		virtual btCollisionShape* GetCollisionShape() const = 0;
	};
}
