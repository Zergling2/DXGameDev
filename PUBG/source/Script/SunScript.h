#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SunScript : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		SunScript() = default;
		virtual ~SunScript() = default;

		virtual void Awake() override { printf("SunScript::Awake()\n"); }
		virtual void OnEnable() override { printf("SunScript::OnEnable()\n"); }
		virtual void Start() override { printf("SunScript::Start()\n"); }
		virtual void FixedUpdate() override;
		virtual void OnDisable() override { printf("SunScript::OnDisable()\n"); }
		virtual void OnDestroy() override { printf("SunScript::OnDestroy()\n"); }
	};
}
