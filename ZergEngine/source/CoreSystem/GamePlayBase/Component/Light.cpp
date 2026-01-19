#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

ILight::ILight(GameObject& owner, uint64_t id)
	: IComponent(owner, id)
{
	XMStoreFloat4A(&m_diffuse, Vector3::One());
	XMStoreFloat4A(&m_specular, Vector3::One());
}

DirectionalLight::DirectionalLight(GameObject& owner)
	: ILight(owner, DirectionalLightManager::GetInstance()->AssignUniqueId())
{
}

IComponentManager* DirectionalLight::GetComponentManager() const
{
	return DirectionalLightManager::GetInstance();
}

PointLight::PointLight(GameObject& owner)
	: ILight(owner, PointLightManager::GetInstance()->AssignUniqueId())
	, m_range(5.0f)
	, m_distAtt(0.0f, 1.0f, 0.0f)
{
}

void PointLight::SetRange(FLOAT range)
{
	if (range < 0.05f)
		range = 0.05f;

	m_range = range;
}

IComponentManager* PointLight::GetComponentManager() const
{
	return PointLightManager::GetInstance();
}

SpotLight::SpotLight(GameObject& owner)
	: ILight(owner, SpotLightManager::GetInstance()->AssignUniqueId())
	, m_range(25.0f)
	, m_innerConeAngle(XMConvertToRadians(10.0f))
	, m_outerConeAngle(XMConvertToRadians(22.5f))
	, m_distAtt(0.0f, 1.0f, 0.0f)
{
}

void SpotLight::SetRange(FLOAT range)
{
	if (range < 0.05f)
		range = 0.05f;

	m_range = range;
}

IComponentManager* SpotLight::GetComponentManager() const
{
	return SpotLightManager::GetInstance();
}
