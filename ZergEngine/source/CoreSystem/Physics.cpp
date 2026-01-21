#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\btBulletCollisionCommon.h>
#include <bullet3\btBulletDynamicsCommon.h>
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

void Physics::DebugDrawWorld()
{
	// 렌더링에 필요한 정보들 세팅만
	m_upDynamicsWorld->debugDrawWorld();
}
