#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObjectInterface.h>

namespace ze
{
	class Button : public IUIObject
	{
		friend class RendererImpl;
	public:
		Button(UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Button() = default;
	public:
		XMVECTOR XM_CALLCONV GetSize() const { return XMLoadFloat2(&m_size); }
		void XM_CALLCONV SetSize(FXMVECTOR size);
		void SetSize(const XMFLOAT2& size) { this->SetSize(XMLoadFloat2(&size)); }
		void SetSize(const XMFLOAT2A& size) { this->SetSize(XMLoadFloat2A(&size)); }
		XMVECTOR XM_CALLCONV GetColor() const { return XMLoadFloat4(&m_color); }
		void XM_CALLCONV SetColor(XMVECTOR color) { XMStoreFloat4(&m_color, color); }
	private:
		virtual bool HitTest(const XMFLOAT2A mousePos) const override;

		bool IsPressed() const { return m_pressed; }
	private:
		bool m_pressed;
		XMFLOAT2 m_size;
		XMFLOAT2 m_halfSize;
		XMFLOAT4 m_color;
	};
}
