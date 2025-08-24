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

		// ���ʹϾ��� Z(Roll), X(Pitch), Y(Yaw) ��ȯ ������ ���Ϸ� ���� �������� ��ȯ��ŵ�ϴ�.
		// ���� ������ �����Դϴ�.
		static XMVECTOR XM_CALLCONV QuaternionToEuler(FXMVECTOR quaternion) { return Math::QuaternionToEulerNormal(XMQuaternionNormalize(quaternion)); }

		// ����ȭ�� ���ʹϾ��� Z(Roll), X(Pitch), Y(Yaw) ��ȯ ������ ���Ϸ� ���� �������� ��ȯ��ŵ�ϴ�.
		// ���� ������ �����Դϴ�.
		static XMVECTOR XM_CALLCONV QuaternionToEulerNormal(FXMVECTOR quaternion) noexcept;

		// ============================================================================
		// Graphics math
		
		// ���� ���������� ����ü ��� 6�� ��� �� ��ȯ
		static void XM_CALLCONV CalcWorldFrustumFromViewProjMatrix(FXMMATRIX viewProj, Frustum& frustum) noexcept;
		static bool TestRayAabbCollision(const Ray& ray, const Aabb& aabb);

		static bool TestFrustumAabbCollision(const Frustum& frustum, const Aabb& aabb);

		// ����ȭ�� Plane�� �����ؾ� �մϴ�!
		static bool TestAabbBehindPlane(const Aabb& aabb, const Plane& plane);

		static const Aabb XM_CALLCONV TransformAabb(const Aabb& aabb, FXMMATRIX m);
	};
}
