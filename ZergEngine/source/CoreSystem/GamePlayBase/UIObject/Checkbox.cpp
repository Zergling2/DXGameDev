#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Checkbox.h>

using namespace ze;

constexpr float DEFAULT_CHECKBOX_SIZE = 14.0f;
constexpr float DEFAULT_TEXTBOX_WIDTH = 100.0f;
constexpr float DEFAULT_TEXTBOX_HEIGHT = 14.0f;

Checkbox::Checkbox(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_isChecked(false)
	, m_leftText(false)	// 텍스트박스의 기본 위치는 오른쪽
	, m_checkboxSize(DEFAULT_CHECKBOX_SIZE, DEFAULT_CHECKBOX_SIZE)
	, m_boxColor(ColorsLinear::OliveDrab)
	, m_checkColor(ColorsLinear::Gold)
	, m_textboxSize(DEFAULT_TEXTBOX_WIDTH, DEFAULT_TEXTBOX_HEIGHT)
	, m_textColor(ColorsLinear::WhiteSmoke)
	, m_text()
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void Checkbox::SetLeftText(bool b)
{
	if (m_leftText == b)
		return;

	m_leftText = b;

	if (m_leftText)
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	else
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}

bool Checkbox::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_checkboxSize.HitTest(pt, wcp);
}

void Checkbox::OnLButtonClick(POINT pt)
{
	m_isChecked = !m_isChecked;
}
