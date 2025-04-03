#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Texture2D;

	class Material
	{
		friend class Graphics;
		friend class Resource;
		/*
		* illum
		* 0. Color on and Ambient off
		* 1. Color on and Ambient on
		* 2. Highlight on
		* 3. Reflection on and Ray trace on
		* 4. Transparency: Glass on, Reflection: Ray trace on
		* 5. Reflection: Fresnel on and Ray trace on
		* 6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
		* 7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
		* 8. Reflection on and Ray trace off
		* 9. Transparency: Glass on, Reflection: Ray trace off
		* 10. Casts shadows onto invisible surfaces
		*/
	public:
		Material()
			: m_ambient(0.0f, 0.0f, 0.0f, 0.0f)
			, m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
			, m_specular(0.0f, 0.0f, 0.0f, 0.0f)
			, m_baseMap(nullptr)
		{
		}
	public:
		XMFLOAT4A m_ambient;	// R/G/B/A
		XMFLOAT4A m_diffuse;
		XMFLOAT4A m_specular;	// R/G/B/Specular power exponent
		std::shared_ptr<Texture2D> m_baseMap;
	};
}
