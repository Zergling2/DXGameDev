#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

class btRigidBody;

namespace ze
{
	class MotionState;
	class ICollider;

	class Rigidbody : public IComponent
	{
		friend class RigidbodyManager;
	public:
		static constexpr ComponentType TYPE = ComponentType::Rigidbody;
		static bool IsCreatable() { return true; }

		Rigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT4& localRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		virtual ~Rigidbody() = default;

		virtual ComponentType GetType() const override { return ComponentType::Rigidbody; }

		float GetMass() const;
		void SetMass(float mass);

		float GetFriction() const;
		void SetFriction(float friction);

		float GetRollingFriction() const;
		void SetRollingFriction(float friction);

		float GetSpinningFriction() const;
		void SetSpinningFriction(float friction);

		XMFLOAT3 GetLinearVelocity() const;
		XMFLOAT3 GetAngularVelocity() const;

		bool IsUsingGravity() const { return m_useGravity; }
		void UseGravity(bool b);
		bool IsKinematic() const { return m_isKinematic; }
		void SetKinematic(bool b);

		void FreezePositionX(bool b);
		void FreezePositionY(bool b);
		void FreezePositionZ(bool b);
		void FreezeRotationX(bool b);
		void FreezeRotationY(bool b);
		void FreezeRotationZ(bool b);
		bool IsPositionXFreezed() const { return m_freezePositionX; }
		bool IsPositionYFreezed() const { return m_freezePositionY; }
		bool IsPositionZFreezed() const { return m_freezePositionZ; }
		bool IsRotationXFreezed() const { return m_freezeRotationX; }
		bool IsRotationYFreezed() const { return m_freezeRotationY; }
		bool IsRotationZFreezed() const { return m_freezeRotationZ; }
	private:
		virtual IComponentManager* GetComponentManager() const override;

		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;
	private:
		std::unique_ptr<MotionState> m_upMotionState;
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btRigidBody> m_upBtRigidBody;
		bool m_useGravity;
		bool m_isKinematic;
		bool m_freezePositionX;
		bool m_freezePositionY;
		bool m_freezePositionZ;
		bool m_freezeRotationX;
		bool m_freezeRotationY;
		bool m_freezeRotationZ;
		uint32_t m_kinematicIndex;	// Component Manager에서의 kinematic 배열 index
	};
}
