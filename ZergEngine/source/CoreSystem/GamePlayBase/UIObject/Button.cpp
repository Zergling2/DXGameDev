#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

constexpr float DEFAULT_BUTTON_WIDTH = 180.0f;
constexpr float DEFAULT_BUTTON_HEIGHT = 60.0f;

Button::Button(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_isPressed(false)
	, m_size(DEFAULT_BUTTON_WIDTH, DEFAULT_BUTTON_HEIGHT)
	, m_buttonColor(ColorsLinear::DarkOliveGreen)
	, m_text()
	, m_textColor(ColorsLinear::WhiteSmoke)
	, m_handlerOnClick()
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

bool Button::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(pt, wcp);
}

void Button::OnLButtonDown(POINT pt)
{
	m_isPressed = true;
}

void Button::OnLButtonUp(POINT pt)
{
	m_isPressed = false;
}

void Button::OnLButtonClick(POINT pt)
{
	// 지연 객체(예시: OnLoadScene)인 경우 콜백 호출 방지
	if (this->IsPending())
		return;

	// 상태 업데이트
	// 버튼은 상태 플래그 업데이트할 값 없음.

	// UI Event Callback
	if (m_handlerOnClick)
	{
		bool success = m_handlerOnClick();
		if (!success)	// 객체가 파괴된 경우
			m_handlerOnClick = nullptr;
	}
}
