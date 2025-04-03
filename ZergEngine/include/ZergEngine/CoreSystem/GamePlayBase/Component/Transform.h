#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class Transform
	{
	public:
		static constexpr bool IsCreatable() { return false; }
		Transform() noexcept
			: m_scale(1.0f, 1.0f, 1.0f)
			, m_rotation(0.0f, 0.0f, 0.0f, 1.0f)
			, m_position(0.0f, 0.0f, 0.0f)
		{
		}
		~Transform() = default;
	public:
		XMFLOAT3A m_scale;
		XMFLOAT4A m_rotation;	// Pitch(x), Yaw(y), Roll(z)
		XMFLOAT3A m_position;
	};
}
