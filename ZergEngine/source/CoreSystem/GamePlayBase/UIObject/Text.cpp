#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>

using namespace ze;

Text::Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_size()
	, m_text()
	, m_color(ColorsLinear::WhiteSmoke)
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}
