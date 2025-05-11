#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class MonoBehaviour : public IComponent
	{
		friend class ScriptManagerImpl;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::SCRIPT;
		static bool IsCreatable() { return true; }

		MonoBehaviour();
		virtual ~MonoBehaviour() = default;

		virtual bool Enable() override;
		virtual bool Disable() override;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::SCRIPT; }
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
