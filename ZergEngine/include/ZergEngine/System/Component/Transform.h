#pragma once

#include <ZergEngine\System\Component\IComponent.h>

namespace zergengine
{
	class Transform
	{
		friend class Graphics;
		friend class Camera;
	public:
		static inline bool IsCreatable() { return false; }
		Transform()
			: m_scale(1.0f, 1.0f, 1.0f)
			, m_rotation(0.0f, 0.0f, 0.0f, 1.0f)
			, m_position(0.0f, 0.0f, 0.0f)
		{
		}
	public:
		XMFLOAT3A m_scale;
		XMFLOAT4A m_rotation;	// Pitch(x), Yaw(y), Roll(z)
		XMFLOAT3A m_position;
	};
}
