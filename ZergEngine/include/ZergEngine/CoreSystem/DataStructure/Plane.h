#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Plane
	{
	public:
		Plane() noexcept
		{
			XMStoreFloat4A(&m_equation, XMVectorZero());
		}
		Plane(FXMVECTOR equation) noexcept
		{
			XMStoreFloat4A(&m_equation, equation);
		}
	public:
		XMFLOAT4A m_equation;
	};
}
