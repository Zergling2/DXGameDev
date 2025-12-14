#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Material
	{
	public:
		Material();
		~Material() = default;
	public:
		XMFLOAT4A m_diffuse;
		XMFLOAT4A m_specular;	// R/G/B/Specular power exponent
		Texture2D m_lightMap;
		Texture2D m_diffuseMap;
		Texture2D m_normalMap;
		Texture2D m_specularMap;
	};
}
