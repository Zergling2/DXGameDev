#include <ZergEngine\CoreSystem\GamePlayBase\Component\StaticRigidbody.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>
#include <bullet3\btBulletDynamicsCommon.h>

using namespace ze;

StaticRigidbody::StaticRigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: IRigidbody(owner, std::move(collider))
{
	assert(m_spCollider != nullptr);
	assert(m_spCollider->GetCollisionShape() != nullptr);

	constexpr float STATIC_RIGIDBODY_MASS = 0.0f;
	const btVector3 STATIC_RIGIDBODY_INERTIA(0.0f, 0.0f, 0.0f);

	// motion state == nullptr
	btRigidBody::btRigidBodyConstructionInfo rbci(0.0f, nullptr, m_spCollider->GetCollisionShape(), STATIC_RIGIDBODY_INERTIA);

	m_upBtRigidBody = std::make_unique<btRigidBody>(rbci);

	// 최초 월드 트랜스폼 계산용 (MotionState 계산 로직 이용)
	MotionState ms(owner.m_transform, localPos, localRot);
	btTransform worldTrans;
	ms.getWorldTransform(worldTrans);
	m_upBtRigidBody->setWorldTransform(worldTrans);
}
