#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class IEffect abstract
	{
		friend class EffectDeviceContext;
	public:
		IEffect() = default;
		virtual ~IEffect() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;
	private:
		// DirtyFlag 기반 변경된 사항들만 업데이트 및 바인딩.
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept = 0;

		// 파이프라인이 다른 Effect 클래스에 의해 변경된 경우 호출되는 함수.
		// 대표적으로 DirtyFlag 재설정 작업을 수행해야 함.
		virtual void OnUnbindFromDeviceContext() noexcept = 0;
	};
}
