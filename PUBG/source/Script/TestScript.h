#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScript : public zergengine::IScript
	{
	public:
		TestScript()	// 스크립트 시스템 완성되면 OnStart 등에서 초기화
		{
		}
		virtual void FixedUpdate() override;
	};
}
