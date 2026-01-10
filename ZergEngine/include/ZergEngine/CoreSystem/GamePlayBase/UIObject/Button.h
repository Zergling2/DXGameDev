#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class Button : public IUIObject
	{
		friend class Renderer;
	public:
		Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Button() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::Button; }

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

		XMVECTOR GetButtonColorVector() const { return m_buttonColor.GetColorVector(); }
		const XMFLOAT4& GetButtonColor() const { return m_buttonColor.GetColor(); }
		void XM_CALLCONV SetButtonColor(FXMVECTOR color) { m_buttonColor.SetColor(color); }
		void SetButtonColor(const XMFLOAT4& color) { m_buttonColor.SetColor(color); }
		void SetButtonColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_buttonColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetButtonColorRGB(FXMVECTOR rgb) { m_buttonColor.SetColorRGB(rgb); }
		void SetButtonColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_buttonColor.SetColorRGB(r, g, b); }
		void SetButtonColorA(FLOAT a) { m_buttonColor.SetColorA(a); }

		std::wstring& GetText() { return m_text.GetText(); }
		const std::wstring& GetText() const { return m_text.GetText(); }
		void SetText(PCWSTR text) { m_text.SetText(text); }
		void SetText(std::wstring text) { m_text.SetText(std::move(text)); }
		TextFormat& GetTextFormat() { return m_text.GetTextFormat(); }
		const TextFormat& GetTextFormat() const { return m_text.GetTextFormat(); }
		DWRITE_TEXT_ALIGNMENT GetTextAlignment() const { return m_text.GetTextAlignment(); }
		void SetTextAlignment(DWRITE_TEXT_ALIGNMENT ta) { m_text.SetTextAlignment(ta); }
		DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() const { return m_text.GetParagraphAlignment(); }
		void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT pa) { m_text.SetParagraphAlignment(pa); }
		IDWriteTextFormat* GetDWriteTextFormat() const { return m_text.GetDWriteTextFormat(); }
		void ApplyTextFormat() { m_text.ApplyTextFormat(); }		// TextFormat을 변경한 후 이 함수를 호출해야 새로운 폰트가 적용됩니다.

		XMVECTOR GetTextColorVector() const { return m_textColor.GetColorVector(); }
		const XMFLOAT4& GetTextColor() const { return m_textColor.GetColor(); }
		void XM_CALLCONV SetTextColor(FXMVECTOR color) { m_textColor.SetColor(color); }
		void SetTextColor(const XMFLOAT4& color) { m_textColor.SetColor(color); }
		void SetTextColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_textColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetTextColorRGB(FXMVECTOR rgb) { m_textColor.SetColorRGB(rgb); }
		void SetTextColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_textColor.SetColorRGB(r, g, b); }
		void SetTextColorA(FLOAT a) { m_textColor.SetColorA(a); }

		void SetHandlerOnClick(std::function<bool()> handler) { m_handlerOnClick = std::move(handler); }

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override;
	private:
		virtual void OnLButtonDown(POINT pt) override;
		virtual void OnLButtonUp(POINT pt) override;
		virtual void OnLButtonClick(POINT pt) override;

		// 렌더링 모양 결정을 위한 상태변수 (작동 로직과는 무관)
		bool IsPressed() const { return m_isPressed; }
	private:
		bool m_isPressed;
		UISize m_size;
		UIColor m_buttonColor;
		UIText m_text;
		UIColor m_textColor;

		std::function<bool()> m_handlerOnClick;
	};
}
