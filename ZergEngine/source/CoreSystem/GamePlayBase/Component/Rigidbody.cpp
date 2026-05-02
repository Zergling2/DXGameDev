#include <ZergEngine\CoreSystem\GamePlayBase\Component\Rigidbody.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

Rigidbody::Rigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: IComponent(owner, RigidbodyManager::GetInstance()->AssignUniqueId())
	, m_listenCollisionEvent(false)
	, m_isTrigger(false)
	, m_useGravity(true)
	, m_bodyType(RigidbodyType::Dynamic)
	, m_spCollider(nullptr)
	, m_upBtRigidBody(nullptr)
	, m_upMotionState(std::make_unique<MotionState>(owner.m_transform, localPos, localRot))
	, m_mass(1.0f)
	, m_freezeState(FF_None)
{
	m_spCollider = std::move(collider);

	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbci(m_mass, m_upMotionState.get(), m_spCollider->GetCollisionShape(), inertia);

	m_upBtRigidBody = std::make_unique<btRigidBody>(rbci);
	m_upBtRigidBody->setUserPointer(static_cast<Rigidbody*>(this));

	if (!owner.IsPending())
		Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void Rigidbody::ListenCollisionEvent()
{
	m_listenCollisionEvent = true;
}

float Rigidbody::GetMass() const
{
	return m_upBtRigidBody->getMass();
}

float Rigidbody::GetInvMass() const
{
	return m_upBtRigidBody->getInvMass();
}

void Rigidbody::SetMass(float mass)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	if (mass <= 0.0f)
		return;

	m_mass = mass;

	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);

	m_upBtRigidBody->setMassProps(m_mass, inertia);
	m_upBtRigidBody->updateInertiaTensor();
}

XMFLOAT3 Rigidbody::GetLinearVelocity() const
{
	return BtToDX::ConvertVector(m_upBtRigidBody->getLinearVelocity());
}

void Rigidbody::SetLinearVelocity(const XMFLOAT3& vel)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->setLinearVelocity(DXToBt::ConvertVector(vel));
}

XMFLOAT3 Rigidbody::GetAngularVelocity() const
{
	return BtToDX::ConvertVector(m_upBtRigidBody->getAngularVelocity());
}

void Rigidbody::SetAngularVelocity(const XMFLOAT3& vel)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->setAngularVelocity(DXToBt::ConvertVector(vel));
}

void Rigidbody::ApplyForce(const XMFLOAT3& force, const XMFLOAT3& relPos)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->applyForce(DXToBt::ConvertVector(force), DXToBt::ConvertVector(relPos));
}

void Rigidbody::ApplyCentralForce(const XMFLOAT3& force)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->applyCentralForce(DXToBt::ConvertVector(force));
}

void Rigidbody::ApplyImpulse(const XMFLOAT3& impulse, const XMFLOAT3& relPos)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->applyImpulse(DXToBt::ConvertVector(impulse), DXToBt::ConvertVector(relPos));
}

void Rigidbody::ApplyCentralImpulse(const XMFLOAT3& impulse)
{
	if (m_bodyType != RigidbodyType::Dynamic)
		return;

	m_upBtRigidBody->activate(true);
	m_upBtRigidBody->applyCentralImpulse(DXToBt::ConvertVector(impulse));
}

void Rigidbody::UseGravity(bool b)
{
	m_useGravity = b;

	if (m_useGravity)
		m_upBtRigidBody->setFlags(m_upBtRigidBody->getFlags() & ~btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
	else
		m_upBtRigidBody->setFlags(m_upBtRigidBody->getFlags() | btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
}

void Rigidbody::SetBodyType(RigidbodyType bodyType)
{
	if (m_bodyType == bodyType)
		return;

	m_bodyType = bodyType;

	// 1. 물리 월드에서 제거
	if (!m_pGameObject->IsPending())
		Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(m_upBtRigidBody.get());


	// 2. 상태 업데이트

	// 월드 트랜스폼 재계산용 (MotionState 계산 로직 이용)
	MotionState ms(m_pGameObject->m_transform, this->GetColliderLocalPos(), this->GetColliderLocalRot());
	btTransform worldTrans;
	ms.getWorldTransform(worldTrans);
	m_upBtRigidBody->setWorldTransform(worldTrans);

	switch (m_bodyType)
	{
	case RigidbodyType::Dynamic:
	{
		assert(m_mass > 0.0f);

		btVector3 inertia;
		m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);

		m_upBtRigidBody->setMassProps(m_mass, inertia);
		m_upBtRigidBody->updateInertiaTensor();

		int cf = m_upBtRigidBody->getCollisionFlags();
		cf = cf & ~(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT);
		cf = cf | btCollisionObject::CF_DYNAMIC_OBJECT;
		m_upBtRigidBody->setCollisionFlags(cf);

		m_upBtRigidBody->activate(true);
		// m_upBtRigidBody->forceActivationState(ACTIVE_TAG);
	}
	break;
	case RigidbodyType::Kinematic:
	{
		const btVector3 inertia(0.0f, 0.0f, 0.0f);
		m_upBtRigidBody->setMassProps(0.0f, inertia);	// Kinematic body -> 질량 0
		m_upBtRigidBody->updateInertiaTensor();

		int cf = m_upBtRigidBody->getCollisionFlags();
		cf = cf & ~(btCollisionObject::CF_DYNAMIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
		cf = cf | btCollisionObject::CF_KINEMATIC_OBJECT;
		m_upBtRigidBody->setCollisionFlags(cf);

		m_upBtRigidBody->setActivationState(DISABLE_DEACTIVATION);	// Bullet_User_Manual.pdf p.22
		/*
		* If you plan to animate or move static objects, you should flag them as kinematic. Also disable the
		* sleeping/deactivation for them during the animation. This means Bullet dynamics world will get the
		* new worldtransform from the btMotionState every simulation frame.
		*
		* If you are using kinematic bodies, then getWorldTransform is called every simulation step. This
		* means that your kinematic body's motionstate should have a mechanism to push the current position
		* of the kinematic body into the motionstate.
		*/
	}
	break;
	case RigidbodyType::Static:
	{
		const btVector3 inertia(0.0f, 0.0f, 0.0f);
		m_upBtRigidBody->setMassProps(0.0f, inertia);
		m_upBtRigidBody->updateInertiaTensor();

		int cf = m_upBtRigidBody->getCollisionFlags();
		cf = cf & ~(btCollisionObject::CF_DYNAMIC_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT);
		cf = cf | btCollisionObject::CF_STATIC_OBJECT;
		m_upBtRigidBody->setCollisionFlags(cf);
	}
	break;
	default:
		break;
	}

	// 선속도 및 각속도 제거
	m_upBtRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	m_upBtRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	// 3. 물리 월드에 다시 추가
	// 씬에 배치되어 있는 상태인 경우에만 수행.
	if (!m_pGameObject->IsPending())
		Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void Rigidbody::GetFreezePosition(bool& freezeX, bool& freezeY, bool& freezeZ)
{
	freezeX = m_freezeState & FF_PositionX;
	freezeY = m_freezeState & FF_PositionY;
	freezeZ = m_freezeState & FF_PositionZ;
}

void Rigidbody::SetFreezePosition(bool freezeX, bool freezeY, bool freezeZ)
{
	freezeX ? m_freezeState |= FF_PositionX : m_freezeState &= ~FF_PositionX;
	freezeY ? m_freezeState |= FF_PositionY : m_freezeState &= ~FF_PositionY;
	freezeZ ? m_freezeState |= FF_PositionZ : m_freezeState &= ~FF_PositionZ;

	btVector3 linearFactor;

	m_freezeState & FF_PositionX ? linearFactor.setX(0.0f) : linearFactor.setX(1.0f);
	m_freezeState & FF_PositionY ? linearFactor.setY(0.0f) : linearFactor.setY(1.0f);
	m_freezeState & FF_PositionZ ? linearFactor.setZ(0.0f) : linearFactor.setZ(1.0f);

	m_upBtRigidBody->setLinearFactor(linearFactor);
}

void Rigidbody::GetFreezeRotation(bool& freezeX, bool& freezeY, bool& freezeZ)
{
	freezeX = m_freezeState & FF_RotationX;
	freezeY = m_freezeState & FF_RotationY;
	freezeZ = m_freezeState & FF_RotationZ;
}

void Rigidbody::SetFreezeRotation(bool freezeX, bool freezeY, bool freezeZ)
{
	freezeX ? m_freezeState |= FF_RotationX : m_freezeState &= ~FF_RotationX;
	freezeY ? m_freezeState |= FF_RotationY : m_freezeState &= ~FF_RotationY;
	freezeZ ? m_freezeState |= FF_RotationZ : m_freezeState &= ~FF_RotationZ;

	btVector3 angularFactor;

	m_freezeState & FF_RotationX ? angularFactor.setX(0.0f) : angularFactor.setX(1.0f);
	m_freezeState & FF_RotationY ? angularFactor.setY(0.0f) : angularFactor.setY(1.0f);
	m_freezeState & FF_RotationZ ? angularFactor.setZ(0.0f) : angularFactor.setZ(1.0f);

	m_upBtRigidBody->setAngularFactor(angularFactor);
}

void Rigidbody::SetTrigger(bool b)
{
	if (m_isTrigger == b)
		return;

	m_isTrigger = b;

	// 이 과정은 씬에 배치되어 있지 않은 상태에서도 수행되어야 하는 동작이므로 IsPending 플래그 검사를 하지 않음.
	// Step 2. 플래그 & 상태 재설정
	if (b)
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		m_upBtRigidBody->activate(true);
	}
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
}

void Rigidbody::OnEnableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->addRigidBody(m_upBtRigidBody.get());
}

void Rigidbody::OnDisableSysJob()
{
	Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(m_upBtRigidBody.get());
}


const XMFLOAT3& Rigidbody::GetColliderLocalPos() const
{
	return m_upMotionState->GetLocalPos();
}

const XMFLOAT4& Rigidbody::GetColliderLocalRot() const
{
	return m_upMotionState->GetLocalRot();
}
