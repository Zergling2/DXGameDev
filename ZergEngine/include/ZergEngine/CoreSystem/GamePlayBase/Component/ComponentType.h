#pragma once

#include <cstdint>

namespace ze
{
	enum class COMPONENT_TYPE : uint16_t
	{
		// 3D GameObject Components
		CAMERA,
		MESH_RENDERER,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
		MONOBEHAVIOUR,
		RIGIDBODY,
		BOX_COLLIDER,
		SPHERE_COLLIDER,
		MESH_COLLIDER,
		TERRAIN,

		UNKNOWN
	};
}
