#include <ZergEngine\CoreSystem\GamePlayBase\Component\CollisionTrigger.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CollisionTriggerManager.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision\CollisionDispatch\btGhostObject.h>
#include <cassert>

using namespace ze;

CollisionTrigger::CollisionTrigger(GameObject& owner, std::shared_ptr<ICollider> collider)
	: IComponent(owner, CollisionTriggerManager::GetInstance()->AssignUniqueId())
	, m_spCollider(std::move(collider))
	, m_upBtGhostObject(nullptr)
{
	assert(m_spCollider != nullptr);
	assert(m_spCollider->GetCollisionShape() != nullptr);

	m_upBtGhostObject = std::make_unique<btGhostObject>();
	m_upBtGhostObject->setCollisionShape(m_spCollider->GetCollisionShape());
}

IComponentManager* CollisionTrigger::GetComponentManager() const
{
	return CollisionTriggerManager::GetInstance();
}

void CollisionTrigger::OnDeploySysJob()
{
	IComponent::OnDeploySysJob();

	if (this->IsEnabled())
		Physics::GetInstance()->GetDynamicsWorld()->addCollisionObject(m_upBtGhostObject.get());
}

void CollisionTrigger::OnEnableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->addCollisionObject(m_upBtGhostObject.get());
}

void CollisionTrigger::OnDisableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->removeCollisionObject(m_upBtGhostObject.get());
}
