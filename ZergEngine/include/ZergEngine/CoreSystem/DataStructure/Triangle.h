#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	// Because of limitations with __vectorcall, we recommend that you not use GXMVECTOR or HXMVECTOR for C++ constructors.
	// Just use FXMVECTOR for the first three XMVECTOR values, then use CXMVECTOR for the rest.

	// Order of winding = m_v[0], m_v[1], m_v[2]
	class Triangle
	{
	public:
		Triangle() = default;
		Triangle(FXMVECTOR v0, FXMVECTOR v1, FXMVECTOR v2)
		{
			XMStoreFloat3A(&m_v[0], v0);
			XMStoreFloat3A(&m_v[1], v1);
			XMStoreFloat3A(&m_v[2], v2);
		}
	public:
		XMFLOAT3A m_v[3];	// [0] -> [1] -> [2]
	};
}
