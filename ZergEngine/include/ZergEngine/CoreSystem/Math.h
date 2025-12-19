#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <cmath>

/*
	XMVECTOR, XMMATRIX 전달 규칙
	https://learn.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-internals
	FXMVECTOR _1, FXMVECTOR _2, FXMVECTOR _3, GXMVECTOR _4, HXMVECTOR _5, HXMVECTOR _6, CXMVECTOR _7, CXMVECTOR _8, ...

	Use the FXMVECTOR alias to pass up to the first three instances of XMVECTOR used as arguments to a function.
	Use the GXMVECTOR alias to pass the 4th instance of an XMVECTOR used as an argument to a function.
	Use the HXMVECTOR alias to pass the 5th and 6th instances of an XMVECTOR used as an argument to a function. For info about additional considerations, see the __vectorcall documentation.
	Use the CXMVECTOR alias to pass any further instances of XMVECTOR used as arguments.

	FXMMATRIX = __m128 x 4
	Use the FXMMATRIX alias to pass the first XMMATRIX as an argument to the function.
	This assumes you don't have more than two FXMVECTOR arguments or more than two float, double,
	or FXMVECTOR arguments to the 'right' of the matrix. For info about additional considerations,
	see the __vectorcall documentation.
	Use the CXMMATRIX alias otherwise.
	*/

	/*
	In addition to the type aliases, you must also use the XM_CALLCONV annotation to make sure
	the function uses the appropriate calling convention (__fastcall versus __vectorcall) based
	on your compiler and architecture. Because of limitations with __vectorcall, we recommend
	that you not use XM_CALLCONV for C++ constructors.
	*/

namespace ze
{
	using Frustum = DirectX::BoundingFrustum;
	using Aabb = DirectX::BoundingBox;
	using Obb = DirectX::BoundingOrientedBox;
	class Ray;
	class Triangle;

	struct SphericalCoord
	{
		float r;		// 원점과의 거리
		float theta;	// Y축 기준 극각 [-PI / 2, +PI / 2]
		float phi;		// XZ 평면에서의 방위각 [0, 2PI)
	};

	class Vector3
	{
	public:
		// 1.0f, 0.0f, 0.0f, 0.0f
		static XMVECTOR Right() { return g_XMIdentityR0; }

		// 0.0f, 1.0f, 0.0f, 0.0f
		static XMVECTOR Up() { return g_XMIdentityR1; }

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

	class Math
	{
	public:
		// 1.0f, 0.0f, 0.0f, 0.0f
		static XMVECTOR IdentityR0() { return g_XMIdentityR0; }

		// 0.0f, 1.0f, 0.0f, 0.0f
		static XMVECTOR IdentityR1() { return g_XMIdentityR1; }

		// 0.0f, 0.0f, 1.0f, 0.0f
		static XMVECTOR IdentityR2() { return g_XMIdentityR2; }

		// 0.0f, 0.0f, 0.0f, 1.0f
		static XMVECTOR IdentityR3() { return g_XMIdentityR3; }
		
		static int WrapInt(int val, int min, int max);
		static float WrapFloat(float val, float max);
		static float WrapFloat(float val, float min, float max);
		static double WrapDouble(double val, double max);
		static double WrapDouble(double val, double min, double max);

		template<typename T>
		static T Clamp(T val, T min, T max)
		{
			if (val < min)
				return min;
			else if (val > max)
				return max;
			else
				return val;
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

		static bool IsNear(float a, float b, float tolerance = 0.0001f)
		{
			return std::abs(a - b) < tolerance;
		}

		static bool XM_CALLCONV IsVector2LengthNear(FXMVECTOR v, float length, float tolerance = 0.00001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector2Length(v)), length, tolerance);
		}

		static bool XM_CALLCONV IsVector3LengthNear(FXMVECTOR v, float length, float tolerance = 0.00001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector3Length(v)), length, tolerance);
		}

		static bool XM_CALLCONV IsVector4LengthNear(FXMVECTOR v, float length, float tolerance = 0.00001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector4Length(v)), length, tolerance);
		}

		static bool XM_CALLCONV IsVector2LengthEstNear(FXMVECTOR v, float length, float tolerance = 0.0001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector2LengthEst(v)), length, tolerance);
		}

		static bool XM_CALLCONV IsVector3LengthEstNear(FXMVECTOR v, float length, float tolerance = 0.0001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector3LengthEst(v)), length, tolerance);
		}

		static bool XM_CALLCONV IsVector4LengthEstNear(FXMVECTOR v, float length, float tolerance = 0.0001f)
		{
			return Math::IsNear(XMVectorGetX(XMVector4LengthEst(v)), length, tolerance);
		}

		static SphericalCoord XM_CALLCONV ToSphericalCoord(FXMVECTOR v);

		// 쿼터니언을 Z(Roll), X(Pitch), Y(Yaw) 변환 순서의 오일러 각도 모음으로 변환시킵니다.
		// 각도 단위는 라디안입니다.
		static XMVECTOR XM_CALLCONV QuaternionToEuler(FXMVECTOR quaternion) { return Math::QuaternionToEulerNormal(XMQuaternionNormalize(quaternion)); }

		// 정규화된 쿼터니언을 Z(Roll), X(Pitch), Y(Yaw) 변환 순서의 오일러 각도 모음으로 변환시킵니다.
		// 각도 단위는 라디안입니다.
		static XMVECTOR XM_CALLCONV QuaternionToEulerNormal(FXMVECTOR quaternion) noexcept;

		// ============================================================================
		// Graphics math
		static bool TestRayAabbCollision(const Ray& ray, const Aabb& aabb);
		static bool TestFrustumAabbCollision(const Frustum& frustum, const Aabb& aabb);
		static bool TestFrustumObbCollision(const Frustum& frustum, const Obb& obb);
		static bool TestRayTriangleCollision(const Ray& ray, const Triangle& tri);

		static void XM_CALLCONV ComputeFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4* pPlanes);
		static void XM_CALLCONV ComputeFrustumPlanesFromViewProjMatrix(FXMMATRIX viewProj, XMFLOAT4A* pPlanes);

		static XMMATRIX XM_CALLCONV ComputeBillboardSphericalMatrix(FXMVECTOR billboardPosW, FXMVECTOR billboardScaleW, FXMVECTOR cameraPosW, GXMVECTOR cameraUpW);
		static XMMATRIX XM_CALLCONV ComputeBillboardCylindricalYMatrix(FXMVECTOR billboardPosW, FXMVECTOR billboardScaleW, FXMVECTOR cameraPosW);
		static XMMATRIX XM_CALLCONV ComputeBillboardScreenAlignedMatrix(FXMMATRIX billboardRotationW, HXMVECTOR billboardPosW, HXMVECTOR billboardScaleW);
	};

	class Quaternion
	{
	public:
		static XMVECTOR Identity() { return g_XMIdentityR3; }

		// from 벡터를 to 벡터와 일치시키는 쿼터니언을 반환합니다.
		static XMVECTOR XM_CALLCONV FromToRotation(FXMVECTOR from, FXMVECTOR to);

		// from 벡터를 to 벡터와 일치시키는 쿼터니언을 반환합니다.
		// from 벡터와 to 벡터는 정규화 되어있는것으로 간주됩니다.
		// from 벡터와 to 벡터가 정규화되어있지 않은 경우 올바른 계산 결과가 보장되지 않습니다.
		static XMVECTOR XM_CALLCONV FromToRotationNorm(FXMVECTOR from, FXMVECTOR to);
	};
}
