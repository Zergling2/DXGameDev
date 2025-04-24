#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	// Because of limitations with __vectorcall, we recommend that you not use GXMVECTOR or HXMVECTOR for C++ constructors.
	// Just use FXMVECTOR for the first three XMVECTOR values, then use CXMVECTOR for the rest.

	class Aabb
	{
	public:
		Aabb(FXMVECTOR lowerBound, FXMVECTOR upperBound)
		{
			XMStoreFloat3A(&m_lowerBound, lowerBound);
			XMStoreFloat3A(&m_upperBound, upperBound);
		}
		~Aabb() = default;
	public:
		XMFLOAT3A m_lowerBound;
		XMFLOAT3A m_upperBound;
	};
}
