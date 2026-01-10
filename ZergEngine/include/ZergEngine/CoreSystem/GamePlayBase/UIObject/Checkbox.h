#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include "RadioButton.h"

namespace ze
{
	class Checkbox : public IUIObject
	{
		friend class Renderer;
	public:
		Checkbox(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Checkbox() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::Checkbox; }

		bool GetCheck() const { return m_isChecked; }
		void SetCheck(bool b);

		bool IsLeftText() const { return m_leftText; }
		// true를 전달하면 텍스트박스가 체크박스의 왼쪽에 위치합니다.
		// false를 전달하면 텍스트박스가 체크박스의 오른쪽에 위치합니다.
		void SetLeftText(bool b = true);

		const XMFLOAT2& GetCheckboxSize() const { return m_checkboxSize.GetSize(); }
		FLOAT GetCheckboxSizeX() const { return m_checkboxSize.GetSizeX(); }
		FLOAT GetCheckboxSizeY() const { return m_checkboxSize.GetSizeY(); }
		FLOAT GetCheckboxWidth() const { return GetCheckboxSizeX(); }
		FLOAT GetCheckboxHeight() const { return GetCheckboxSizeY(); }
		FLOAT GetCheckboxHalfSizeX() const { return m_checkboxSize.GetHalfSizeX(); }
		FLOAT GetCheckboxHalfSizeY() const { return m_checkboxSize.GetHalfSizeY(); }
		FLOAT GetCheckboxHalfWidth() const { return GetCheckboxHalfSizeX(); }
		FLOAT GetCheckboxHalfHeight() const { return GetCheckboxHalfSizeY(); }
		void SetCheckboxSize(FLOAT size) { m_checkboxSize.SetSize(size, size); }

		XMVECTOR GetBoxColorVector() const { return m_boxColor.GetColorVector(); }
		const XMFLOAT4& GetBoxColor() const { return m_boxColor.GetColor(); }
		void XM_CALLCONV SetBoxColor(FXMVECTOR color) { m_boxColor.SetColor(color); }
		void SetBoxColor(const XMFLOAT4& color) { m_boxColor.SetColor(color); }
		void SetBoxColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_boxColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetBoxColorRGB(FXMVECTOR rgb) { m_boxColor.SetColorRGB(rgb); }
		void SetBoxColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_boxColor.SetColorRGB(r, g, b); }
		void SetBoxColorA(FLOAT a) { m_boxColor.SetColorA(a); }

		XMVECTOR GetCheckColorVector() const { return m_checkColor.GetColorVector(); }
		const XMFLOAT4& GetCheckColor() const { return m_checkColor.GetColor(); }
		void XM_CALLCONV SetCheckColor(FXMVECTOR color) { m_checkColor.SetColor(color); }
		void SetCheckColor(const XMFLOAT4& color) { m_checkColor.SetColor(color); }
		void SetCheckColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_checkColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetCheckColorRGB(FXMVECTOR rgb) { m_checkColor.SetColorRGB(rgb); }
		void SetCheckColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_checkColor.SetColorRGB(r, g, b); }
		void SetCheckColorA(FLOAT a) { m_checkColor.SetColorA(a); }

		const XMFLOAT2& GetTextboxSize() const { return m_textboxSize.GetSize(); }
		FLOAT GetTextboxSizeX() const { return m_textboxSize.GetSizeX(); }
		FLOAT GetTextboxSizeY() const { return m_textboxSize.GetSizeY(); }
		FLOAT GetTextboxWidth() const { return GetTextboxSizeX(); }
		FLOAT GetTextboxHeight() const { return GetTextboxSizeY(); }
		FLOAT GetTextboxHalfSizeX() const { return m_textboxSize.GetHalfSizeX(); }
		FLOAT GetTextboxHalfSizeY() const { return m_textboxSize.GetHalfSizeY(); }
		FLOAT GetTextboxHalfWidth() const { return GetTextboxHalfSizeX(); }
		FLOAT GetTextboxHalfHeight() const { return GetTextboxHalfSizeY(); }
		void SetTextboxSize(FLOAT size) { m_textboxSize.SetSize(size, size); }

		XMVECTOR GetTextColorVector() const { return m_textColor.GetColorVector(); }
		const XMFLOAT4& GetTextColor() const { return m_textColor.GetColor(); }
		void XM_CALLCONV SetTextColor(FXMVECTOR color) { m_textColor.SetColor(color); }
		void SetTextColor(const XMFLOAT4& color) { m_textColor.SetColor(color); }
		void SetTextColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_textColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetTextColorRGB(FXMVECTOR rgb) { m_textColor.SetColorRGB(rgb); }
		void SetTextColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_textColor.SetColorRGB(r, g, b); }
		void SetTextColorA(FLOAT a) { m_textColor.SetColorA(a); }

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
		void ApplyTextFormat() { m_text.ApplyTextFormat(); }	// TextFormat을 변경한 후 이 함수를 호출해야 새로운 폰트가 적용됩니다.

		void SetHandlerOnClick(std::function<bool()> handler) { m_handlerOnClick = std::move(handler); }

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override;
	private:
		virtual void OnLButtonClick(POINT pt) override;
	private:
		bool m_isChecked;	// 렌더링 변수
		bool m_leftText;	// 렌더링 변수
		UISize m_checkboxSize;
		UIColor m_boxColor;
		UIColor m_checkColor;
		UISize m_textboxSize;
		UIColor m_textColor;
		UIText m_text;

		std::function<bool()> m_handlerOnClick;
	};
}
