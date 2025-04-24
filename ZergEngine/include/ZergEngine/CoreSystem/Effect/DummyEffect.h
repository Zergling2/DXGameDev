#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>

namespace ze
{
	// Apply() 함수 내 조건 분기 제거용 더미 이펙트 객체
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
