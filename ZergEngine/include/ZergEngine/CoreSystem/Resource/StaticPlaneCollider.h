#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class StaticPlaneCollider : public ICollider
	{
	public:
		StaticPlaneCollider(const XMFLOAT3& planeNormal = XMFLOAT3(0.0f, 1.0f, 0.0f), float planeConstant = 0.0f);
		virtual ~StaticPlaneCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::StaticPlane; }
	};
}
