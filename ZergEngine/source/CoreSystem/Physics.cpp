#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Rigidbody.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Resource\BoxCollider.h>
#include <ZergEngine\CoreSystem\Resource\SphereCollider.h>
#include <ZergEngine\CoreSystem\Resource\CapsuleCollider.h>
#include <bullet3\btBulletCollisionCommon.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision\CollisionDispatch\btGhostObject.h>
#include <cassert>

using namespace ze;

Physics* Physics::s_pInstance = nullptr;

struct AllConvexResultCallbackNotInclude : public btCollisionWorld::ConvexResultCallback
{
public:
	AllConvexResultCallbackNotInclude(std::vector<SweepHit>& results, const btCollisionObject* pExcept)
		: m_results(results)
		, m_pExcept(pExcept)
	{
	}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		if (!btCollisionWorld::ConvexResultCallback::needsCollision(proxy0))
			return false;

		const btCollisionObject* pObj = static_cast<const btCollisionObject*>(proxy0->m_clientObject);

		if (pObj == m_pExcept)
			return false;

		return true;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& result, bool normalInWorldSpace) override
	{
		SweepHit h;

		h.m_pHitObject = static_cast<Rigidbody*>(result.m_hitCollisionObject->getUserPointer());

		const btVector3 hitNormalWorld = normalInWorldSpace ?
			result.m_hitNormalLocal : result.m_hitCollisionObject->getWorldTransform().getBasis() * result.m_hitNormalLocal;
		h.m_hitNormalWorld = BtToDX::ConvertVector(hitNormalWorld);

		h.m_hitPointWorld = BtToDX::ConvertVector(result.m_hitPointLocal);

		h.m_hitFraction = result.m_hitFraction;

		m_results.push_back(h);

		return result.m_hitFraction;
	}
public:
	std::vector<SweepHit>& m_results;
private:
	const btCollisionObject* m_pExcept;
};

struct AllConvexResultCallbackNotIncludeExceptTrigger : public AllConvexResultCallbackNotInclude
{
	AllConvexResultCallbackNotIncludeExceptTrigger(std::vector<SweepHit>& results, const btCollisionObject* pExcept)
		: AllConvexResultCallbackNotInclude(results, pExcept)
	{
	}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		if (!AllConvexResultCallbackNotInclude::needsCollision(proxy0))
			return false;

		const btCollisionObject* pObj = static_cast<const btCollisionObject*>(proxy0->m_clientObject);
		const Rigidbody* pRigidbody = static_cast<const Rigidbody*>(pObj->getUserPointer());
		if (pRigidbody->IsTrigger())
			return false;

		return true;
	}
};

struct ClosestConvexResultCallbackNotInclude : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	ClosestConvexResultCallbackNotInclude(const btVector3& convexFromWorld, const btVector3& convexToWorld, const btCollisionObject* pExcept)
		: btCollisionWorld::ClosestConvexResultCallback(convexFromWorld, convexToWorld)
		, m_pExcept(pExcept)
	{
	}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
	{
		if (!btCollisionWorld::ClosestConvexResultCallback::needsCollision(proxy0))
			return false;

		const btCollisionObject* pObj = static_cast<const btCollisionObject*>(proxy0->m_clientObject);

		if (pObj == m_pExcept)
			return false;

		return true;
	}
private:
	const btCollisionObject* m_pExcept;
};

struct ClosestConvexResultCallbackNotIncludeExceptTrigger : public ClosestConvexResultCallbackNotInclude
{
	ClosestConvexResultCallbackNotIncludeExceptTrigger(const btVector3& convexFromWorld, const btVector3& convexToWorld, const btCollisionObject* pExcept)
		: ClosestConvexResultCallbackNotInclude(convexFromWorld, convexToWorld, pExcept)
	{
	}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
	{
		if (!ClosestConvexResultCallbackNotInclude::needsCollision(proxy0))
			return false;

		const btCollisionObject* pObj = static_cast<const btCollisionObject*>(proxy0->m_clientObject);
		const Rigidbody* pRigidbody = static_cast<const Rigidbody*>(pObj->getUserPointer());
		if (pRigidbody->IsTrigger())
			return false;

		return true;
	}
};

void Physics::DrawDebugInfo(bool b)
{
	m_drawDebugInfo = b;

	if (m_drawDebugInfo)
		m_upDynamicsWorld->setDebugDrawer(m_upDebugDrawer.get());
	else
		m_upDynamicsWorld->setDebugDrawer(nullptr);
}

void Physics::SetGravity(const XMFLOAT3& gravity)
{
	m_upDynamicsWorld->setGravity(DXToBt::ConvertVector(gravity));
}

size_t XM_CALLCONV Physics::ConvexSweepTestAllNotInclude(std::vector<SweepHit>& results, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept)
{
	results.clear();

	do
	{
		if (!pCollider->GetCollisionShape()->isConvex())
			break;

		btTransform from = DXToBt::ConvertMatrix(transform);
		btTransform to = from;

		to.setOrigin(from.getOrigin() + DXToBt::ConvertVector(move));

		const btConvexShape* pBtConvexShape = static_cast<const btConvexShape*>(pCollider->GetCollisionShape());

		AllConvexResultCallbackNotInclude cb(results, pExcept->m_upBtRigidBody.get());
		m_upDynamicsWorld->convexSweepTest(pBtConvexShape, from, to, cb);
	} while (false);

	return results.size();
}

size_t XM_CALLCONV Physics::ConvexSweepTestAllNotIncludeExceptTrigger(std::vector<SweepHit>& results, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept)
{
	results.clear();

	do
	{
		if (!pCollider->GetCollisionShape()->isConvex())
			break;

		btTransform from = DXToBt::ConvertMatrix(transform);
		btTransform to = from;

		to.setOrigin(from.getOrigin() + DXToBt::ConvertVector(move));

		const btConvexShape* pBtConvexShape = static_cast<const btConvexShape*>(pCollider->GetCollisionShape());

		AllConvexResultCallbackNotIncludeExceptTrigger cb(results, pExcept->m_upBtRigidBody.get());
		m_upDynamicsWorld->convexSweepTest(pBtConvexShape, from, to, cb);
	} while (false);

	return results.size();
}

bool XM_CALLCONV Physics::ConvexSweepTestClosestNotInclude(SweepHit& result, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept)
{
	btTransform from = DXToBt::ConvertMatrix(transform);
	btTransform to = from;

	to.setOrigin(from.getOrigin() + DXToBt::ConvertVector(move));

	const btConvexShape* pBtConvexShape = static_cast<const btConvexShape*>(pCollider->GetCollisionShape());
	
	ClosestConvexResultCallbackNotInclude cb(from.getOrigin(), to.getOrigin(), pExcept->m_upBtRigidBody.get());

	m_upDynamicsWorld->convexSweepTest(pBtConvexShape, from, to, cb);

	if (cb.hasHit())
	{
		result.m_pHitObject = static_cast<Rigidbody*>(cb.m_hitCollisionObject->getUserPointer());
		result.m_hitNormalWorld = BtToDX::ConvertVector(cb.m_hitNormalWorld);
		result.m_hitPointWorld = BtToDX::ConvertVector(cb.m_hitPointWorld);
		result.m_hitFraction = cb.m_closestHitFraction;
		return true;
	}
	else
	{
		return false;
	}
}

bool XM_CALLCONV Physics::ConvexSweepTestClosestNotIncludeExceptTrigger(SweepHit& result, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept)
{
	btTransform from = DXToBt::ConvertMatrix(transform);
	btTransform to = from;

	to.setOrigin(from.getOrigin() + DXToBt::ConvertVector(move));

	const btConvexShape* pBtConvexShape = static_cast<const btConvexShape*>(pCollider->GetCollisionShape());

	ClosestConvexResultCallbackNotIncludeExceptTrigger cb(from.getOrigin(), to.getOrigin(), pExcept->m_upBtRigidBody.get());

	m_upDynamicsWorld->convexSweepTest(pBtConvexShape, from, to, cb);

	if (cb.hasHit())
	{
		result.m_pHitObject = static_cast<Rigidbody*>(cb.m_hitCollisionObject->getUserPointer());
		result.m_hitNormalWorld = BtToDX::ConvertVector(cb.m_hitNormalWorld);
		result.m_hitPointWorld = BtToDX::ConvertVector(cb.m_hitPointWorld);
		result.m_hitFraction = cb.m_closestHitFraction;
		return true;
	}
	else
	{
		return false;
	}
}

RayHit Physics::ClosestRaycastTest(const XMFLOAT3& fromWorld, const XMFLOAT3& toWorld)
{
	const btVector3 btFromWorld = DXToBt::ConvertVector(fromWorld);
	const btVector3 btToWorld = DXToBt::ConvertVector(toWorld);

	btCollisionWorld::ClosestRayResultCallback cb(btFromWorld, btToWorld);
	m_upDynamicsWorld->rayTest(btFromWorld, btToWorld, cb);

	RayHit result;
	if (cb.hasHit())
	{
		result.m_pHitObject = static_cast<Rigidbody*>(cb.m_collisionObject->getUserPointer());
		result.m_hitNormalWorld = BtToDX::ConvertVector(cb.m_hitNormalWorld);
		result.m_hitPointWorld = BtToDX::ConvertVector(cb.m_hitPointWorld);
		result.m_hitFraction = cb.m_closestHitFraction;
	}
	else
	{
		result.m_pHitObject = nullptr;
	}

	return result;
}

std::vector<RayHit> Physics::RaycastTest(const XMFLOAT3& fromWorld, const XMFLOAT3& toWorld)
{
	const btVector3 btFromWorld = DXToBt::ConvertVector(fromWorld);
	const btVector3 btToWorld = DXToBt::ConvertVector(toWorld);

	btCollisionWorld::AllHitsRayResultCallback cb(btFromWorld, btToWorld);
	m_upDynamicsWorld->rayTest(btFromWorld, btToWorld, cb);

	int hitCount = cb.m_collisionObjects.size();

	std::vector<RayHit> result;
	for (int i = 0; i < hitCount; ++i)
	{
		const btCollisionObject* obj = cb.m_collisionObjects[i];

		RayHit hit;
		hit.m_pHitObject = static_cast<Rigidbody*>(cb.m_collisionObjects[i]->getUserPointer());
		hit.m_hitNormalWorld = BtToDX::ForwardVector(cb.m_hitNormalWorld[i]);
		hit.m_hitPointWorld = BtToDX::ForwardVector(cb.m_hitPointWorld[i]);
		hit.m_hitFraction = cb.m_hitFractions[i];

		result.push_back(hit);
	}

	return result;
}

Physics::Physics()
	: m_drawDebugInfo(true)
	, m_gravity(0.0f, 0.0f, 0.0f)
	, m_upDebugDrawer()
	, m_upCollisionConfiguration()
	, m_upDispatcher()
	, m_upBroadphase()
	, m_upSolver()
	, m_upGhostPairCallback()
	, m_upDynamicsWorld()
	, m_prevCollisionPairs()
	, m_currCollisionPairs()
{
}

Physics::~Physics()
{
}

void Physics::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new Physics();
}

void Physics::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

bool Physics::Init()
{
	m_gravity.x = 0.0f;
	m_gravity.y = -9.81f;
	m_gravity.z = 0.0f;

	m_upDebugDrawer = std::make_unique<PhysicsDebugDrawer>();

	// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_upCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_upDispatcher = std::make_unique<btCollisionDispatcher>(m_upCollisionConfiguration.get());
	
	// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_upBroadphase = std::make_unique<btDbvtBroadphase>();
	m_upGhostPairCallback = std::make_unique<btGhostPairCallback>();
	m_upBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_upGhostPairCallback.get());

	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_upSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_upDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_upDispatcher.get(),
		m_upBroadphase.get(),
		m_upSolver.get(),
		m_upCollisionConfiguration.get()
	);

	// GhostPairCallback 등록

	// 디버그 렌더러 초기화 및 등록
	bool init = m_upDebugDrawer->Init(GraphicDevice::GetInstance()->GetDevice());
	if (!init)
		return false;

	m_upDebugDrawer->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe |
		// btIDebugDraw::DBG_DrawAabb |
		btIDebugDraw::DBG_DrawContactPoints |
		btIDebugDraw::DBG_DrawConstraints |
		btIDebugDraw::DBG_DrawFrames
	);
	m_upDynamicsWorld->setDebugDrawer(m_upDebugDrawer.get());

	// 중력 설정
	m_upDynamicsWorld->setGravity(DXToBt::ConvertVector(m_gravity));
	
	

	return true;
}

void Physics::Shutdown()
{
	m_upDynamicsWorld->setDebugDrawer(nullptr);

	m_upDebugDrawer->Release();
	m_upDebugDrawer.reset();

	m_upDynamicsWorld.reset();

	m_upSolver.reset();

	m_upBroadphase.reset();
	m_upGhostPairCallback.reset();

	m_upDispatcher.reset();

	m_upCollisionConfiguration.reset();
}

void Physics::UpdateDebugDrawerResources(ID3D11DeviceContext* pDeviceContext)
{
	m_upDebugDrawer->UpdateResources(GraphicDevice::GetInstance()->GetImmediateContext());
}

void Physics::ClearDebugDrawerCache()
{
	m_upDebugDrawer->ClearDebugDrawerCache();
}

void Physics::StepSimulation(float timeStep, int maxSubSteps, float fixedTimeStep)
{
	m_upDynamicsWorld->stepSimulation(timeStep, maxSubSteps, fixedTimeStep);
}

void Physics::DispatchTriggerEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerEnter();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerEnter();
	}
}

void Physics::DispatchTriggerStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerStay();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerStay();
	}
}

void Physics::DispatchTriggerExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerExit();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnTriggerExit();
	}
}

void Physics::DispatchCollisionEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionEnter();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionEnter();
	}
}

void Physics::DispatchCollisionStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionStay();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionStay();
	}
}

void Physics::DispatchCollisionExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair)
{
	Rigidbody* pRbA = static_cast<Rigidbody*>(pair.first->getUserPointer());
	Rigidbody* pRbB = static_cast<Rigidbody*>(pair.second->getUserPointer());

	if (pRbA->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbA->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionExit();
	}

	if (pRbB->IsListeningCollisionEvent())
	{
		for (IComponent* pComponent : pRbB->m_pGameObject->m_components)
			if (pComponent->GetType() == ComponentType::MonoBehaviour)
				static_cast<MonoBehaviour*>(pComponent)->OnCollisionExit();
	}
}

void Physics::DispatchCollisionEvents()
{
	// 1. 충돌쌍 찾기
	m_currCollisionPairs.clear();

	const int numManifolds = m_upDynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* pManifold =
			m_upDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		if (pManifold->getNumContacts() == 0)
			continue;

		const btCollisionObject* pA = pManifold->getBody0();
		const btCollisionObject* pB = pManifold->getBody1();

		constexpr uintptr_t MSB = std::uintptr_t(1) << (sizeof(std::uintptr_t) * 8 - 1);

		// 캐릭터 컨트롤러의 CollisionObject는 무시!
		// (안그러면 잘못된 캐스팅을 하게됨. 모든 btCollisionObject의 UserPointer는 ze::Rigidbody 컴포넌트로 캐스팅하므로)
		if (HasPtrMSBTag(pA->getUserPointer()) || HasPtrMSBTag(pB->getUserPointer()))
			continue;

		if (pA > pB)
			std::swap(pA, pB);

		m_currCollisionPairs.insert(std::make_pair(pA, pB));
	}

	// 2. Enter & Stay 이벤트 디스패칭
	for (const auto& pair : m_currCollisionPairs)
	{
		const bool isTriggerA = static_cast<Rigidbody*>(pair.first->getUserPointer())->IsTrigger();
		const bool isTriggerB = static_cast<Rigidbody*>(pair.second->getUserPointer())->IsTrigger();
		assert(isTriggerA == static_cast<bool>(pair.first->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
		assert(isTriggerB == static_cast<bool>(pair.second->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));

		if (m_prevCollisionPairs.find(pair) == m_prevCollisionPairs.cend())
		{
			if (isTriggerA || isTriggerB)
				DispatchTriggerEnter(pair);
			else
				DispatchCollisionEnter(pair);
		}
		else
		{
			if (isTriggerA || isTriggerB)
				DispatchTriggerStay(pair);
			else
				DispatchCollisionStay(pair);
		}
	}

	// 3. Exit 이벤트 디스패칭
	for (const auto& pair : m_prevCollisionPairs)
	{
		if (m_currCollisionPairs.find(pair) == m_currCollisionPairs.cend())
		{
			const bool isTriggerA = static_cast<Rigidbody*>(pair.first->getUserPointer())->IsTrigger();
			const bool isTriggerB = static_cast<Rigidbody*>(pair.second->getUserPointer())->IsTrigger();
			assert(isTriggerA == static_cast<bool>(pair.first->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
			assert(isTriggerB == static_cast<bool>(pair.second->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));

			if (isTriggerA || isTriggerB)
				DispatchTriggerExit(pair);
			else
				DispatchCollisionExit(pair);
		}
	}

	std::swap(m_prevCollisionPairs, m_currCollisionPairs);
	m_currCollisionPairs.clear();
}

void Physics::DebugDrawWorld() const
{
	// 렌더링에 필요한 정보들 세팅만
	m_upDynamicsWorld->debugDrawWorld();
}
