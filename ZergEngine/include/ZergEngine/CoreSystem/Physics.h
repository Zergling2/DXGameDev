#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\EngineConstants.h>
#include <memory>
#include <unordered_set>
#include <utility>
#include <functional>

class btIDebugDraw;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btOverlappingPairCallback;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionObject;
class btConvexShape;

namespace ze
{
	class PhysicsDebugDrawer;
	class BoxCollider;
	class SphereCollider;
	class CapsuleCollider;
	class Rigidbody;

	struct CollisionPairHash
	{
		size_t operator()(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const noexcept
		{
			uintptr_t a = reinterpret_cast<uintptr_t>(pair.first);
			uintptr_t b = reinterpret_cast<uintptr_t>(pair.second);

			constexpr uintptr_t k =
				sizeof(uintptr_t) == 8
				? static_cast<uintptr_t>(0x9e3779b97f4a7c15ull)   // 64bit
				: static_cast<uintptr_t>(0x9e3779b9u);            // 32bit

			a ^= b + k + (a << 6) + (a >> 2);

			return static_cast<size_t>(a);
		}
	};

	struct CollisionPairEqual
	{
		bool operator()(const std::pair<const btCollisionObject*, const btCollisionObject*>& a,
			const std::pair<const btCollisionObject*, const btCollisionObject*>& b) const noexcept
		{
			return a.first == b.first && a.second == b.second;
		}
	};

	struct SweepHit
	{
		const Rigidbody* m_pHitObject;
		XMFLOAT3 m_hitNormalWorld;
		XMFLOAT3 m_hitPointWorld;
		FLOAT m_hitFraction;
	};

	struct RayHit
	{
		bool HasHit() const { return m_pHitObject != nullptr; }
		const Rigidbody* m_pHitObject;
		XMFLOAT3 m_hitNormalWorld;
		XMFLOAT3 m_hitPointWorld;
		FLOAT m_hitFraction;
	};

	class Physics
	{
		friend class Runtime;
		friend class Renderer;
		friend class Rigidbody;
		friend class RigidbodyManager;
	public:
		static Physics* GetInstance() { return s_pInstance; }

		void DrawDebugInfo(bool b);

		void SetGravity(const XMFLOAT3& gravity);
		const XMFLOAT3& GetGravity() const { return m_gravity; }

		std::vector<SweepHit> XM_CALLCONV BoxSweepTest(FXMMATRIX transform, const XMFLOAT3& move, const BoxCollider* pCollider);
		std::vector<SweepHit> XM_CALLCONV SphereSweepTest(FXMMATRIX transform, const XMFLOAT3& move, const SphereCollider* pCollider);
		std::vector<SweepHit> XM_CALLCONV CapsuleSweepTest(FXMMATRIX transform, const XMFLOAT3& move, const CapsuleCollider* pCollider);
		std::vector<SweepHit> XM_CALLCONV ConvexSweepTestImpl(FXMMATRIX transform, const XMFLOAT3& move, const btConvexShape* pBtConvexShape);
		RayHit ClosestRaycastTest(const XMFLOAT3& fromWorld, const XMFLOAT3& toWorld);
		std::vector<RayHit> RaycastTest(const XMFLOAT3& fromWorld, const XMFLOAT3& toWorld);
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
		void DispatchTriggerEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchTriggerStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchTriggerExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchCollisionEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchCollisionStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchCollisionExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair) const;
		void DispatchCollisionEvents();
		void DebugDrawWorld() const;
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

		std::unordered_set<std::pair<const btCollisionObject*, const btCollisionObject*>, CollisionPairHash, CollisionPairEqual> m_prevCollisionPairs;
		std::unordered_set<std::pair<const btCollisionObject*, const btCollisionObject*>, CollisionPairHash, CollisionPairEqual> m_currCollisionPairs;
	};
}
