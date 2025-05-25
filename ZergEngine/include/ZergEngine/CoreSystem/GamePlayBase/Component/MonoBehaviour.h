#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

// ComponentManagerMap 검색 시 필요
#define DECLARE_MONOBEHAVIOUR_TYPE \
public:\
	static constexpr ze::COMPONENT_TYPE TYPE = ze::COMPONENT_TYPE::MONOBEHAVIOUR;

namespace ze
{
	class MonoBehaviour : public IComponent
	{
		friend class MonoBehaviourManagerImpl;
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		static bool IsCreatable() { return true; }

		MonoBehaviour();
		virtual ~MonoBehaviour() = default;

		virtual bool Enable() override;
		virtual bool Disable() override;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::MONOBEHAVIOUR; }
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
