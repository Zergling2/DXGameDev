#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class Ray
	{
	public:
		Ray() noexcept
		{
			XMStoreFloat3A(&m_origin, XMVectorZero());
			XMStoreFloat3A(&m_direction, XMVectorZero());
		}
		Ray(FXMVECTOR origin, FXMVECTOR direction) noexcept
		{
			XMStoreFloat3A(&m_origin, origin);
			XMStoreFloat3A(&m_direction, direction);
		}
		Ray(const Ray&) = default;
		Ray& operator=(const Ray&) = default;

		void XM_CALLCONV Transform(FXMMATRIX matrix) noexcept;
	public:
		// The origin point of the ray.
		XMFLOAT3A m_origin;

		// The direction of the ray.
		XMFLOAT3A m_direction;
	};
}
