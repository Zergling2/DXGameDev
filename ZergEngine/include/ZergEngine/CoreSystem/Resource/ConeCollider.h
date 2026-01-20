#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class ConeCollider : public ICollider
	{
	public:
		ConeCollider(float radius, float height);
		virtual ~ConeCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Cone; }
	};
}
