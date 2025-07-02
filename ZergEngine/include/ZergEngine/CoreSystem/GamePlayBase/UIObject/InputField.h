#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	class InputField : public ISizeColorUIObject
	{
	public:
		InputField(uint64_t id ,UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~InputField() = default;

		// Input Field �ڽ��� ��� ������ �����ɴϴ�.
		const XMFLOAT4& GetBkColor() const { return m_bkColor; }
		XMVECTOR XM_CALLCONV GetBkColorVector() const { return XMLoadFloat4(&m_bkColor); }

		// Input Field �ڽ��� ��� ������ �����մϴ�.
		void XM_CALLCONV SetBkColor(FXMVECTOR color) { XMStoreFloat4(&m_bkColor, color); }
		void SetBkColor(const XMFLOAT4A& color) { m_bkColor = color; }
		void SetBkColor(const XMFLOAT4& color) { m_bkColor = color; }

		void Clear() { m_text.clear(); }
		void ShrinkToFit() { m_text.shrink_to_fit(); }
	private:
		XMFLOAT4 m_bkColor;
		std::wstring m_text;
	};
}
