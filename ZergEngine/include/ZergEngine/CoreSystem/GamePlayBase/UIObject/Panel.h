#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	enum class PanelShape
	{
		Rectangle,
		RoundedRectangle
	};

	class Panel : public IUIObject
	{
	public:
		Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Panel() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::Panel; }

		const XMFLOAT2& GetSize() const { return m_size.GetSize(); }
		FLOAT GetSizeX() const { return m_size.GetSizeX(); }
		FLOAT GetSizeY() const { return m_size.GetSizeY(); }
		FLOAT GetWidth() const { return GetSizeX(); }
		FLOAT GetHeight() const { return GetSizeY(); }
		FLOAT GetHalfSizeX() const { return m_size.GetHalfSizeX(); }
		FLOAT GetHalfSizeY() const { return m_size.GetHalfSizeY(); }
		FLOAT GetHalfWidth() const { return GetHalfSizeX(); }
		FLOAT GetHalfHeight() const { return GetHalfSizeY(); }
		void SetSize(FLOAT width, FLOAT height) { m_size.SetSize(width, height); }
		void SetSize(const XMFLOAT2& size) { m_size.SetSize(size); }
		void XM_CALLCONV SetSize(FXMVECTOR size) { m_size.SetSize(size); }

		XMVECTOR GetColorVector() const { return m_color.GetColorVector(); }
		const XMFLOAT4& GetColor() const { return m_color.GetColor(); }
		void XM_CALLCONV SetColor(FXMVECTOR color) { m_color.SetColor(color); }
		void SetColor(const XMFLOAT4& color) { m_color.SetColor(color); }
		void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_color.SetColor(r, g, b, a); }
		void XM_CALLCONV SetColorRGB(FXMVECTOR rgb) { m_color.SetColorRGB(rgb); }
		void SetColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_color.SetColorRGB(r, g, b); }
		void SetColorA(FLOAT a) { m_color.SetColorA(a); }

		void SetShape(PanelShape shape) { m_shape = shape; }
		PanelShape GetShape() const { return m_shape; }

		void SetRadiusX(float rx) { m_radius.x = rx; }
		void SetRadiusY(float ry) { m_radius.y = ry; }
		float GetRadiusX() const { return m_radius.x; }
		float GetRadiusY() const { return m_radius.y; }

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override;
	private:
		UISize m_size;
		UIColor m_color;
		XMFLOAT2 m_radius;	// m_shape가 PanelShape::Rectangle로 설정된 경우에는 의미를 갖지 않습니다.
		PanelShape m_shape;
	};
}
