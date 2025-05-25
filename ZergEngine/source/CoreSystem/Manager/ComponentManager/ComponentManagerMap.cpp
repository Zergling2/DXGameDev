#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
// #include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
// #include <ZergEngine\CoreSystem\Manager\ComponentManager\BoxColliderManager.h>
// #include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshColliderManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>

using namespace ze;

IComponentManager* ComponentManagerMap::GetManager(COMPONENT_TYPE type)
{
	IComponentManager* pComponentManager;

	switch (type)
	{
	case COMPONENT_TYPE::CAMERA:
		pComponentManager = &CameraManager;
		break;
	case COMPONENT_TYPE::MESH_RENDERER:
		pComponentManager = &MeshRendererManager;
		break;
	case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
		pComponentManager = &DirectionalLightManager;
		break;
	case COMPONENT_TYPE::POINT_LIGHT:
		pComponentManager = &PointLightManager;
		break;
	case COMPONENT_TYPE::SPOT_LIGHT:
		pComponentManager = &SpotLightManager;
		break;
	case COMPONENT_TYPE::MONOBEHAVIOUR:
		pComponentManager = &MonoBehaviourManager;
		break;
	case COMPONENT_TYPE::RIGIDBODY:
		// pComponentManager = &RigidbodyManager;
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::BOX_COLLIDER:
		// pComponentManager = &BoxColliderManager;
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::MESH_COLLIDER:
		// pComponentManager = &MeshColliderManager;
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::TERRAIN:
		pComponentManager = &TerrainManager;
		break;
	default:
		pComponentManager = nullptr;
		break;
	}

	return pComponentManager;
}
