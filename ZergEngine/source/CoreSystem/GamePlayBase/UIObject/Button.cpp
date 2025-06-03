#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

Button::Button(UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(flag, name, UIOBJECT_TYPE::BUTTON)
	, m_pressed(false)
{
}

void Button::OnLButtonDown()
{
	this->SetPressed(true);
}

void Button::OnLButtonUp()
{
	this->SetPressed(false);
}
