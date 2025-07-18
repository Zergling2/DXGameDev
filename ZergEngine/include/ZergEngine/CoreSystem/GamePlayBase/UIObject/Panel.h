#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	enum class PANEL_SHAPE
	{
		RECTANGLE,
		ROUNDED_RECTANGLE
	};

	class Panel : public ISizeColorUIObject
	{
	public:
		Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Panel() = default;

		void SetShape(PANEL_SHAPE shape) { m_shape = shape; }
		PANEL_SHAPE GetShape() const { return m_shape; }

		void SetRadiusX(float rx) { m_radius.x = rx; }
		void SetRadiusY(float ry) { m_radius.y = ry; }
		float GetRadiusX() const { return m_radius.x; }
		float GetRadiusY() const { return m_radius.y; }
	private:
		PANEL_SHAPE m_shape;
		// The x, y radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
		// 패널의 모양이 PANEL_SHAPE::RECTANGLE로 설정된 경우에는 의미를 갖지 않습니다.
		XMFLOAT2 m_radius;
	};
}
