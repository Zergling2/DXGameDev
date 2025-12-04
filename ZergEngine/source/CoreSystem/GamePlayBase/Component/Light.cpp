#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

DirectionalLight::DirectionalLight()
	: ILight(DirectionalLightManager::GetInstance()->AssignUniqueId())
{
}

IComponentManager* DirectionalLight::GetComponentManager() const
{
	return DirectionalLightManager::GetInstance();
}

PointLight::PointLight()
	: ILight(PointLightManager::GetInstance()->AssignUniqueId())
	, m_range(10.0f)
	, m_att(0.0f, 1.0f, 0.0f)
{
}

IComponentManager* PointLight::GetComponentManager() const
{
	return PointLightManager::GetInstance();
}

SpotLight::SpotLight()
	: ILight(SpotLightManager::GetInstance()->AssignUniqueId())
	, m_range(25.0f)
	, m_spotExp(32.0f)
	, m_att(0.0f, 1.0f, 0.0f)
{
}

IComponentManager* SpotLight::GetComponentManager() const
{
	return SpotLightManager::GetInstance();
}
