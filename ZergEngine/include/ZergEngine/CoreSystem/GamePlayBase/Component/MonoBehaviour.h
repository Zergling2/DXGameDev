#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\DisplayMode.h>

// ComponentManagerMap 검색 시 필요
#define DECLARE_MONOBEHAVIOUR_TYPE \
public:\
	static constexpr ze::ComponentType TYPE = ze::ComponentType::MonoBehaviour;

namespace ze
{
	class MonoBehaviour : public IComponent
	{
		DECLARE_MONOBEHAVIOUR_TYPE
		friend class MonoBehaviourManager;
		friend class GameObject;
	public:
		static bool IsCreatable() { return true; }

		MonoBehaviour(GameObject& owner);
		virtual ~MonoBehaviour() = default;

		virtual ComponentType GetType() const override { return ComponentType::MonoBehaviour; }
		virtual void Awake();
		virtual void OnEnable();
		virtual void Start();
		virtual void FixedUpdate();
		virtual void Update();
		virtual void LateUpdate();
		virtual void OnDisable();
		virtual void OnDestroy();

		virtual void OnTriggerEnter() {}
		virtual void OnTriggerStay() {}
		virtual void OnTriggerExit() {}
		virtual void OnCollisionEnter() {}
		virtual void OnCollisionStay() {}
		virtual void OnCollisionExit() {}
	protected:
		// mode가 DisplayMode::BorderlessWindowed일 경우 width, height는 무시됩니다.
		// mode가 DisplayMode::Fullscreen일 경우 width, height는 각각 전체화면 해상도의 너비, 높이값으로 사용됩니다.
		// 이 함수는 스크립트 내에서만 호출되어야 하므로 MonoBehaviour 클래스의 멤버 함수로 옮겨졌습니다.
		bool SetResolution(uint32_t width, uint32_t height, DisplayMode mode);
	private:
		virtual IComponentManager* GetComponentManager() const override;

		virtual void OnDeploySysJob() override;
		virtual void OnEnableSysJob() override;
		virtual void OnDisableSysJob() override;
	private:
		uint32_t m_startingQueueIndex;
	};
}
