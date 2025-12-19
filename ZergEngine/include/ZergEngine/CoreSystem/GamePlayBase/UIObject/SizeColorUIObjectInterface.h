#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeUIObjectInterface.h>

namespace ze
{
	class ISizeColorUIObject : public ISizeUIObject
	{
	public:
		ISizeColorUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~ISizeColorUIObject() = default;

		const XMFLOAT4& GetColor() const { return m_color; }
		XMVECTOR XM_CALLCONV GetColorVector() const { return XMLoadFloat4(&m_color); }
		void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_color.x = r; m_color.y = g; m_color.z = b; m_color.w = a; }
		void XM_CALLCONV SetColor(FXMVECTOR color) { XMStoreFloat4(&m_color, color); }
		void SetColor(const XMFLOAT4& color) { m_color = color; }
	private:
		XMFLOAT4 m_color;
	};
}
