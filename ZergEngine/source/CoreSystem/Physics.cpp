#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\RigidbodyInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <bullet3\btBulletCollisionCommon.h>
#include <bullet3\btBulletDynamicsCommon.h>
#include <bullet3\BulletCollision\CollisionDispatch\btGhostObject.h>
#include <cassert>

using namespace ze;

Physics* Physics::s_pInstance = nullptr;

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

Physics::Physics()
	: m_drawDebugInfo(true)
	, m_gravity(0.0f, -9.81f, 0.0f)
	, m_upDebugDrawer(nullptr)
	, m_upCollisionConfiguration(nullptr)
	, m_upDispatcher(nullptr)
	, m_upOverlappingPairCache(nullptr)
	, m_upSolver(nullptr)
	, m_upDynamicsWorld(nullptr)
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
	m_upOverlappingPairCache = std::make_unique<btDbvtBroadphase>();


	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_upSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_upDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_upDispatcher.get(),
		m_upOverlappingPairCache.get(),
		m_upSolver.get(),
		m_upCollisionConfiguration.get()
	);

	// 디버그 렌더러 초기화 및 등록
	bool init = m_upDebugDrawer->Init(GraphicDevice::GetInstance()->GetDevice());
	if (!init)
		return false;

	m_upDebugDrawer->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe |
		btIDebugDraw::DBG_DrawAabb |
		// btIDebugDraw::DBG_DrawContactPoints |
		btIDebugDraw::DBG_DrawConstraints |
		btIDebugDraw::DBG_DrawFrames
	);
	m_upDynamicsWorld->setDebugDrawer(m_upDebugDrawer.get());

	// 중력 설정
	m_upDynamicsWorld->setGravity(btVector3(m_gravity.x, m_gravity.y, m_gravity.z));

	return true;
}

void Physics::Shutdown()
{
	m_upDebugDrawer->Release();
	m_upDebugDrawer.reset();

	m_upDynamicsWorld.reset();

	m_upSolver.reset();

	m_upOverlappingPairCache.reset();

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

void Physics::DispatchTriggerEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

void Physics::DispatchTriggerStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

void Physics::DispatchTriggerExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

void Physics::DispatchCollisionEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

void Physics::DispatchCollisionStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

void Physics::DispatchCollisionExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const
{
	IRigidbody* pRbA = static_cast<IRigidbody*>(pair.first->getUserPointer());
	IRigidbody* pRbB = static_cast<IRigidbody*>(pair.second->getUserPointer());

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

	int numManifolds = m_upDynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* manifold =
			m_upDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		if (manifold->getNumContacts() == 0)
			continue;

		const btCollisionObject* pA = manifold->getBody0();
		const btCollisionObject* pB = manifold->getBody1();

		if (pA > pB)
			std::swap(pA, pB);

		m_currCollisionPairs.insert(std::make_pair(pA, pB));
	}

	// 2. Enter & Stay 이벤트 디스패칭
	for (const auto& pair : m_currCollisionPairs)
	{
		const bool isTriggerA = static_cast<IRigidbody*>(pair.first->getUserPointer())->IsTrigger();
		const bool isTriggerB = static_cast<IRigidbody*>(pair.second->getUserPointer())->IsTrigger();
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
			const bool isTriggerA = static_cast<IRigidbody*>(pair.first->getUserPointer())->IsTrigger();
			const bool isTriggerB = static_cast<IRigidbody*>(pair.second->getUserPointer())->IsTrigger();
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
