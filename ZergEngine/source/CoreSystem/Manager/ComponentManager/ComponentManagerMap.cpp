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
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>

using namespace ze;

IComponentManager* ComponentManagerMap::GetManager(COMPONENT_TYPE type)
{
	IComponentManager* pComponentManager;

	// 3D GameObject Components
	switch (type)
	{
	case COMPONENT_TYPE::CAMERA:
		pComponentManager = CameraManager::GetInstance();
		break;
	case COMPONENT_TYPE::MESH_RENDERER:
		pComponentManager = MeshRendererManager::GetInstance();
		break;
	case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
		pComponentManager = DirectionalLightManager::GetInstance();
		break;
	case COMPONENT_TYPE::POINT_LIGHT:
		pComponentManager = PointLightManager::GetInstance();
		break;
	case COMPONENT_TYPE::SPOT_LIGHT:
		pComponentManager = SpotLightManager::GetInstance();
		break;
	case COMPONENT_TYPE::MONOBEHAVIOUR:
		pComponentManager = MonoBehaviourManager::GetInstance();
		break;
	case COMPONENT_TYPE::RIGIDBODY:
		// pComponentManager = RigidbodyManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::BOX_COLLIDER:
		// pComponentManager = BoxColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::SPHERE_COLLIDER:
		// pComponentManager = SphereColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::MESH_COLLIDER:
		// pComponentManager = MeshColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case COMPONENT_TYPE::TERRAIN:
		pComponentManager = TerrainManager::GetInstance();
		break;
	case COMPONENT_TYPE::AUDIO_SOURCE:
		pComponentManager = AudioSourceManager::GetInstance();
		break;
	default:
		pComponentManager = nullptr;
		break;
	}

	return pComponentManager;
}
