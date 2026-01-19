#pragma once

#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class SphereCollider : public ICollider
	{
	public:
		virtual ColliderType GetType() const override { return ColliderType::Sphere; }
	};
}
