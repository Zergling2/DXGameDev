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

IComponentManager* ComponentManagerMap::GetManager(ComponentType type)
{
	IComponentManager* pComponentManager;

	// 3D GameObject Components
	switch (type)
	{
	case ComponentType::CAMERA:
		pComponentManager = CameraManager::GetInstance();
		break;
	case ComponentType::MESH_RENDERER:
		pComponentManager = MeshRendererManager::GetInstance();
		break;
	case ComponentType::DIRECTIONAL_LIGHT:
		pComponentManager = DirectionalLightManager::GetInstance();
		break;
	case ComponentType::POINT_LIGHT:
		pComponentManager = PointLightManager::GetInstance();
		break;
	case ComponentType::SPOT_LIGHT:
		pComponentManager = SpotLightManager::GetInstance();
		break;
	case ComponentType::MONOBEHAVIOUR:
		pComponentManager = MonoBehaviourManager::GetInstance();
		break;
	case ComponentType::RIGIDBODY:
		// pComponentManager = RigidbodyManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::BOX_COLLIDER:
		// pComponentManager = BoxColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::SPHERE_COLLIDER:
		// pComponentManager = SphereColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::MESH_COLLIDER:
		// pComponentManager = MeshColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::TERRAIN:
		pComponentManager = TerrainManager::GetInstance();
		break;
	case ComponentType::AUDIO_SOURCE:
		pComponentManager = AudioSourceManager::GetInstance();
		break;
	default:
		pComponentManager = nullptr;
		break;
	}

	return pComponentManager;
}
