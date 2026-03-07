#include <ZergEngine\CoreSystem\GamePlayBase\Component\RigidbodyInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

IRigidbody::IRigidbody(GameObject& owner)
	: IComponent(owner, RigidbodyManager::GetInstance()->AssignUniqueId())
	, m_listenCollisionEvent(false)
	, m_isTrigger(false)
	, m_spCollider(nullptr)
	, m_upBtRigidBody(nullptr)
{
}

void IRigidbody::ListenCollisionEvent()
{
	m_listenCollisionEvent = true;
}

void IRigidbody::SetTrigger(bool b)
{
	if (m_isTrigger == b)
		return;

	m_isTrigger = b;

	// 이 과정은 씬에 배치되어 있지 않은 상태에서도 수행되어야 하는 동작이므로 IsPending 플래그 검사를 하지 않음.
	// Step 2. 플래그 & 상태 재설정
	if (b)
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	else
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		m_upBtRigidBody->activate(true);
	}
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
