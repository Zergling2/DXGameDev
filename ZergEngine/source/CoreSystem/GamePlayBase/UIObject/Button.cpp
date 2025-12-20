#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

constexpr float BUTTON_DEFAULT_WIDTH = 200.0f;
constexpr float BUTTON_DEFAULT_HEIGHT = 75.0f;

Button::Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_isPressed(false)
	, m_size(BUTTON_DEFAULT_WIDTH, BUTTON_DEFAULT_HEIGHT)
	, m_buttonColor(ColorsLinear::DarkOliveGreen)
	, m_text()
	, m_textColor(ColorsLinear::White)
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

bool Button::HitTest(const XMFLOAT2& mousePos) const
{
	XMFLOAT2 wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(mousePos, wcp);
}

void Button::OnLButtonDown()
{
	m_isPressed = true;
}

void Button::OnLButtonUp()
{
	m_isPressed = false;
}
