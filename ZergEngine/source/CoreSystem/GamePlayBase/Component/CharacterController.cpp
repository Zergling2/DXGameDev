#include <ZergEngine\CoreSystem\GamePlayBase\Component\CharacterController.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CharacterControllerManager.h>
#include <ZergEngine\CoreSystem\Resource\CapsuleCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet3\BulletDynamics/Character/btKinematicCharacterController.h>

using namespace ze;

CharacterController::CharacterController(GameObject& owner, std::shared_ptr<CapsuleCollider> collider, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
    : IComponent(owner, CharacterControllerManager::GetInstance()->AssignUniqueId())
    , m_inPhysicsWorld(false)
    , m_stepHeight(0.2f)
    , m_jumpSpeed(8.0f)
    , m_fallSpeed(50.0f)
    , m_maxSlope(XMConvertToRadians(45.0f))
    , m_upGhostObject()
    , m_spCollider(std::move(collider))
    , m_upController()
{
    // 1. GhostObject 생성 및 초기화
    m_upGhostObject = std::make_unique<btPairCachingGhostObject>();
    void* pMsb = 0;
    pMsb = SetPtrMSBTag(pMsb);
    m_upGhostObject->setUserPointer(pMsb);      // CharacterController의 collision object는 user pointer로 구분. (충돌 콜백에서 제외시키기 위해서)

    m_upGhostObject->setCollisionShape(m_spCollider->GetCollisionShape());
    m_upGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    XMFLOAT3 pos;
    XMStoreFloat3(&pos, m_pGameObject->m_transform.GetWorldPosition());
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(DXToBt::ConvertVector(pos));
    m_upGhostObject->setWorldTransform(startTransform);


    // 2. CharacterController 생성 및 초기화
    assert(m_spCollider->GetCollisionShape()->isConvex());
    
    XMFLOAT3A up;
    XMStoreFloat3A(&up, Vector3::Up());
    m_upController = std::make_unique<btKinematicCharacterController>(
        m_upGhostObject.get(),
        static_cast<btConvexShape*>(m_spCollider->GetCollisionShape()),
        m_stepHeight,
        DXToBt::ConvertVector(up)
    );

    m_upController->setGravity(DXToBt::ConvertVector(Physics::GetInstance()->GetGravity()));
    m_upController->setJumpSpeed(m_jumpSpeed);
    m_upController->setFallSpeed(m_fallSpeed);
    m_upController->setMaxSlope(m_maxSlope);


    // 물리 월드에 추가
    if (!m_pGameObject->IsPending())
        this->AddToPhysicsWorld();
}

void CharacterController::SetWorldPosition(const XMFLOAT3& pos)
{
    m_upController->warp(DXToBt::ConvertVector(pos));
    // btTransform transform;
    // transform.setIdentity();
    // transform.setOrigin(DXToBt::ConvertVector(pos));
    // 
    // m_upGhostObject->setWorldTransform(transform);
}

void CharacterController::SetWalkDirection(const XMFLOAT3& dir)
{
    m_upController->setWalkDirection(DXToBt::ConvertVector(dir));
}

void CharacterController::SetMaxSlopeRadian(float angle)
{
    m_maxSlope = std::abs(angle);

    m_upController->setMaxSlope(m_maxSlope);
}

void CharacterController::SetMaxSlopeDegree(float angle)
{
    this->SetMaxSlopeRadian(XMConvertToRadians(angle));
}

float CharacterController::GetMaxSlopeDegree() const
{
    return XMConvertToDegrees(m_maxSlope);
}

void CharacterController::SetJumpSpeed(float speed)
{
    m_jumpSpeed = std::abs(speed);

    m_upController->setJumpSpeed(m_jumpSpeed);
}

void CharacterController::SetFallSpeed(float speed)
{
    m_fallSpeed = std::abs(speed);

    m_upController->setFallSpeed(m_fallSpeed);
}

IComponentManager* CharacterController::GetComponentManager() const
{
    return CharacterControllerManager::GetInstance();
}

void CharacterController::OnDeploySysJob()
{
    IComponent::OnDeploySysJob();

    // 물리 월드에 등록
    if (this->IsEnabled())
        this->AddToPhysicsWorld();
}

void CharacterController::OnEnableSysJob()
{
    IComponent::OnEnableSysJob();

    if (!m_inPhysicsWorld)
        this->AddToPhysicsWorld();
}

void CharacterController::OnDisableSysJob()
{
    if (m_inPhysicsWorld)
        this->RemoveFromPhysicsWorld();

    IComponent::OnDisableSysJob();
}

void CharacterController::AddToPhysicsWorld()
{
    assert(!m_inPhysicsWorld);

    Physics::GetInstance()->GetDynamicsWorld()->addCollisionObject(m_upGhostObject.get(), btBroadphaseProxy::CharacterFilter);
    Physics::GetInstance()->GetDynamicsWorld()->addAction(m_upController.get());

    m_inPhysicsWorld = true;
}

void CharacterController::RemoveFromPhysicsWorld()
{
    assert(m_inPhysicsWorld);

    Physics::GetInstance()->GetDynamicsWorld()->removeAction(m_upController.get());
    Physics::GetInstance()->GetDynamicsWorld()->removeCollisionObject(m_upGhostObject.get());

    m_inPhysicsWorld = false;
}
