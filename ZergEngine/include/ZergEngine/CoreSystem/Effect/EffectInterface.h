#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

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
		// Dirty Flag Ȯ���ؼ� ����� ���׵鸸 ����
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept = 0;

		// ��� Dirty Flag �缳�� �� ��Ÿ �ʱ�ȭ
		virtual void KickedFromDeviceContext() noexcept = 0;
	};
}
