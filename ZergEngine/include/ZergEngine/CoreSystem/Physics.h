#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>

namespace ze
{
	class Physics
	{
		friend class Runtime;
		friend class Renderer;
	public:
		static Physics* GetInstance() { return s_pInstance; }
	private:
		Physics();
		~Physics();

		static void CreateInstance();
		static void DestroyInstance();

		bool Init();
		void UnInit();
		
		PhysicsDebugDrawer& GetPhysicsDebugDrawer() { return m_pdd; }

		btDiscreteDynamicsWorld* GetDynamicsWorld() const { return m_pDynamicsWorld; }
	private:
		static Physics* s_pInstance;
		PhysicsDebugDrawer m_pdd;
		XMFLOAT3 m_gravity;
		btDefaultCollisionConfiguration* m_pCollisionConfiguration;
		btCollisionDispatcher* m_pDispatcher;
		btBroadphaseInterface* m_pOverlappingPairCache;
		btSequentialImpulseConstraintSolver* m_pSolver;
		btDiscreteDynamicsWorld* m_pDynamicsWorld;
	};
}
