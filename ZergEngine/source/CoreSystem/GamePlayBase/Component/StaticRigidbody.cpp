#include <ZergEngine\CoreSystem\GamePlayBase\Component\StaticRigidbody.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

StaticRigidbody::StaticRigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: IRigidbody(owner)
{
	m_spCollider = std::move(collider);

	constexpr float STATIC_RIGIDBODY_MASS = 0.0f;
	const btVector3 STATIC_RIGIDBODY_INERTIA(0.0f, 0.0f, 0.0f);

	// motion state == nullptr
	btRigidBody::btRigidBodyConstructionInfo rbci(STATIC_RIGIDBODY_MASS, nullptr, m_spCollider->GetCollisionShape(), STATIC_RIGIDBODY_INERTIA);

	m_upBtRigidBody = std::make_unique<btRigidBody>(rbci);
	m_upBtRigidBody->setUserPointer(static_cast<IRigidbody*>(this));

	// 최초 월드 트랜스폼 계산용 (MotionState 계산 로직 이용)
	MotionState ms(owner.m_transform, localPos, localRot);
	btTransform worldTrans;
	ms.getWorldTransform(worldTrans);
	m_upBtRigidBody->setWorldTransform(worldTrans);
}
