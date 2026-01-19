#pragma once

#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class CapsuleCollider : public ICollider
	{
	public:
		virtual ColliderType GetType() const override { return ColliderType::Capsule; }
	};
}
