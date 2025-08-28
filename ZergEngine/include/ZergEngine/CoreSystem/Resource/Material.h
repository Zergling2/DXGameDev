#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Material
	{
	public:
		Material()
			: m_lightMap()
			, m_diffuseMap()
			, m_normalMap()
			, m_specularMap()
		{
			XMStoreFloat4A(&m_ambient, g_XMOneHalf);
			XMStoreFloat4A(&m_diffuse, XMVectorSplatOne());
			XMStoreFloat4A(&m_specular, XMVectorSplatOne());
			m_specular.w = 4.0f;
		}
		~Material() = default;
	public:
		XMFLOAT4A m_ambient;	// R/G/B/A
		XMFLOAT4A m_diffuse;
		XMFLOAT4A m_specular;	// R/G/B/Specular power exponent
		Texture2D m_lightMap;
		Texture2D m_diffuseMap;
		Texture2D m_normalMap;
		Texture2D m_specularMap;
	};
}
