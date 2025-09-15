#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

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
