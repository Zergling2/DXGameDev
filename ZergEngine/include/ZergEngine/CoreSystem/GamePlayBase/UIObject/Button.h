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
	private:
		virtual void OnLButtonDown() override;
		virtual void OnLButtonUp() override;

		bool IsPressed() const { return m_pressed; }
		void SetPressed(bool pressed) { m_pressed = pressed; }
	private:
		bool m_pressed;
	};
}
