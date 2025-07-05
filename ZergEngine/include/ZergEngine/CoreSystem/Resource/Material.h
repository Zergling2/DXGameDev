#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Material
	{
	public:
		Material()
			: m_ambient(0.0f, 0.0f, 0.0f, 0.0f)
			, m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
			, m_specular(0.0f, 0.0f, 0.0f, 0.0f)
			, m_lightMap()
			, m_diffuseMap()
			, m_normalMap()
			, m_specularMap()
		{
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
