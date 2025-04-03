#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	enum class COMPONENT_TYPE : uint16_t
	{
		CAMERA,
		MESH_RENDERER,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
		SCRIPT,
		RIGIDBODY,
		BOX_COLLIDER,
		SPHERE_COLLIDER,
		MESH_COLLIDER,
		TERRAIN,

		UNKNOWN
	};
}