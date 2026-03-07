#include <ZergEngine\CoreSystem\GamePlayBase\Component\Rigidbody.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

Rigidbody::Rigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: IRigidbody(owner)
	, m_upMotionState(std::make_unique<MotionState>(owner.m_transform, localPos, localRot))
	, m_mass(1.0f)
	, m_useGravity(true)
	, m_isKinematic(false)
	, m_freezeState(FF_None)
{
	m_spCollider = std::move(collider);

	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbci(m_mass, m_upMotionState.get(), m_spCollider->GetCollisionShape(), inertia);

	m_upBtRigidBody = std::make_unique<btRigidBody>(rbci);
	m_upBtRigidBody->setUserPointer(static_cast<IRigidbody*>(this));
}

float Rigidbody::GetMass() const
{
	return m_upBtRigidBody->getMass();
}

void Rigidbody::SetMass(float mass)
{
	m_mass = mass;
	
	if (this->IsKinematic())
		return;

	btVector3 inertia;
	m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);

	m_upBtRigidBody->setMassProps(m_mass, inertia);
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

	// 씬에 배치되어 있는 상태인 경우에만 수행.
	// Step 1. 물리 월드에서 제거
	if (!m_pGameObject->IsPending())
		Physics::GetInstance()->GetDynamicsWorld()->removeRigidBody(m_upBtRigidBody.get());

	// 이 과정은 씬에 배치되어 있지 않은 상태에서도 수행되어야 하는 동작이므로 IsPending 플래그 검사를 하지 않음.
	// Step 2. 플래그 & 상태 재설정
	if (b)
	{
		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
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

		const btVector3 inertia(0.0f, 0.0f, 0.0f);
		m_upBtRigidBody->setMassProps(0.0f, inertia);
	}
	else
	{
		m_upBtRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		m_upBtRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

		m_upBtRigidBody->setCollisionFlags(m_upBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		m_upBtRigidBody->forceActivationState(ACTIVE_TAG);

		btVector3 inertia;
		m_spCollider->GetCollisionShape()->calculateLocalInertia(m_mass, inertia);
		m_upBtRigidBody->setMassProps(m_mass, inertia);

		// sleeping 상태에 있을 수 있으므로 solver가 새로운 질량/관성모멘트를 기준으로 물리력 계산을 하도록 activate
		m_upBtRigidBody->activate(true);
	}

	// Step 3. 물리 월드에 재배치
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
