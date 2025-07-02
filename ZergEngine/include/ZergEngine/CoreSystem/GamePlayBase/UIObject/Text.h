#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	class Text : public ISizeColorUIObject
	{
	public:
		Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Text() = default;

		void SetText(PCWSTR text) { m_text = text; }
		void SetText(const std::wstring& text) { m_text = text; }
		void SetText(std::wstring&& text) { m_text = std::move(text); }
		PCWSTR GetText() const { return m_text.c_str(); }
	private:
		std::wstring m_text;
	};
}
