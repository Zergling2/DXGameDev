#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	// Because of limitations with __vectorcall, we recommend that you not use GXMVECTOR or HXMVECTOR for C++ constructors.
	// Just use FXMVECTOR for the first three XMVECTOR values, then use CXMVECTOR for the rest.

	class Aabb
	{
	public:
		Aabb() noexcept
		{
			XMStoreFloat3A(&m_center, XMVectorZero());
			XMStoreFloat3A(&m_extent, XMVectorZero());
		}
		Aabb(FXMVECTOR center, FXMVECTOR extent) noexcept
		{
			XMStoreFloat3A(&m_center, center);
			XMStoreFloat3A(&m_extent, extent);
		}
		Aabb(const XMFLOAT3A& center, const XMFLOAT3A& extent) noexcept
			: m_center(center)
			, m_extent(extent)
		{
		}
		~Aabb() = default;
	public:
		XMFLOAT3A m_center;
		XMFLOAT3A m_extent;	// Half size (Always positive)
	};
}
