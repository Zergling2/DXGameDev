#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>
#include <ZergEngine\CoreSystem\Resource\Font.h>

namespace ze
{
	class Text : public ISizeColorUIObject
	{
	public:
		Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Text() = default;

		virtual UIOBJECT_TYPE GetType() const override { return UIOBJECT_TYPE::TEXT; }

		void SetText(PCWSTR text) { m_text = text; }
		void SetText(const std::wstring& text) { m_text = text; }
		void SetText(std::wstring&& text) { m_text = std::move(text); }
		PCWSTR GetText() const { return m_text.c_str(); }
		size_t GetTextLength() const { return m_text.length(); }
		TextFormat& GetTextFormat() { return m_tf; }
		DWRITE_TEXT_ALIGNMENT GetTextAlignment() const { return m_textAlignment; }
		void SetTextAlignment(DWRITE_TEXT_ALIGNMENT ta) { m_textAlignment = ta; }
		DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() const { return m_paragraphAlignment; }
		void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT pa) { m_paragraphAlignment = pa; }

		// TextFormat을 변경한 후 호출해야 새로운 폰트로 갱신됩니다.
		void Refresh();

		IDWriteTextFormat* GetDWriteTextFormatComInterface() const { return m_spDWriteTextFormatWrapper->GetDWriteTextFormatComInterface(); }
	private:
		std::wstring m_text;
		TextFormat m_tf;
		DWRITE_TEXT_ALIGNMENT m_textAlignment;
		DWRITE_PARAGRAPH_ALIGNMENT m_paragraphAlignment;
		std::shared_ptr<DWriteTextFormatWrapper> m_spDWriteTextFormatWrapper;
	};
}
