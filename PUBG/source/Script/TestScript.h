#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScript : public zergengine::IScript
	{
	public:
		TestScript()	// ��ũ��Ʈ �ý��� �ϼ��Ǹ� OnStart ��� �ʱ�ȭ
		{
		}
		virtual void FixedUpdate() override;
	};
}
