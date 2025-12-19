#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

Button::Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: Text(id, flag, name)
	, m_pressed(false)
	, m_buttonColor(Colors::Orange)
{
	this->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	this->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void Button::OnDetachedFromUIInteraction()
{
	Text::OnDetachedFromUIInteraction();

	m_pressed = false;
}

void Button::OnLButtonDown()
{
	Text::OnLButtonDown();

	this->SetPressed(true);
}

void Button::OnLButtonUp()
{
	Text::OnLButtonUp();

	this->SetPressed(false);
}
