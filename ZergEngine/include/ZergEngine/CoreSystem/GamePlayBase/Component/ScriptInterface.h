#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class IScript : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::SCRIPT;
		static inline bool IsCreatable() { return true; }

		IScript();
		virtual ~IScript() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::SCRIPT; }
		virtual void Awake() {}
		virtual void Start() {}
		virtual void FixedUpdate() {}
		virtual void Update() {}
		virtual void LateUpdate() {}
		virtual void OnEnable() {}
		virtual void OnDisable() {}
		virtual void OnDestroy() {}
	private:
		virtual IComponentManager* GetComponentManager() const override;
		virtual void SystemJobOnEnabled() override;
		virtual void SystemJobOnDisabled() override;
	};
}
