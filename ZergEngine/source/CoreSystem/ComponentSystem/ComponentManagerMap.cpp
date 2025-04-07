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
		return &CameraManager;
	case COMPONENT_TYPE::MESH_RENDERER:
		return &MeshRendererManager;
	case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
		return &DirectionalLightManager;
	case COMPONENT_TYPE::POINT_LIGHT:
		return &PointLightManager;
	case COMPONENT_TYPE::SPOT_LIGHT:
		return &SpotLightManager;
	case COMPONENT_TYPE::SCRIPT:
		return &ScriptManager;
	case COMPONENT_TYPE::RIGIDBODY:
		return nullptr;
	case COMPONENT_TYPE::BOX_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::SPHERE_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::MESH_COLLIDER:
		return nullptr;
	case COMPONENT_TYPE::TERRAIN:
		return &TerrainManager;
	case COMPONENT_TYPE::UNKNOWN:
		return nullptr;
	default:
		Debug::ForceCrashWithMessageBox(L"Error", L"ComponentManagerMap::GetComponentManager()\nUnknown component.");
		return nullptr;
	}
}
