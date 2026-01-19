#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

class btCompoundShape;

namespace ze
{
	class BoxCollider : public ICollider
	{
	public:
		BoxCollider(const XMFLOAT3& halfExtents, const XMFLOAT3& localPos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT4& localRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		virtual ~BoxCollider();

		virtual ColliderType GetType() const override { return ColliderType::Box; }
		virtual btCollisionShape* GetCollisionShape() const override;
	private:
		std::unique_ptr<btCompoundShape> m_upBtCollisionShape;
	};
}
