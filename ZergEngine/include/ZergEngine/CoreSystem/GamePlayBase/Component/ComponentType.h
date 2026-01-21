#pragma once

namespace ze
{
	enum class ComponentType
	{
		// 3D GameObject Components
		Camera,
		MeshRenderer,
		SkinnedMeshRenderer,
		Billboard,
		DirectionalLight,
		PointLight,
		SpotLight,
		MonoBehaviour,
		Rigidbody,
		StaticRigidbody,
		CollisionTrigger,
		Terrain,
		AudioSource,

		UNKNOWN
	};
}
