#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>

namespace ze
{
	class Button : public Text
	{
		friend class Renderer;
	public:
		Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Button() = default;

		virtual UIOBJECT_TYPE GetType() const override { return UIOBJECT_TYPE::BUTTON; }

		XMVECTOR GetButtonColorVector() const { return XMLoadFloat4(&m_buttonColor); }
		const XMFLOAT4& GetButtonColor() const { return m_buttonColor; }
		void XM_CALLCONV SetButtonColor(FXMVECTOR color) { XMStoreFloat4(&m_buttonColor, color); }
		void SetButtonColor(const XMFLOAT4A& color) { m_buttonColor = color; }
		void SetButtonColor(const XMFLOAT4& color) { m_buttonColor = color; }
	private:
		virtual void OnDetachUIInteraction() override;	// 시각적으로 눌린 상태를 해제한다.

		virtual void OnLButtonDown() override;
		virtual void OnLButtonUp() override;

		// 렌더링 모양 결정을 위한 상태변수 (작동 로직과는 무관)
		bool IsPressed() const { return m_pressed; }
		void SetPressed(bool pressed) { m_pressed = pressed; }
	private:
		bool m_pressed;
		XMFLOAT4 m_buttonColor;
	};
}
