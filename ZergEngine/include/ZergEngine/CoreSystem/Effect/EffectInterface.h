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
		// Dirty Flag 확인해서 변경된 사항들만 세팅
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept = 0;

		// 모든 Dirty Flag 재설정 및 기타 초기화
		virtual void KickedOutOfDeviceContext() noexcept = 0;
	};
}
