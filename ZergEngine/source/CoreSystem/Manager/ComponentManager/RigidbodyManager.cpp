#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\RigidbodyInterface.h>
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
	// 1. 물리 월드에서 제거 & Kinematic Body인 경우 m_kinematicBodys 배열에서 포인터 제거
	for (IComponent* pComponent : m_destroyed)
	{
		IRigidbody* pRigidbody = static_cast<IRigidbody*>(pComponent);

		// 물리 월드에서 제거
		if (pRigidbody->IsEnabled())	// 활성화된 경우에만 물리 월드에 존재하므로
			Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(pRigidbody->m_upBtRigidBody.get());
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
