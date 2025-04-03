#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class MeshRenderer;

	class ITechnique abstract
	{
	public:
		ITechnique()
			: m_pDeviceContext(nullptr)
		{
		}
		virtual ~ITechnique() = default;

		void SetDeviceContext(ID3D11DeviceContext* pDeviceContext) { m_pDeviceContext = pDeviceContext; }
		inline ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; }
	private:
		ID3D11DeviceContext* m_pDeviceContext;
	};
}
