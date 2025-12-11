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

SpotLight::SpotLight()
	: ILight(SpotLightManager::GetInstance()->AssignUniqueId())
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
