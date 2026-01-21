#include <ZergEngine\CoreSystem\GamePlayBase\Component\RigidbodyInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

IRigidbody::IRigidbody(GameObject& owner, std::shared_ptr<ICollider> collider)
	: IComponent(owner, RigidbodyManager::GetInstance()->AssignUniqueId())
	, m_spCollider(std::move(collider))
	, m_upBtRigidBody(nullptr)
{
	assert(m_spCollider != nullptr);
	assert(m_spCollider->GetCollisionShape() != nullptr);
}

float IRigidbody::GetFriction() const
{
	return m_upBtRigidBody->getFriction();
}

void IRigidbody::SetFriction(float friction)
{
	m_upBtRigidBody->setFriction(friction);
}

float IRigidbody::GetRollingFriction() const
{
	return m_upBtRigidBody->getRollingFriction();
}

void IRigidbody::SetRollingFriction(float friction)
{
	m_upBtRigidBody->setRollingFriction(friction);
}

float IRigidbody::GetSpinningFriction() const
{
	return m_upBtRigidBody->getSpinningFriction();
}

void IRigidbody::SetSpinningFriction(float friction)
{
	m_upBtRigidBody->setSpinningFriction(friction);
}

IComponentManager* IRigidbody::GetComponentManager() const
{
	return RigidbodyManager::GetInstance();
}

void IRigidbody::OnDeploySysJob()
{
	IComponent::OnDeploySysJob();

	// 물리 월드에 등록
	if (this->IsEnabled())
		Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void IRigidbody::OnEnableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void IRigidbody::OnDisableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(m_upBtRigidBody.get());
}
