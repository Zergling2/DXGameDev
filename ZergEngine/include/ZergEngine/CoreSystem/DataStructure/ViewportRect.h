#pragma once

namespace ze
{
	// All member values are considered normalized.
	struct ViewportRect
	{
	public:
		ViewportRect(float x, float y, float width, float height)
			: m_x(x)
			, m_y(y)
			, m_width(width)
			, m_height(height)
		{
		}
	public:
		float m_x;
		float m_y;
		float m_width;
		float m_height;
	};
}