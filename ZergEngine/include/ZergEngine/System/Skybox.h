#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Skybox
	{
		friend class Graphics;
		friend class Resource;
	public:
		inline ID3D11ShaderResourceView* GetShaderResourceView() const { return m_srv.Get(); }
		inline ComPtr<ID3D11ShaderResourceView> GetShaderResourceViewComPtr() const { return m_srv; }
	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};
}