#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

class btRigidBody;

namespace ze
{
	class ICollider;

	class IRigidbody : public IComponent
	{
		friend class RigidbodyManager;
	public:
		static bool IsCreatable() { return true; }

		IRigidbody(GameObject& owner, std::shared_ptr<ICollider> collider);
		virtual ~IRigidbody() = default;

		float GetFriction() const;
		void SetFriction(float friction);

		float GetRollingFriction() const;
		void SetRollingFriction(float friction);

		float GetSpinningFriction() const;
		void SetSpinningFriction(float friction);

		std::shared_ptr<ICollider> GetCollider() const { return m_spCollider; }
	private:
		virtual IComponentManager* GetComponentManager() const override;

		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;
	protected:
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btRigidBody> m_upBtRigidBody;
	};
}
