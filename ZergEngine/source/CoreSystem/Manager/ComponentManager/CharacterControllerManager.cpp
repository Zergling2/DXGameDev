#include <ZergEngine\CoreSystem\Manager\ComponentManager\CharacterControllerManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\CharacterController.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet3\BulletDynamics/Character/btKinematicCharacterController.h>
#include <cassert>

using namespace ze;

CharacterControllerManager* CharacterControllerManager::s_pInstance = nullptr;

void CharacterControllerManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new CharacterControllerManager();
}

void CharacterControllerManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void CharacterControllerManager::SyncTransformFromPhysicsEngine()
{
	for (IComponent* pComponent : m_directAccessGroup)
	{
		if (!pComponent->IsEnabled())
			continue;

		CharacterController* pCharacterController = static_cast<CharacterController*>(pComponent);

		const btTransform& wt = pCharacterController->m_upGhostObject->getWorldTransform();

		// 캐릭터 컨트롤러의 오브젝트는 회전 개념 X.
		// Translation 성분만 고려한다.
		pCharacterController->m_pGameObject->m_transform.SetWorldPosition(BtToDX::ConvertVector(wt.getOrigin()));
	}
}

void CharacterControllerManager::RemoveDestroyedComponents()
{
	// 1. 물리 월드에서 제거
	for (IComponent* pComponent : m_destroyed)
	{
		CharacterController* pCharacterController = static_cast<CharacterController*>(pComponent);

		// 제거
		if (pCharacterController->IsEnabled())
			pCharacterController->RemoveFromPhysicsWorld();
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
