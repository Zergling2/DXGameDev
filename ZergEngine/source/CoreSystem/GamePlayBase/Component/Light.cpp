#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

DirectionalLight::DirectionalLight() noexcept
	: ILight(DirectionalLightManager::GetInstance()->AssignUniqueId())
{
}

IComponentManager* DirectionalLight::GetComponentManager() const
{
	return DirectionalLightManager::GetInstance();
}

PointLight::PointLight() noexcept
	: ILight(PointLightManager::GetInstance()->AssignUniqueId())
	, m_range(1.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
}

IComponentManager* PointLight::GetComponentManager() const
{
	return PointLightManager::GetInstance();
}

void PointLight::SetRange(FLOAT range)
{
	if (range < 0.0f)
		range = 0.0f;
}

void PointLight::SetAtt(const XMFLOAT3 att)
{
	SetAtt(&att);
}

void PointLight::SetAtt(const XMFLOAT3* pAtt)
{
	m_att = *pAtt;
}

SpotLight::SpotLight() noexcept
	: ILight(SpotLightManager::GetInstance()->AssignUniqueId())
	, m_range(1.0f)
	, m_spotExp(4.0f)
	, m_att(0.0f, 0.0f, 1.0f)
{
}

IComponentManager* SpotLight::GetComponentManager() const
{
	return SpotLightManager::GetInstance();
}
