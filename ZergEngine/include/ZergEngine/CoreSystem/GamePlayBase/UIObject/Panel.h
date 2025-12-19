#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	enum class PanelShape : uint8_t
	{
		Rectangle,
		RoundedRectangle
	};

	class Panel : public ISizeColorUIObject
	{
	public:
		Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Panel() = default;

		virtual UIOBJECT_TYPE GetType() const override { return UIOBJECT_TYPE::PANEL; }

		void SetShape(PanelShape shape) { m_shape = shape; }
		PanelShape GetShape() const { return m_shape; }

		void SetRadiusX(float rx) { m_radius.x = rx; }
		void SetRadiusY(float ry) { m_radius.y = ry; }
		float GetRadiusX() const { return m_radius.x; }
		float GetRadiusY() const { return m_radius.y; }
	private:
		PanelShape m_shape;
		// The x, y radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
		// 패널의 모양이 PANEL_SHAPE::RECTANGLE로 설정된 경우에는 의미를 갖지 않습니다.
		XMFLOAT2 m_radius;
	};
}
