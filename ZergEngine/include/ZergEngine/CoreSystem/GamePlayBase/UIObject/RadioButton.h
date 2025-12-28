#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class RadioButtonGroup;

	class RadioButton : public IUIObject
	{
		friend class RadioButtonGroup;
	public:
		RadioButton(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~RadioButton() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::RadioButton; }

		void Join(RadioButton* pOther);

		bool GetCheck() const { return m_isChecked; }
		// RadioButton은 항상 체크 동작만 가능하며 이미 버튼이 체크되어 있던 경우 아무런 동작도 하지 않습니다.
		void SetCheck();

		bool IsLeftText() const { return m_leftText; }
		// true를 전달하면 텍스트박스가 체크박스의 왼쪽에 위치합니다.
		// false를 전달하면 텍스트박스가 체크박스의 오른쪽에 위치합니다.
		void SetLeftText(bool b = true);

		FLOAT GetRadius() const { return m_radius; }
		void SetRadius(FLOAT r) { m_radius = r; }

		XMVECTOR GetButtonColorVector() const { return m_buttonColor.GetColorVector(); }
		const XMFLOAT4& GetButtonColor() const { return m_buttonColor.GetColor(); }
		void XM_CALLCONV SetButtonColor(FXMVECTOR color) { m_buttonColor.SetColor(color); }
		void SetButtonColor(const XMFLOAT4& color) { m_buttonColor.SetColor(color); }
		void SetButtonColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_buttonColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetButtonColorRGB(FXMVECTOR rgb) { m_buttonColor.SetColorRGB(rgb); }
		void SetButtonColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_buttonColor.SetColorRGB(r, g, b); }
		void SetButtonColorA(FLOAT a) { m_buttonColor.SetColorA(a); }

		XMVECTOR GetDotColorVector() const { return m_dotColor.GetColorVector(); }
		const XMFLOAT4& GetDotColor() const { return m_dotColor.GetColor(); }
		void XM_CALLCONV SetDotColor(FXMVECTOR color) { m_dotColor.SetColor(color); }
		void SetDotColor(const XMFLOAT4& color) { m_dotColor.SetColor(color); }
		void SetDotColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_dotColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetDotColorRGB(FXMVECTOR rgb) { m_dotColor.SetColorRGB(rgb); }
		void SetDotColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_dotColor.SetColorRGB(r, g, b); }
		void SetDotColorA(FLOAT a) { m_dotColor.SetColorA(a); }

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
		IDWriteTextFormat* GetDWriteTextFormatComInterface() const { return m_text.GetDWriteTextFormatComInterface(); }
		void ApplyTextFormat() { m_text.ApplyTextFormat(); }	// TextFormat을 변경한 후 이 함수를 호출해야 새로운 폰트가 적용됩니다.

		std::shared_ptr<RadioButtonGroup> GetGroup() { return m_spGroup; }
		std::shared_ptr<RadioButtonGroup> GetGroup() const { return m_spGroup; }	

		virtual bool HitTest(POINT pt) const override;

		void SetHandlerOnClick(std::function<bool()> handler) { m_handlerOnClick = std::move(handler); }
	private:
		virtual void OnLButtonClick(POINT pt) override;
		virtual void OnDestroy() override;

		void LeaveGroup();
	private:
		bool m_isChecked;	// 렌더링 변수
		bool m_leftText;	// 렌더링 변수
		FLOAT m_radius;		// Button radius
		UIColor m_buttonColor;
		UIColor m_dotColor;
		UISize m_textboxSize;
		UIColor m_textColor;
		UIText m_text;

		int32_t m_groupIndex;
		std::shared_ptr<RadioButtonGroup> m_spGroup;

		std::function<bool()> m_handlerOnClick;
	};

	class RadioButtonGroup
	{
	public:
		size_t GetGroupSize() const { return m_buttons.size(); }
		RadioButton* GetButton(size_t index) const;
		RadioButton* const* GetButtons() const { return m_buttons.data(); }
	public:
		std::vector<RadioButton*> m_buttons;
	};
}
