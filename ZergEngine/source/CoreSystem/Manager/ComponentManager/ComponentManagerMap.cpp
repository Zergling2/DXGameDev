#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>
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
	case ComponentType::Camera:
		pComponentManager = CameraManager::GetInstance();
		break;
	case ComponentType::MeshRenderer:
		pComponentManager = MeshRendererManager::GetInstance();
		break;
	case ComponentType::SkinnedMeshRenderer:
		pComponentManager = SkinnedMeshRendererManager::GetInstance();
		break;
	case ComponentType::Billboard:
		pComponentManager = BillboardManager::GetInstance();
		break;
	case ComponentType::DirectionalLight:
		pComponentManager = DirectionalLightManager::GetInstance();
		break;
	case ComponentType::PointLight:
		pComponentManager = PointLightManager::GetInstance();
		break;
	case ComponentType::SpotLight:
		pComponentManager = SpotLightManager::GetInstance();
		break;
	case ComponentType::MonoBehaviour:
		pComponentManager = MonoBehaviourManager::GetInstance();
		break;
	case ComponentType::Rigidbody:
		// pComponentManager = RigidbodyManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::BoxCollider:
		// pComponentManager = BoxColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::SphereCollider:
		// pComponentManager = SphereColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::MeshCollider:
		// pComponentManager = MeshColliderManager::GetInstance();
		pComponentManager = nullptr;
		break;
	case ComponentType::Terrain:
		pComponentManager = TerrainManager::GetInstance();
		break;
	case ComponentType::AudioSource:
		pComponentManager = AudioSourceManager::GetInstance();
		break;
	default:
		pComponentManager = nullptr;
		break;
	}

	return pComponentManager;
}
