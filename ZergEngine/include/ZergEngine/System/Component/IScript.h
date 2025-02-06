#pragma once

#include <ZergEngine\System\Component\IComponent.h>

namespace zergengine
{
	class IScript : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::SCRIPT;
		static inline bool IsCreatable() { return true; }
		virtual ~IScript();
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
		virtual void SystemJobOnEnabled() override;
		virtual void SystemJobOnDisabled() override;
	};
}
