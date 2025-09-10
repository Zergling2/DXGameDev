#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

Text::Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name)
	, m_tf(L"", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11)
	, m_textAlignment((DWRITE_TEXT_ALIGNMENT_LEADING))
	, m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	, m_spDWriteTextFormatWrapper(GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf))
{
}

void Text::Apply()
{
	m_spDWriteTextFormatWrapper = GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf);
}
