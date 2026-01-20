#include <ZergEngine\CoreSystem\GamePlayBase\Component\Rigidbody.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

Rigidbody::Rigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: IComponent(owner, RigidbodyManager::GetInstance()->AssignUniqueId())
	, m_upMotionState(std::make_unique<MotionState>(owner.m_transform, localPos, localRot))
	, m_spCollider(std::move(collider))
	, m_upBtRigidBody()
	, m_useGravity(true)
	, m_isKinematic(false)
	, m_freezePositionX(false)
	, m_freezePositionY(false)
	, m_freezePositionZ(false)
	, m_freezeRotationX(false)
	, m_freezeRotationY(false)
	, m_freezeRotationZ(false)
	, m_kinematicIndex((std::numeric_limits<uint32_t>::max)())
{
	assert(m_spCollider != nullptr);
	assert(m_spCollider->GetCollisionShape() != nullptr);

	constexpr float DEFAULT_MASS = 1.0f;

	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(DEFAULT_MASS, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbci(DEFAULT_MASS, m_upMotionState.get(), m_spCollider->GetCollisionShape(), inertia);

	m_upBtRigidBody = std::make_unique<btRigidBody>(rbci);
}

float Rigidbody::GetMass() const
{
	return m_upBtRigidBody->getMass();
}

void Rigidbody::SetMass(float mass)
{
	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(mass, inertia);

	m_upBtRigidBody->setMassProps(mass, inertia);
}

float Rigidbody::GetFriction() const
{
	return m_upBtRigidBody->getFriction();
}

void Rigidbody::SetFriction(float friction)
{
	m_upBtRigidBody->setFriction(friction);
}

float Rigidbody::GetRollingFriction() const
{
	return m_upBtRigidBody->getRollingFriction();
}

void Rigidbody::SetRollingFriction(float friction)
{
	m_upBtRigidBody->setRollingFriction(friction);
}

float Rigidbody::GetSpinningFriction() const
{
	return m_upBtRigidBody->getSpinningFriction();
}

void Rigidbody::SetSpinningFriction(float friction)
{
	m_upBtRigidBody->setSpinningFriction(friction);
}

XMFLOAT3 Rigidbody::GetLinearVelocity() const
{
	return BtToDX::ConvertVector(m_upBtRigidBody->getLinearVelocity());
}

XMFLOAT3 Rigidbody::GetAngularVelocity() const
{
	return BtToDX::ConvertVector(m_upBtRigidBody->getAngularVelocity());
}

void Rigidbody::UseGravity(bool b)
{
	m_useGravity = b;

	if (m_useGravity)
		m_upBtRigidBody->setFlags(m_upBtRigidBody->getFlags() & ~btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
	else
		m_upBtRigidBody->setFlags(m_upBtRigidBody->getFlags() | btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
}

void Rigidbody::SetKinematic(bool b)
{
	if (m_isKinematic == b)
		return;

	m_isKinematic = b;

	// 지연 오브젝트일 경우 더 수행할 작업 없음.
	if (!m_pGameObject->IsPending())
		return;

	m_upBtRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	m_upBtRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	if (b)
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

		RigidbodyManager::GetInstance()->AddToKinematicBodyGroup(this);
	}
	else
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);

		// sleeping 상태에 있을 수 있으므로 solver가 새로운 질량/관성모멘트를 기준으로 물리력 계산을 하도록 activate
		m_upBtRigidBody->activate();

		RigidbodyManager::GetInstance()->RemoveFromKinematicBodyGroup(this);
	}
}

void Rigidbody::FreezePositionX(bool b)
{
	m_freezePositionX = b;

	btVector3 linearFactor = m_upBtRigidBody->getLinearFactor();
	linearFactor.setX(b ? 0.0f : 1.0f);
	
	m_upBtRigidBody->setLinearFactor(linearFactor);
}

void Rigidbody::FreezePositionY(bool b)
{
	m_freezePositionY = b;

	btVector3 linearFactor = m_upBtRigidBody->getLinearFactor();
	linearFactor.setY(b ? 0.0f : 1.0f);

	m_upBtRigidBody->setLinearFactor(linearFactor);
}

void Rigidbody::FreezePositionZ(bool b)
{
	m_freezePositionZ = b;

	btVector3 linearFactor = m_upBtRigidBody->getLinearFactor();
	linearFactor.setZ(b ? 0.0f : 1.0f);

	m_upBtRigidBody->setLinearFactor(linearFactor);
}

void Rigidbody::FreezeRotationX(bool b)
{
	m_freezeRotationX = b;

	btVector3 angularFactor = m_upBtRigidBody->getAngularFactor();
	angularFactor.setX(b ? 0.0f : 1.0f);

	m_upBtRigidBody->setAngularFactor(angularFactor);
}

void Rigidbody::FreezeRotationY(bool b)
{
	m_freezeRotationY = b;

	btVector3 angularFactor = m_upBtRigidBody->getAngularFactor();
	angularFactor.setY(b ? 0.0f : 1.0f);

	m_upBtRigidBody->setAngularFactor(angularFactor);
}

void Rigidbody::FreezeRotationZ(bool b)
{
	m_freezeRotationZ = b;

	btVector3 angularFactor = m_upBtRigidBody->getAngularFactor();
	angularFactor.setZ(b ? 0.0f : 1.0f);

	m_upBtRigidBody->setAngularFactor(angularFactor);
}

IComponentManager* Rigidbody::GetComponentManager() const
{
	return RigidbodyManager::GetInstance();
}

void Rigidbody::OnDeploySysJob()
{
	IComponent::OnDeploySysJob();

	// 물리 월드에 등록
	if (this->IsEnabled())
		Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());

	// Kinematic인 경우 group 인덱스 저장 (빠른 접근)
	if (this->IsKinematic())
		RigidbodyManager::GetInstance()->AddToKinematicBodyGroup(this);
}

void Rigidbody::OnEnableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void Rigidbody::OnDisableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(m_upBtRigidBody.get());
}
