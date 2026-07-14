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
class btGhostPairCallback;
class btDiscreteDynamicsWorld;
class btCollisionObject;

namespace ze
{
	class PhysicsDebugDrawer;
	class ICollider;
	class Rigidbody;

	struct SweepHit
	{
		const Rigidbody* m_pHitObject;
		XMFLOAT3 m_hitNormalWorld;
		XMFLOAT3 m_hitPointWorld;
		float m_hitFraction;
	};

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
		friend class CharacterController;
		friend class CharacterControllerManager;
	public:
		static Physics* GetInstance() { return s_pInstance; }

		void DrawDebugInfo(bool b);

		void SetGravity(const XMFLOAT3& gravity);
		const XMFLOAT3& GetGravity() const { return m_gravity; }

		size_t XM_CALLCONV ConvexSweepTestAllNotInclude(std::vector<SweepHit>& results, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept);
		size_t XM_CALLCONV ConvexSweepTestAllNotIncludeExceptTrigger(std::vector<SweepHit>& results, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept);
		bool XM_CALLCONV ConvexSweepTestClosestNotInclude(SweepHit& result, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept);
		bool XM_CALLCONV ConvexSweepTestClosestNotIncludeExceptTrigger(SweepHit& result, FXMMATRIX transform, const XMFLOAT3& move, const ICollider* pCollider, const Rigidbody* pExcept);

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
		static void DispatchTriggerEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		static void DispatchTriggerStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		static void DispatchTriggerExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		static void DispatchCollisionEnter(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		static void DispatchCollisionStay(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		static void DispatchCollisionExit(const std::pair<const btCollisionObject*, const btCollisionObject*>& pair);
		void DispatchCollisionEvents();
		void DebugDrawWorld() const;
	private:
		static Physics* s_pInstance;
		bool m_drawDebugInfo;
		XMFLOAT3 m_gravity;
		std::unique_ptr<PhysicsDebugDrawer> m_upDebugDrawer;
		std::unique_ptr<btDefaultCollisionConfiguration> m_upCollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> m_upDispatcher;
		std::unique_ptr<btBroadphaseInterface> m_upBroadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_upSolver;
		std::unique_ptr<btGhostPairCallback> m_upGhostPairCallback;
		std::unique_ptr<btDiscreteDynamicsWorld> m_upDynamicsWorld;

		std::unordered_set<std::pair<const btCollisionObject*, const btCollisionObject*>, CollisionPairHash, CollisionPairEqual> m_prevCollisionPairs;
		std::unordered_set<std::pair<const btCollisionObject*, const btCollisionObject*>, CollisionPairHash, CollisionPairEqual> m_currCollisionPairs;
	};
}
