#pragma once

#include <cstdint>

namespace ze
{
	enum class ComponentType
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
		AUDIO_SOURCE,

		UNKNOWN
	};
}
