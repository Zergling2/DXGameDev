#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Skybox
	{
		friend class ResourceManager;
	public:
		Skybox() = default;
		~Skybox() = default;

		inline ID3D11Texture2D* GetTex2DComInterface() const { return m_cpTex2D.Get(); }
		inline ID3D11ShaderResourceView* GetSRVComInterface() const { return m_cpSRV.Get(); }
	protected:
		ComPtr<ID3D11Texture2D> m_cpTex2D;
		ComPtr<ID3D11ShaderResourceView> m_cpSRV;
	};
}
