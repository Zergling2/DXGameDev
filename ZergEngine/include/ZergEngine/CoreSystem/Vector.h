#pragma once

namespace ze
{
	struct Vector2
	{
		Vector2(float x, float y)
			: m_x(x)
			, m_y(y)
		{
		}
		float m_x;
		float m_y;
	};

	struct Vector3
	{
		Vector3(float x, float y, float z)
			: m_x(x)
			, m_y(y)
			, m_z(z)
		{
		}
		float m_x;
		float m_y;
		float m_z;
	};

	struct Vector4
	{
		Vector4(float x, float y, float z, float w)
			: m_x(x)
			, m_y(y)
			, m_z(z)
			, m_w(w)
		{
		}
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};
}
