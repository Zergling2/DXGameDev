#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Frustum;
	class Plane;
	class Ray;
	class Aabb;

	class Math
	{
	public:
		class Vector3
		{
		public:
			static const XMVECTORF32 UP;
			static const XMVECTORF32 RIGHT;
			static const XMVECTORF32 FORWARD;
		};

		template<typename T>
		static inline void Clamp(T& val, T min, T max)
		{
			if (val < min)
				val = min;
			else if (val > max)
				val = max;
		}

		template <typename T>
		static constexpr bool IsPowerOf2(T n)
		{
			return n > 0 && (n & (n - 1)) == 0;
		}

		template <typename T>
		static constexpr T PowerOf2(T n)
		{
			return static_cast<T>(1) << n;
		}

		// 쿼터니언을 Z(Roll), X(Pitch), Y(Yaw) 변환 순서의 오일러 각도 모음으로 변환시킵니다.
		// 각도 단위는 라디안입니다.
		static XMVECTOR XM_CALLCONV QuaternionToEuler(FXMVECTOR quaternion) { return Math::QuaternionToEulerNormal(XMQuaternionNormalize(quaternion)); }

		// 정규화된 쿼터니언을 Z(Roll), X(Pitch), Y(Yaw) 변환 순서의 오일러 각도 모음으로 변환시킵니다.
		// 각도 단위는 라디안입니다.
		static XMVECTOR XM_CALLCONV QuaternionToEulerNormal(FXMVECTOR quaternion) noexcept;

		// ============================================================================
		// Graphics math
		
		// 월드 공간에서의 절두체 평면 6개 계산 및 반환
		static void XM_CALLCONV CalcWorldFrustumFromViewProjMatrix(FXMMATRIX viewProj, Frustum& frustum) noexcept;
		static bool TestRayAabbCollision(const Ray& ray, const Aabb& aabb);

		static bool TestFrustumAabbCollision(const Frustum& frustum, const Aabb& aabb);

		// 정규화된 Plane을 전달해야 합니다!
		static bool TestAabbBehindPlane(const Aabb& aabb, const Plane& plane);

		static const Aabb XM_CALLCONV TransformAabb(const Aabb& aabb, FXMMATRIX m);
	};
}
