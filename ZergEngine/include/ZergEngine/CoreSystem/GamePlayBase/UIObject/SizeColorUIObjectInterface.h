#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class ISizeColorUIObject : public IUIObject
	{
	public:
		ISizeColorUIObject(UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type);
		virtual ~ISizeColorUIObject() = default;

		XMVECTOR XM_CALLCONV GetSize() const { return XMLoadFloat2(&m_size); }
		void XM_CALLCONV SetSize(FXMVECTOR size);
		void SetSize(const XMFLOAT2& size) { this->SetSize(XMLoadFloat2(&size)); }
		void SetSize(const XMFLOAT2A& size) { this->SetSize(XMLoadFloat2A(&size)); }
		XMVECTOR XM_CALLCONV GetColor() const { return XMLoadFloat4(&m_color); }
		void XM_CALLCONV SetColor(XMVECTOR color) { XMStoreFloat4(&m_color, color); }
	private:
		virtual bool XM_CALLCONV HitTest(FXMVECTOR mousePosition) const override;

		virtual void OnLButtonDown() override {}
		virtual void OnLButtonUp() override {}
	protected:
		XMFLOAT2 m_size;
		XMFLOAT2 m_halfSize;
		XMFLOAT4 m_color;
	};
}
