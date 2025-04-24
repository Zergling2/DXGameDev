#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>

namespace ze
{
	// Apply() �Լ� �� ���� �б� ���ſ� ���� ����Ʈ ��ü
	class DummyEffect : public IEffect
	{
	public:
		DummyEffect() = default;
		virtual ~DummyEffect() = default;

		virtual void Init() override {}
		virtual void Release() override {}
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override { /* Do Nothing */ }
		virtual void KickedFromDeviceContext() noexcept override { /* Do Nothing */ }
	};
}
