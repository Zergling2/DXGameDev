#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	class Button : public ISizeColorUIObject
	{
		friend class RendererImpl;
	public:
		Button(UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Button() = default;

		XMVECTOR GetTextColorVector() const { return XMLoadFloat4(&m_textColor); }
		const XMFLOAT4& GetTextColor() const { return m_textColor; }
		void XM_CALLCONV SetTextColor(FXMVECTOR color) { XMStoreFloat4(&m_textColor, color); }
		void SetTextColor(const XMFLOAT4A& color) { m_textColor = color; }
		void SetTextColor(const XMFLOAT4& color) { m_textColor = color; }

		void SetText(PCWSTR text) { StringCbCopyW(m_text, sizeof(m_text), text); }
		PCWSTR GetText() const { return m_text; }
	private:
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
	};
}
