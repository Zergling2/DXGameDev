#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\EngineConstants.h>
#include <memory>

class btIDebugDraw;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace ze
{
	class PhysicsDebugDrawer;

	class Physics
	{
		friend class Runtime;
		friend class Renderer;
		friend class Rigidbody;
		friend class RigidbodyManager;
		friend class CollisionTrigger;
		friend class CollisionTriggerManager;
	public:
		static Physics* GetInstance() { return s_pInstance; }

		void DrawDebugInfo(bool b);

		void SetGravity(const XMFLOAT3& gravity);
		XMFLOAT3 GetGravity() const { return m_gravity; }
	private:
		Physics();
		~Physics();

		static void CreateInstance();
		static void DestroyInstance();

		bool Init();
		void Shutdown();
		
		PhysicsDebugDrawer* GetPhysicsDebugDrawer() const { return m_upDebugDrawer.get(); }
		void UpdateDebugDrawerResources(ID3D11DeviceContext* pDeviceContext);
		void ClearDebugDrawerCache();

		btDiscreteDynamicsWorld* GetDynamicsWorld() const { return m_upDynamicsWorld.get(); }
		void StepSimulation(float timeStep, int maxSubSteps = 1, float fixedTimeStep = FIXED_DELTA_TIME);
		void DebugDrawWorld();
	private:
		static Physics* s_pInstance;
		bool m_drawDebugInfo;
		XMFLOAT3 m_gravity;
		std::unique_ptr<PhysicsDebugDrawer> m_upDebugDrawer;
		std::unique_ptr<btDefaultCollisionConfiguration> m_upCollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> m_upDispatcher;
		std::unique_ptr<btBroadphaseInterface> m_upOverlappingPairCache;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_upSolver;
		std::unique_ptr<btDiscreteDynamicsWorld> m_upDynamicsWorld;
	};
}
