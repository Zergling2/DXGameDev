#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class CylinderCollider : public ICollider
	{
	public:
		CylinderCollider(float radius, float height);
		virtual ~CylinderCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Cylinder; }
	};
}
