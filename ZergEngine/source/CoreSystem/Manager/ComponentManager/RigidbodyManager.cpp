#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Rigidbody.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

RigidbodyManager* RigidbodyManager::s_pInstance = nullptr;

void RigidbodyManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new RigidbodyManager();
}

void RigidbodyManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void RigidbodyManager::RemoveDestroyedComponents()
{
	auto& collisionPairs = Physics::GetInstance()->m_prevCollisionPairs;

	// 1. 물리 월드에서 제거
	for (IComponent* pComponent : m_destroyed)
	{
		Rigidbody* pRigidbody = static_cast<Rigidbody*>(pComponent);

		btRigidBody* const pBtRigidBody = pRigidbody->m_upBtRigidBody.get();

		// 충돌쌍 목록에서 제거 (댕글링 포인터 방지)
		auto iter = collisionPairs.begin();
		while (iter != collisionPairs.end())
		{
			if (iter->first == pBtRigidBody || iter->second == pBtRigidBody)
				iter = collisionPairs.erase(iter);
			else
				++iter;
		}

		// 물리 월드에서 제거
		if (pRigidbody->IsEnabled())	// 활성화된 경우에만 물리 월드에 존재하므로
			Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(pBtRigidBody);
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
