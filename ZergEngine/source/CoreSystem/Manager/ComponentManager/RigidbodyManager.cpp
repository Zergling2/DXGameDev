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

void RigidbodyManager::AddToKinematicBodyGroup(Rigidbody* pRigidbody)
{
	assert(pRigidbody->m_kinematicIndex == (std::numeric_limits<uint32_t>::max)());

	m_kinematicBodys.push_back(pRigidbody);
	pRigidbody->m_kinematicIndex = static_cast<uint32_t>(m_kinematicBodys.size() - 1);
}

void RigidbodyManager::RemoveFromKinematicBodyGroup(Rigidbody* pRigidbody)
{
	assert(pRigidbody->m_kinematicIndex != (std::numeric_limits<uint32_t>::max)());
	assert(pRigidbody->m_kinematicIndex + 1 <= m_kinematicBodys.size());
	assert(m_kinematicBodys[pRigidbody->m_kinematicIndex] == pRigidbody);

	const size_t endIndex = m_kinematicBodys.size() - 1;
	if (pRigidbody->m_kinematicIndex != endIndex)	// 제거될 객체가 중간에 있는 경우 한 칸씩 밀지 않기 위해서 끝에 있는 요소와 스왑
	{
		Rigidbody* pEndItem = m_kinematicBodys[endIndex];

		std::swap(m_kinematicBodys[pRigidbody->m_kinematicIndex], m_kinematicBodys[pEndItem->m_kinematicIndex]);
		std::swap(pRigidbody->m_kinematicIndex, pEndItem->m_kinematicIndex);
	}

	pRigidbody->m_kinematicIndex = (std::numeric_limits<uint32_t>::max)();
	m_kinematicBodys.pop_back();
}

void RigidbodyManager::RemoveDestroyedComponents()
{
	// 1. 물리 월드에서 제거 & Kinematic Body인 경우 m_kinematicBodys 배열에서 포인터 제거
	for (IComponent* pComponent : m_destroyed)
	{
		Rigidbody* pRigidbody = static_cast<Rigidbody*>(pComponent);

		// A. 물리 월드에서 제거
		if (pRigidbody->IsEnabled())	// 활성화된 경우에만 물리 월드에 존재하므로
			Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(pRigidbody->m_upBtRigidBody.get());

		// B. Kinematic Body인 경우 m_kinematicBodys 배열에서 포인터 제거
		if (pRigidbody->IsKinematic())
			this->RemoveFromKinematicBodyGroup(pRigidbody);
	}

	// 2. 부모 기능 호출
	IComponentManager::RemoveDestroyedComponents();
}
