#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\ComponentSystem\CameraManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\PointLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\SpotLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ScriptManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\TerrainManager.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

IComponentManager* ComponentManagerMap::GetComponentManager(COMPONENT_TYPE type)
{
	switch (type)
	{
	case COMPONENT_TYPE::CAMERA:
		return &CameraManager::GetInstance();
	case COMPONENT_TYPE::MESH_RENDERER:
		return &MeshRendererManager::GetInstance();
	case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
		return &DirectionalLightManager::GetInstance();
	case COMPONENT_TYPE::POINT_LIGHT:
		return &PointLightManager::GetInstance();
	case COMPONENT_TYPE::SPOT_LIGHT:
		return &SpotLightManager::GetInstance();
	case COMPONENT_TYPE::SCRIPT:
		return &ScriptManager::GetInstance();
	case COMPONENT_TYPE::RIGIDBODY:
		return nullptr;
	case COMPONENT_TYPE::BOX_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::SPHERE_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::MESH_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::TERRAIN:
		return &TerrainManager::GetInstance();
	case COMPONENT_TYPE::UNKNOWN:
		return nullptr;
	default:
		Debug::ForceCrashWithMessageBox(L"Error", L"ComponentManagerMap::GetComponentManager()\nUnknown component.");
		return nullptr;
	}
}
