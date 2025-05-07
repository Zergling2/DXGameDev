#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
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
		Debug::ForceCrashWithMessageBox(L"Error", L"RIGIDBODY Component Manager is not implemented.");
		return nullptr;
	case COMPONENT_TYPE::BOX_COLLIDER:
		Debug::ForceCrashWithMessageBox(L"Error", L"BOX_COLLIDER Component Manager is not implemented.");
		return nullptr;
	case COMPONENT_TYPE::SPHERE_COLLIDER:
		Debug::ForceCrashWithMessageBox(L"Error", L"SPHERE_COLLIDER Component Manager is not implemented.");
		return nullptr;
	case COMPONENT_TYPE::MESH_COLLIDER:
		Debug::ForceCrashWithMessageBox(L"Error", L"MESH_COLLIDER Component Manager is not implemented.");
		return nullptr;
	case COMPONENT_TYPE::TERRAIN:
		return &TerrainManager;
	case COMPONENT_TYPE::UNKNOWN:
		__fallthrough;
	default:
		Debug::ForceCrashWithMessageBox(L"Error", L"ComponentManagerMap error.\nUnknown component.");
		return nullptr;
	}
}
