#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Texture2D
	{
		friend class ResourceManager;
	public:
		Texture2D()
			: m_cpTex2D(nullptr)
			, m_cpSRV(nullptr)
		{
		}
		virtual ~Texture2D() = default;

		inline ID3D11Texture2D* GetTex2DComInterface() const { return m_cpTex2D.Get(); }
		inline ID3D11ShaderResourceView* GetSRVComInterface() const { return m_cpSRV.Get(); }
		inline void GetTexDesc(D3D11_TEXTURE2D_DESC* pDesc) { m_cpTex2D->GetDesc(pDesc); }
	protected:
		ComPtr<ID3D11Texture2D> m_cpTex2D;
		ComPtr<ID3D11ShaderResourceView> m_cpSRV;
	};

	class RenderTargetTexture2D : public Texture2D
	{
	public:
		RenderTargetTexture2D()
			: m_cpRTV(nullptr)
		{
		}
		virtual ~RenderTargetTexture2D()
		{
		}

		inline ID3D11RenderTargetView* GetRTVComInterface() const { return m_cpRTV.Get(); }
	protected:
		ComPtr<ID3D11RenderTargetView> m_cpRTV;
	};
}
