#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <cassert>

using namespace ze;

Physics* Physics::s_pInstance = nullptr;

Physics::Physics()
	: m_gravity(0.0f, 0.0f, 0.0f)
	, m_pdd()
	, m_pCollisionConfiguration(nullptr)
	, m_pDispatcher(nullptr)
	, m_pOverlappingPairCache(nullptr)
	, m_pSolver(nullptr)
	, m_pDynamicsWorld(nullptr)
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

	// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_pOverlappingPairCache = new btDbvtBroadphase();

	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_pSolver = new btSequentialImpulseConstraintSolver;

	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);

	// 디버그 렌더러 초기화 및 등록
	bool init = m_pdd.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	if (!init)
		return false;

	m_pdd.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_DrawConstraints);
	m_pDynamicsWorld->setDebugDrawer(&m_pdd);

	// 중력 설정
	m_pDynamicsWorld->setGravity(btVector3(m_gravity.x, m_gravity.y, m_gravity.z));


	btCollisionShape* pBoxShape = new btBoxShape(btVector3(1, 1, 1)); // 2x2x2 박스
	btDefaultMotionState* pBoxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 5, 0)));
	btScalar mass = 1.0f;
	btVector3 inertia(0, 0, 0);
	pBoxShape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, pBoxMotionState, pBoxShape, inertia);
	btRigidBody* pBoxRigidBody = new btRigidBody(boxRigidBodyCI);
	m_pDynamicsWorld->addRigidBody(pBoxRigidBody);


	btCollisionShape* pGroundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* pGroundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, pGroundMotionState, pGroundShape);
	btRigidBody* pGroundRigidBody = new btRigidBody(groundRigidBodyCI);
	m_pDynamicsWorld->addRigidBody(pGroundRigidBody);



	return true;
}

void Physics::UnInit()
{
	m_pdd.Release();

	// delete dynamics world
	delete m_pDynamicsWorld;

	// delete solver
	delete m_pSolver;

	// delete broadphase
	delete m_pOverlappingPairCache;

	// delete dispatcher
	delete m_pDispatcher;

	delete m_pCollisionConfiguration;
}
