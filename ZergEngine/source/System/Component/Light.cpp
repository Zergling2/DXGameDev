#include <ZergEngine\System\Component\Light.h>
#include <ZergEngine\System\ComponentSystem.h>

using namespace zergengine;

bool DirectionalLight::IsCreatable()
{
	return ComponentSystem::LightManager::GetDirectionalLightCount() < MAX_GLOBAL_LIGHT_COUNT;
}

void DirectionalLight::SystemJobOnEnabled()
{
}

void DirectionalLight::SystemJobOnDisabled()
{
}

bool PointLight::IsCreatable()
{
	return ComponentSystem::LightManager::GetPointLightCount() < MAX_GLOBAL_LIGHT_COUNT;
}

void PointLight::SystemJobOnEnabled()
{
}

void PointLight::SystemJobOnDisabled()
{
}


bool SpotLight::IsCreatable()
{
	return ComponentSystem::LightManager::GetSpotLightCount() < MAX_GLOBAL_LIGHT_COUNT;
}

void SpotLight::SystemJobOnEnabled()
{
}

void SpotLight::SystemJobOnDisabled()
{
}
