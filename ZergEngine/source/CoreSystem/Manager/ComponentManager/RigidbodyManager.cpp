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
	auto& prevCollisionPairs = Physics::GetInstance()->m_prevCollisionPairs;
	auto& currCollisionPairs = Physics::GetInstance()->m_currCollisionPairs;
	assert(currCollisionPairs.size() == 0);

	// 1. 물리 월드에서 제거
	for (IComponent* pComponent : m_destroyed)
	{
		Rigidbody* pRigidbody = static_cast<Rigidbody*>(pComponent);

		btRigidBody* const pBtRigidBody = pRigidbody->m_upBtRigidBody.get();

		// 충돌쌍 목록에서 제거 (댕글링 포인터 방지)
		// 추후 더 빠르게 개선할 수 있는 방법 -> 충돌쌍에 포함되어 있는지 플래그를 저장해두고 플래그가 켜져있을때만 충돌쌍을 탐색해서 제거한다.
		{
			auto iter = prevCollisionPairs.begin();
			while (iter != prevCollisionPairs.end())
			{
				if (iter->first == pBtRigidBody || iter->second == pBtRigidBody)
					iter = prevCollisionPairs.erase(iter);
				else
					++iter;
			}
		}

		{
			auto iter = currCollisionPairs.begin();
			while (iter != currCollisionPairs.end())
			{
				if (iter->first == pBtRigidBody || iter->second == pBtRigidBody)
					iter = currCollisionPairs.erase(iter);
				else
					++iter;
			}
		}

		// 물리 월드에서 제거
		if (pRigidbody->IsEnabled())	// 활성화된 경우에만 물리 월드에 존재하므로
			pRigidbody->RemoveFromPhysicsWorld();
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
