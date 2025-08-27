#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

Button::Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: Text(id, flag, name)
	, m_pressed(false)
	, m_buttonColor(Colors::Orange)
{
}

void Button::OnDetachUIInteraction()
{
	m_pressed = false;
}

void Button::OnLButtonDown()
{
	this->SetPressed(true);
}

void Button::OnLButtonUp()
{
	this->SetPressed(false);
}
