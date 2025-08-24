#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	class Button : public ISizeColorUIObject
	{
		friend class Renderer;
	public:
		Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Button() = default;

		XMVECTOR GetTextColorVector() const { return XMLoadFloat4(&m_textColor); }
		const XMFLOAT4& GetTextColor() const { return m_textColor; }
		void XM_CALLCONV SetTextColor(FXMVECTOR color) { XMStoreFloat4(&m_textColor, color); }
		void SetTextColor(const XMFLOAT4A& color) { m_textColor = color; }
		void SetTextColor(const XMFLOAT4& color) { m_textColor = color; }

		void SetText(PCWSTR text);
		PCWSTR GetText() const { return m_text; }
		uint16_t GetTextLength() const { return m_textLength; }
	private:
		virtual void OnDetachUIInteraction() override;	// 시각적으로 눌린 상태를 해제한다.

		virtual void OnLButtonDown() override;
		virtual void OnLButtonUp() override;

		bool IsPressed() const { return m_pressed; }
		void SetPressed(bool pressed) { m_pressed = pressed; }
	private:
		bool m_pressed;
		// uint8_t m_textSize;
		// uint8_t m_textThickness;
		XMFLOAT4 m_textColor;
		WCHAR m_text[20];
		uint16_t m_textLength;
	};
}
