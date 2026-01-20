#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class CapsuleCollider : public ICollider
	{
	public:
		CapsuleCollider(float radius, float height);
		virtual ~CapsuleCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Capsule; }
	};
}
