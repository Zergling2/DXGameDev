#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

Button::Button(UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(flag, name, UIOBJECT_TYPE::BUTTON)
	, m_pressed(false)
	, m_textColor(0.0f, 0.0f, 0.0f, 1.0f)
{
	m_text[0] = L'\0';
}

void Button::OnLButtonDown()
{
	this->SetPressed(true);
}

void Button::OnLButtonUp()
{
	this->SetPressed(false);
}
