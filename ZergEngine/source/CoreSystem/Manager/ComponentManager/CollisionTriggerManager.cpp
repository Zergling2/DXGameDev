#include <ZergEngine\CoreSystem\Manager\ComponentManager\CollisionTriggerManager.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\CollisionTrigger.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision\CollisionDispatch\btGhostObject.h>

using namespace ze;

CollisionTriggerManager* CollisionTriggerManager::s_pInstance = nullptr;

void CollisionTriggerManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new CollisionTriggerManager();
}

void CollisionTriggerManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void CollisionTriggerManager::RemoveDestroyedComponents()
{
	// 물리 월드에서 제거
	for (IComponent* pComponent : m_destroyed)
	{
		CollisionTrigger* pCollisionTrigger = static_cast<CollisionTrigger*>(pComponent);

		if (pCollisionTrigger->IsEnabled())	// 활성화된 경우에만 물리 월드에 존재하므로
			Physics::GetInstance()->GetDynamicsWorld()->removeCollisionObject(pCollisionTrigger->m_upBtGhostObject.get());
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
