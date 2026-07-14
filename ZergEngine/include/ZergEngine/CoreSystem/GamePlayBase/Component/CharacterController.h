#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

class btPairCachingGhostObject;
class btKinematicCharacterController;

namespace ze
{
	class CapsuleCollider;

	class CharacterController : public IComponent
	{
		friend class CharacterControllerManager;
	public:
		static constexpr ComponentType TYPE = ComponentType::CharacterController;
		static bool IsCreatable() { return true; }

		CharacterController(GameObject& owner, std::shared_ptr<CapsuleCollider> collider, const XMFLOAT3& localPos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT4& localRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		virtual ~CharacterController() = default;

		virtual ComponentType GetType() const override { return ComponentType::CharacterController; }

		/**
		* @brief CharacterController 컴포넌트를 소유한 GameObject의 경우, 위치를 순간적으로 이동시키기 위해 GameObject의 m_transform 값을 직접 수정하지 마세요.
		* 대신 이 함수를 이용해 객체의 위치를 워프시킬 수 있습니다.
		* 
		* @param pos CharacterController를 소유한 GameObject에 새로 지정할 월드 위치.
		*/
		void SetWorldPosition(const XMFLOAT3& pos);

		void SetWalkDirection(const XMFLOAT3& dir);

		/**
		* @brief 등반 가능한 최대 경사면 각도를 설정합니다.
		* @param angle 등반 가능한 최대 경사면 각도.
		*/
		void SetMaxSlopeRadian(float angle);

		/**
		* @brief 등반 가능한 최대 경사면 각도를 설정합니다.
		* @param angle 등반 가능한 최대 경사면 각도.
		*/
		void SetMaxSlopeDegree(float angle);

		float GetMaxSlopeRadian() const { return m_maxSlope; }
		float GetMaxSlopeDegree() const;

		/**
		* @brief 점프 속력을 설정합니다.
		* @param speed 새로운 점프 속력.
		*/
		void SetJumpSpeed(float speed);

		float GetJumpSpeed() const { return m_jumpSpeed; }

		/**
		* @brief 낙하 종단 속력을 설정합니다.
		* @param speed 새로운 낙하 종단 속력.
		*/
		void SetFallSpeed(float speed);
		
		float GetFallSpeed() const { return m_fallSpeed; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;

		void AddToPhysicsWorld();
		void RemoveFromPhysicsWorld();
	private:
		bool m_inPhysicsWorld;
		float m_stepHeight;
		float m_jumpSpeed;
		float m_fallSpeed;
		float m_maxSlope;	// radian
		std::unique_ptr<btPairCachingGhostObject> m_upGhostObject;
		std::shared_ptr<CapsuleCollider> m_spCollider;
		std::unique_ptr<btKinematicCharacterController> m_upController;
	};
}
