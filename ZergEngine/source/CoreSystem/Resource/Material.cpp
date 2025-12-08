#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

Material::Material()
	: m_lightMap()
	, m_diffuseMap()
	, m_normalMap()
	, m_specularMap()
{
	XMStoreFloat4A(&m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
	XMStoreFloat4A(&m_diffuse, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.75f), 1.0f));
	XMStoreFloat4A(&m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 2.0f));
}
