#pragma once

#include <ZergEngine\CoreSystem\DataStructure\Plane.h>

namespace ze
{
	class Frustum
	{
	public:
		Frustum() = default;
	public:
		Plane m_plane[6];
	};
}
