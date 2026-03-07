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
		friend class Physics;
	public:
		static bool IsCreatable() { return true; }

		IRigidbody(GameObject& owner);
		virtual ~IRigidbody() = default;

		void ListenCollisionEvent();

		/**
		* @brief 스크립트에서 충돌 이벤트 함수를 받을지 결정합니다.
		*
		* Kinematic Rigidbody는 충돌 이벤트 함수에 관여되지 않습니다.
		*/
		bool IsListeningCollisionEvent() const { return m_listenCollisionEvent; }

		void SetTrigger(bool b);
		bool IsTrigger() const { return m_isTrigger; }

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
		bool m_listenCollisionEvent;
		bool m_isTrigger;
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btRigidBody> m_upBtRigidBody;
	};
}
