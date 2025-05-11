#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SunScript : public ze::MonoBehaviour
	{
	public:
		SunScript() = default;
		virtual ~SunScript() = default;

		virtual void Awake() override { printf("SunScript::Awake()\n"); }
		virtual void OnEnable() override { printf("SunScript::OnEnable()\n"); }
		virtual void Start() override { printf("SunScript::Start()\n"); }
		virtual void FixedUpdate() override;
	};
}
