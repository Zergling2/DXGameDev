#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Frustum;
	class Ray;
	class Triangle;
	using Aabb = DirectX::BoundingBox;

	class Math
	{
	public:
		class Vector3
		{
		public:
			// 0.0f, 1.0f, 0.0f, 0.0f
			static XMVECTOR Up() { return g_XMIdentityR1; }

			// 1.0f, 0.0f, 0.0f, 0.0f
			static XMVECTOR Right() { return g_XMIdentityR0; }

			// 0.0f, 0.0f, 1.0f, 0.0f
			static XMVECTOR Forward() { return g_XMIdentityR2; }

			// 1.0f, 1.0f, 1.0f, 1.0f
			static XMVECTOR One() { return g_XMOne; }

			// 1.0f, 1.0f, 1.0f, 0.0f
			static XMVECTOR One3() { return g_XMOne3; }

			// 0.5f, 0.5f, 0.5f, 0.5f
			static XMVECTOR OneHalf() { return g_XMOneHalf; }

			// 0.0f, 0.0f, 0.0f, 0.0f
			static XMVECTOR Zero() { return g_XMZero; }
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

		static bool TestRayTriangleCollision(const Ray& ray, const Triangle& tri);
	};
}
