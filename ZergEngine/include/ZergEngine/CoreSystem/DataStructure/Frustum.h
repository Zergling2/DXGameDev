#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Frustum
	{
	public:
		Frustum() = default;
	public:
		XMFLOAT4A m_plane[6];
	};
}
