#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

Button::Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name, UIOBJECT_TYPE::BUTTON)
	, m_pressed(false)
	, m_textColor(0.0f, 0.0f, 0.0f, 1.0f)
	, m_textLength(0)
{
	m_text[0] = L'\0';
}

void Button::SetText(PCWSTR text)
{
	StringCbCopyW(m_text, sizeof(m_text), text);
	m_textLength = static_cast<uint16_t>(wcslen(m_text));
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
