#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeUIObjectInterface.h>

namespace ze
{
	class ISizeColorUIObject : public ISizeUIObject
	{
	public:
		ISizeColorUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type);
		virtual ~ISizeColorUIObject() = default;

		const XMFLOAT4& GetColor() const { return m_color; }
		XMVECTOR XM_CALLCONV GetColorVector() const { return XMLoadFloat4(&m_color); }
		void XM_CALLCONV SetColor(FXMVECTOR color) { XMStoreFloat4(&m_color, color); }
		void SetColor(const XMFLOAT4A& color) { m_color = color; }
		void SetColor(const XMFLOAT4& color) { m_color = color; }
	private:
		XMFLOAT4 m_color;
	};
}
