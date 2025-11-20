#pragma once

namespace ze
{
	enum class ComponentType
	{
		// 3D GameObject Components
		Camera,
		MeshRenderer,
		SkinnedMeshRenderer,
		DirectionalLight,
		PointLight,
		SpotLight,
		MonoBehaviour,
		Rigidbody,
		BoxCollider,
		SphereCollider,
		MeshCollider,
		Terrain,
		AudioSource,

		UNKNOWN
	};
}
