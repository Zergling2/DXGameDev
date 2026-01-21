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
		enum FreezeFlag : uint8_t
		{
			FF_None			= 0b00000000,
			FF_PositionX	= 0b00000001,
			FF_PositionY	= 0b00000010,
			FF_PositionZ	= 0b00000100,
			FF_RotationX	= 0b00001000,
			FF_RotationY	= 0b00010000,
			FF_RotationZ	= 0b00100000
		};
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

		void GetFreezePosition(bool& freezeX, bool& freezeY, bool& freezeZ);
		void SetFreezePosition(bool freezeX, bool freezeY, bool freezeZ);
		void GetFreezeRotation(bool& freezeX, bool& freezeY, bool& freezeZ);
		void SetFreezeRotation(bool freezeX, bool freezeY, bool freezeZ);
	private:
		virtual IComponentManager* GetComponentManager() const override;

		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;
	private:
		std::unique_ptr<MotionState> m_upMotionState;
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btRigidBody> m_upBtRigidBody;
		float m_mass;	// mass backup (kinematic <-> dynamic)
		bool m_useGravity;
		bool m_isKinematic;
		uint8_t m_freezeState;
	};
}
