#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class GameObject;

	enum ANCHOR : uint32_t
	{
		ANCHOR_LEFT		= 1 << 0,
		ANCHOR_CENTER	= 1 << 1,
		ANCHOR_RIGHT	= 1 << 2,
		ANCHOR_TOP		= 1 << 3,
		ANCHOR_VCENTER	= 1 << 4,
		ANCHOR_BOTTOM	= 1 << 5
	};

	class RectTransform
	{
	public:
		RectTransform() noexcept
			: m_anchor(static_cast<ANCHOR>(ANCHOR_CENTER | ANCHOR_VCENTER))
			, m_x(0.0f)
			, m_y(0.0f)
		{
		}
		RectTransform(const RectTransform& other) = default;
		RectTransform(RectTransform&& other) = default;
		~RectTransform() = default;
	public:
		ANCHOR m_anchor;
		float m_x;
		float m_y;
	};
}
