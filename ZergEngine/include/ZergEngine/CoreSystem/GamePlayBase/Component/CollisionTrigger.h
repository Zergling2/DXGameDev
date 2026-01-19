#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

class btGhostObject;

namespace ze
{
	class ICollider;

	class CollisionTrigger : public IComponent
	{
		friend class CollisionTriggerManager;
	public:
		static constexpr ComponentType TYPE = ComponentType::CollisionTrigger;
		static bool IsCreatable() { return true; }

		CollisionTrigger(GameObject& owner, std::shared_ptr<ICollider> collider);
		virtual ~CollisionTrigger() = default;

		virtual ComponentType GetType() const override { return ComponentType::CollisionTrigger; }
	private:
		virtual IComponentManager* GetComponentManager() const override;

		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;
	private:
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btGhostObject> m_upBtGhostObject;
	};
}
