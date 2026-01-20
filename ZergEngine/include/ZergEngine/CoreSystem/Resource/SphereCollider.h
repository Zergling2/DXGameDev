#pragma once

#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class SphereCollider : public ICollider
	{
	public:
		SphereCollider(float radius);
		virtual ~SphereCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Sphere; }
	};
}
