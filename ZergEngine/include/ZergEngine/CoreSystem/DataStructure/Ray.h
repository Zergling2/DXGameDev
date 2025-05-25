#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Ray
	{
	public:
		Ray(FXMVECTOR origin, FXMVECTOR direction)
		{
			XMStoreFloat3A(&m_origin, origin);
			XMStoreFloat3A(&m_direction, direction);
		}
	public:
		// The origin point of the ray.
		XMFLOAT3A m_origin;

		// The direction of the ray.
		XMFLOAT3A m_direction;
	};
}
