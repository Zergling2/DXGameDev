#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Texture2D
	{
		friend class Graphics;
		friend class Resource;
	public:
		Texture2D()
			: m_texture()
			, m_srv()
		{
		}
		inline ID3D11Texture2D* GetTextureInterface() const { return m_texture.Get(); }
		inline ID3D11ShaderResourceView* GetShaderResourceViewInterface() const { return m_srv.Get(); }
	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};

	class RenderTargetTexture2D
	{
		friend class Graphics;
		friend class Resource;
	public:
		RenderTargetTexture2D()
			: m_texture()
			, m_rtv()
			, m_srv()
		{
		}
		inline ID3D11Texture2D* GetTextureInterface() const { return m_texture.Get(); }
		inline ID3D11RenderTargetView* GetRenderTargetViewInterface() const { return m_rtv.Get(); }
		inline ID3D11ShaderResourceView* GetShaderResourceViewInterface() const { return m_srv.Get(); }
	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11RenderTargetView> m_rtv;
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};
}
