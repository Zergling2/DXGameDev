#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class BoxCollider : public ICollider
	{
	public:
		BoxCollider(const XMFLOAT3& halfExtents);
		virtual ~BoxCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Box; }
	};
}
