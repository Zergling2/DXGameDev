#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

// ComponentManagerMap 검색 시 필요
#define DECLARE_MONOBEHAVIOUR_TYPE \
public:\
	static constexpr ze::ComponentType TYPE = ze::ComponentType::MONOBEHAVIOUR;

namespace ze
{
	class MonoBehaviour : public IComponent
	{
		DECLARE_MONOBEHAVIOUR_TYPE
		friend class MonoBehaviourManager;
	public:
		static bool IsCreatable() { return true; }

		MonoBehaviour();
		virtual ~MonoBehaviour() = default;

		virtual void Enable() override;
		virtual void Disable() override;

		virtual ComponentType GetType() const override { return ComponentType::MONOBEHAVIOUR; }
		virtual void Awake();
		virtual void OnEnable();
		virtual void Start();
		virtual void FixedUpdate();
		virtual void Update();
		virtual void LateUpdate();
		virtual void OnDisable();
		virtual void OnDestroy();
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		uint32_t m_startingQueueIndex;
	};
}
