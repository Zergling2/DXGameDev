#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\ComponentSystem\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\PointLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\SpotLightManager.h>

using namespace ze;

DirectionalLight::DirectionalLight()
{
	this->SetId(DirectionalLightManager.AssignUniqueId());
}

IComponentManager* DirectionalLight::GetComponentManager() const
{
	return &DirectionalLightManager;
}

PointLight::PointLight()
	: m_range(1.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
	this->SetId(PointLightManager.AssignUniqueId());
}

IComponentManager* PointLight::GetComponentManager() const
{
	return &PointLightManager;
}

SpotLight::SpotLight()
	: m_range(1.0f)
	, m_spotExp(4.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
	this->SetId(SpotLightManager.AssignUniqueId());
}

IComponentManager* SpotLight::GetComponentManager() const
{
	return &SpotLightManager;
}
