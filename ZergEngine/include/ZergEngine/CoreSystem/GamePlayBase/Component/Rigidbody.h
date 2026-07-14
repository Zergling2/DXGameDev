#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

class btRigidBody;

namespace ze
{
	class MotionState;
	class ICollider;

	enum class RigidbodyType
	{
		Dynamic,
		Kinematic,
		Static
	};

	class Rigidbody : public IComponent
	{
		friend class RigidbodyManager;
		friend class Physics;
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

		/**
		* @brief MonoBehaviour 스크립트에서 충돌 콜백을 수신할지 여부를 결정합니다.
		*/
		void ListenCollisionEvent();
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

		float GetMass() const;
		float GetInvMass() const;
		void SetMass(float mass);

		XMFLOAT3 GetLinearVelocity() const;
		void SetLinearVelocity(const XMFLOAT3& vel);
		XMFLOAT3 GetAngularVelocity() const;
		void SetAngularVelocity(const XMFLOAT3& vel);

		void ApplyForce(const XMFLOAT3& force, const XMFLOAT3& relPos);
		void ApplyCentralForce(const XMFLOAT3& force);
		void ApplyImpulse(const XMFLOAT3& impulse, const XMFLOAT3& relPos);
		void ApplyCentralImpulse(const XMFLOAT3& impulse);

		bool IsUsingGravity() const { return m_useGravity; }
		void UseGravity(bool b);
		RigidbodyType GetBodyType() const { return m_bodyType; }
		void SetBodyType(RigidbodyType bodyType);

		void GetFreezePosition(bool& freezeX, bool& freezeY, bool& freezeZ);
		void SetFreezePosition(bool freezeX, bool freezeY, bool freezeZ);
		void GetFreezeRotation(bool& freezeX, bool& freezeY, bool& freezeZ);
		void SetFreezeRotation(bool freezeX, bool freezeY, bool freezeZ);

		const XMFLOAT3& GetColliderLocalPos() const;
		const XMFLOAT4& GetColliderLocalRot() const;
	private:
		virtual IComponentManager* GetComponentManager() const override;
		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;

		void AddToPhysicsWorld();
		void RemoveFromPhysicsWorld();
	private:
		bool m_inPhysicsWorld;
		bool m_listenCollisionEvent;
		bool m_isTrigger;
		bool m_useGravity;
		RigidbodyType m_bodyType;
		std::shared_ptr<ICollider> m_spCollider;
		std::unique_ptr<btRigidBody> m_upBtRigidBody;
		std::unique_ptr<MotionState> m_upMotionState;
		float m_mass;	// mass backup (Dynamic <-> Kinematic/Static 전환시 보존)
		uint8_t m_freezeState;
	};
}
