#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\ComponentSystem\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\PointLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\SpotLightManager.h>

using namespace ze;

DirectionalLight::DirectionalLight()
{
	this->SetId(DirectionalLightManager::GetInstance().AssignUniqueId());
}

IComponentManager* DirectionalLight::GetComponentManager() const
{
	return &DirectionalLightManager::GetInstance();
}

PointLight::PointLight()
	: m_range(1.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
	this->SetId(PointLightManager::GetInstance().AssignUniqueId());
}

IComponentManager* PointLight::GetComponentManager() const
{
	return &PointLightManager::GetInstance();
}

SpotLight::SpotLight()
	: m_range(1.0f)
	, m_spotExp(4.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
	this->SetId(SpotLightManager::GetInstance().AssignUniqueId());
}

IComponentManager* SpotLight::GetComponentManager() const
{
	return &SpotLightManager::GetInstance();
}
